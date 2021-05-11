/* Copyright (c) 2006-2021, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//
// PLEXIL timebase implmentation based on timer_create
// Provided on Linux and several BSDs
//

#include "Timebase.hh"

#include "Debug.hh"
#include "InterfaceError.hh"
#include "TimebaseFactory.hh" // REGISTER_TIMEBASE() macro
#include "timespec-utils.hh"

#include <iomanip> // std::fixed, std::setprecision()

#if defined(HAVE_CERRNO)
#include <cerrno>
#elif defined(HAVE_ERRNO_H)
#include <errno.h>
#endif

#if defined(HAVE_CSIGNAL)
#include <csignal>
#elif defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif

#if defined(HAVE_CTIME)
#include <ctime>
#elif defined(HAVE_TIME_H)
#include <time.h>
#endif

// Local constants
#define NSEC_PER_SEC (1000000000)

namespace PLEXIL
{

  class PosixTimebase : public Timebase
  {
  public:

    PosixTimebase(WakeupFn fn, void *arg)
      : Timebase(fn, arg),
        m_interval_usec(0),
        m_started(false)
    {
      debugMsg("PosixTimebase", " constructor");
    }

    virtual ~PosixTimebase() = default;

    virtual double getTime() const
    {
      return getPosixTime();
    }

    virtual void setTickInterval(uint32_t intvl)
    {
      checkInterfaceError(!m_started,
                          "PosixTimebase: setTickInterval() called while running");
      m_interval_usec = intvl;
    }

    virtual uint32_t getTickInterval() const
    {
      return m_interval_usec;
    }

    virtual void start()
    {
      if (m_started) {
        debugMsg("PosixTimebase:start", " already running, ignored");
        return;
      }

      m_started = true;
      debugMsg("PosixTimebase:start", " entered");

      // Construct the timer
      struct sigevent event;
      event.sigev_notify = SIGEV_THREAD;
      event.sigev_value.sival_ptr = m_wakeupArg;
      event.sigev_notify_function = (void(*)(union sigval)) m_wakeupFn;
      event.sigev_notify_attributes = nullptr;

      checkInterfaceError(!timer_create(CLOCK_REALTIME,
					&event,
					&m_timer),
                          "PosixTimebase: timer_create failed, errno = "
                          << errno << ":\n " << strerror(errno));

      if (!m_interval_usec) {
        debugMsg("PosixTimebase:start", " deadline mode");
        return;
      }

      // Start a repeating timer
      struct itimerspec tymrSpec = {{0, 0}, {0, 0}};

      // Calculate repeat interval in nanosec.
      uint64_t nanos = 1000 * m_interval_usec;
      tymrSpec.it_interval.tv_sec  = nanos / NSEC_PER_SEC; // better be *integer* divide!
      tymrSpec.it_interval.tv_nsec = nanos % NSEC_PER_SEC;
      // Set initial interval to the same
      tymrSpec.it_value = tymrSpec.it_interval;

      debugMsg("PosixTimebase:start",
	       "Setting initial interval to "
	       << std::fixed << std::setprecision(6) << timespecToDouble(tymrSpec.it_value)
	       << ", repeat interval " << timespecToDouble(tymrSpec.it_interval));

      // Set the timer
      checkInterfaceError(!timer_settime(m_timer,
                                         0, // flags
                                         &tymrSpec,
                                         nullptr),
                          "PosixTimebase::start: timer_settime failed, errno = "
                          << errno << ":\n " << strerror(errno));

      debugMsg("PosixTimebase:start", " tick mode");
    }

    virtual void stop()
    {
      if (!m_started) {
        debugMsg("PosixTimebase:stop", " not running, ignored");
        return;
      }

      debugMsg("PosixTimebase:stop", " entered");

      // Whether tick or deadline, disable the timer
      struct itimerspec tymrSpec = {{0, 0}, {0, 0}};
      if (timer_settime(m_timer,
			0, // flags
			&tymrSpec,
			nullptr)) {
        warn("PosixTimebase::stop: timer_settime failed, errno = "
             << errno << ":\n " << strerror(errno));
      }

      if (timer_delete(m_timer)) {
        warn("PosixTimebase:stop: timer_delete failed, errno = "
             << errno << ":\n " << strerror(errno));
      }
      debugMsg("PosixTimebase:stop", " complete");
    }

    virtual void setTimer(double d)
    {
      if (m_interval_usec) {
        debugMsg("PosixTimebase:setTimer", " tick mode, ignoring");
        return;
      }

      debugMsg("PosixTimebase:setTimer", 
               " deadline " << std::fixed << std::setprecision(6) << d);

      // Get the wakeup time into the format timer_settime wants.
      struct itimerspec tymrSpec = {{0, 0}, {0, 0}};
      tymrSpec.it_value = doubleToTimespec(d);
      
      // Get the current time
      struct timespec now;
      checkInterfaceError(!clock_gettime(CLOCK_REALTIME, &now),
                          "PosixTimebase:setTimer: clock_gettime failed, errno = " << errno
                          << ":\n " << strerror(errno));

      // Have we missed the deadline already?
      if (tymrSpec.it_value < now) {
        // Already past the scheduled time
        debugMsg("PosixTimebase:setTimer",
                 " new value " << std::fixed << std::setprecision(6) << d
                 << " is in past, calling wakeup function now");
        m_nextWakeup = 0;
        (m_wakeupFn)(m_wakeupArg);
        return;
      }

      // Set the timer for the deadline
      checkInterfaceError(!timer_settime(m_timer,
                                         TIMER_ABSTIME, // flags
                                         &tymrSpec,
                                         nullptr),
                          "PosixTimebase::setTimer: timer_settime failed, errno = "
                          << errno << ":\n " << strerror(errno));

      // Truth in advertising
      m_nextWakeup = timespecToDouble(tymrSpec.it_value);
      debugMsg("PosixTimebase:setTimer",
               " deadline set to "
	       << std::fixed << std::setprecision(6) << m_nextWakeup);
    }

  private:

    timer_t m_timer;
    uint32_t m_interval_usec;
    bool m_started;
  };

  void registerPosixTimebase()
  {
    REGISTER_TIMEBASE(PosixTimebase, "Posix", 1000);
  }

} // namespace PLEXIL
