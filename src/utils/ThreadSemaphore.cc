/* Copyright (c) 2006-2022, Universities Space Research Association (USRA).
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

#include "plexil-config.h"

#include "ThreadSemaphore.hh"
#include "Error.hh"

#ifdef HAVE_UNISTD_H
#include <unistd.h> // POSIX configuration
#endif

// Uncomment for debugging.
//#define PLEXIL_SEMAPHORE_DEBUG

#ifdef PLEXIL_SEMAPHORE_DEBUG
// debug case
#include "Debug.hh"
#define myDebugMsg(label, msg) debugMsg(label, msg)
#else
// non-debug case
#define myDebugMsg(label, msg)
#endif

namespace PLEXIL
{

  // Forward reference
  static ThreadSemaphoreImpl *makeSemImpl();

  ThreadSemaphore::ThreadSemaphore()
    : m_impl(makeSemImpl())
  {
  }

  int ThreadSemaphore::wait()
  {
    return m_impl->wait();
  }

  int ThreadSemaphore::post()
  {
    return m_impl->post();
  }

} // namespace PLEXIL

//
// Implementation details here
//

//
// Does this platform support POSIX thread semaphores?
//

#if defined(_POSIX_SEMAPHORES) && _POSIX_SEMAPHORES >= 0
#if defined(HAVE_SEMAPHORE_H)
#define PLEXIL_USE_POSIX_SEMAPHORES
#endif
#elif defined(__MACH__)
#if defined(HAVE_MACH_SEMAPHORE_H)
#define PLEXIL_USE_MACH_SEMAPHORES // e.g. Mac OS X
#endif
#endif

#if !defined(PLEXIL_USE_POSIX_SEMAPHORES) && !defined(PLEXIL_USE_MACH_SEMAPHORES) 
#error "ThreadSemaphore is unimplemented for this platform. Contact PLEXIL support."
#endif
  
#ifdef PLEXIL_USE_POSIX_SEMAPHORES

//
// POSIX implementation
//

#include <semaphore.h>
#include <cerrno>

namespace PLEXIL
{
  class PosixSemaphore final : public ThreadSemaphoreImpl
  {
  public:
    PosixSemaphore()
    {
      int status = sem_init(&m_posix_sem, 0, 0);
      assertTrueMsg(status != -1,
                    "ThreadSemaphore (POSIX) constructor: sem_init failed, errno = "
                    << errno);
      myDebugMsg("ThreadSemaphore:ThreadSemaphore", " @ " << this);
    }

    ~PosixSemaphore()
    {
      int status = sem_destroy(&m_posix_sem);
      assertTrueMsg(status != -1,
                    "ThreadSemaphore (POSIX) destructor: sem_destroy failed, errno = "
                    << errno);
      myDebugMsg("ThreadSemaphore:~ThreadSemaphore", " @ " << this);
    }

    int wait()
    {
      myDebugMsg("ThreadSemaphore:wait", " on " << this);
      int status;

      // If the wait fails due to a signal, ignore the error (EINTR).
      // If the error is not EINTR, stop the thread.
      while (((status = sem_wait(&m_posix_sem)) == -1) && (errno == EINTR))
        continue;
    
      if (status == -1) {
        myDebugMsg("ThreadSemaphore:wait", " failed on " << this << ", errno = " << errno);
        return errno;
      }
      myDebugMsg("ThreadSemaphore:wait", " complete on " << this);
      return 0;
    }

    virtual int post()
    {
      int status = sem_post(&m_posix_sem);
      if (status == -1)
        return errno;
      else return 0;
    }

  private:
    sem_t m_posix_sem;
  };

  ThreadSemaphoreImpl *makeSemImpl()
  {
    return new PosixSemaphore();
  }

} // namespace PLEXIL

#endif // PLEXIL_USE_POSIX_SEMAPHORES

#ifdef PLEXIL_USE_MACH_SEMAPHORES

  //
  // Mach implementation
  //

#include <mach/kern_return.h>  // for KERN_ABORTED
#include <mach/mach_types.h>   // for semaphore_t, task_t
#include <mach/mach_init.h>    // mach_task_self()
#include <mach/semaphore.h>    // semaphore_signal(), semaphore_wait(), SYNC_POLICY_FIFO
#include <mach/task.h>         // semaphore_create(), semaphore_destroy()

namespace PLEXIL
{

  class MachSemaphore final : public ThreadSemaphoreImpl
  {
  public:
    MachSemaphore()
      : m_mach_owning_task(mach_task_self())
    {
      kern_return_t status = 
        semaphore_create(m_mach_owning_task,
                         &m_mach_sem,
                         SYNC_POLICY_FIFO,
                         0);
      assertTrueMsg(status == KERN_SUCCESS,
                    "MachSemaphore constructor: semaphore_create failed, status = "
                    << status);
      myDebugMsg("MachSemaphore",
                 " constructor, @ " << this << " proxy for " << m_mach_sem);
    }

    ~MachSemaphore()
    {
      myDebugMsg("MachSemaphore",
                 " destructor, @ " << this << " proxy for " << m_mach_sem);
      kern_return_t status = 
        semaphore_destroy(m_mach_owning_task, m_mach_sem);
      assertTrueMsg(status == KERN_SUCCESS,
                    "MachSemaphore destructor: semaphore_destroy failed, status = "
                    << status);
    }

    // *** N.B. There's a problem here relative to the POSIX version.
    // POSIX uniquely identifies when the sem_wait() call is interrupted
    // by a signal, and isn't documented to unblock when (e.g.)
    // pthread_cancel() is called.  Mach has a catch-all KERN_ABORTED
    // return value for both cases.
    //
    // This becomes a problem when waiting on a semaphore which is
    // posted to from a signal handler.
    int wait()
    {
      myDebugMsg("MachSemaphore:wait",
                 ' ' << this << " proxy for " << m_mach_sem);
      kern_return_t status = KERN_SUCCESS;
      do {
        status = semaphore_wait(m_mach_sem);
        myDebugMsg("MachSemaphore:wait",
                   ' ' << this << " semaphore_wait returned " << status);
      } while (status == KERN_ABORTED);
      myDebugMsg("MachSemaphore:wait", " returning " << status);
      return status;
    }

    int post()
    {
      myDebugMsg("MachSemaphore:post", " to " << this << " proxy for " << m_mach_sem);
      return (int) semaphore_signal(m_mach_sem);
    }

  private:
    semaphore_t m_mach_sem;
    task_t m_mach_owning_task;
  };

  ThreadSemaphoreImpl *makeSemImpl()
  {
    return new MachSemaphore();
  }

} // namespace PLEXIL

#endif // PLEXIL_USE_MACH_SEMAPHORES
