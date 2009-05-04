#ifndef LCM_BASE_IMPL_SASEXEC_HH
#define LCM_BASE_IMPL_SASEXEC_HH

#include <telemetryDouble.h>
#include <genericResponse.h>
#include "SASAdaptor.hh"

static void
genericResponse_handler (const lcm_recv_buf_t *rbuf, const char * channel, 
                         const genericResponse * msg, void * user)
{
  SASAdaptor* server = static_cast<SASAdaptor*>(user);
  printf("Received a generic response\n");
  server->postCommandResponse(msg->name, msg->retValue[0]);
}

static void
telemetryDouble_handler (const lcm_recv_buf_t *rbuf, const char * channel, 
                       const telemetryDouble * msg, void * user)
{
  SASAdaptor* server = static_cast<SASAdaptor*>(user);
  printf ("GOT some response for telemetry: %s %d %f\n", msg->state, msg->number, msg->values[0]);
  
  //  server->postTelemetryState(msg->state, msg->number, msg->values);
  server->postTelemetryState(msg->state, 1, msg->values);
}


class LcmBaseImplSASExec
{
public:
  LcmBaseImplSASExec(lcm_t *lcm, SASAdaptor* sasAdaptor)
    : m_lcm(lcm), m_sasAdaptor(sasAdaptor)
  {
    subscribeToMessages();
  }

  ~LcmBaseImplSASExec()
  {
    unsubscribeFromMessages();
  }

  void subscribeToMessages()
  {
    genericRespSub = genericResponse_subscribe(m_lcm, "GENERICRESPONSE", 
                                               &genericResponse_handler, m_sasAdaptor);
    telDouble = telemetryDouble_subscribe(m_lcm, "TELEMETRYDOUBLE",
                                          &telemetryDouble_handler, m_sasAdaptor);
  }

  void unsubscribeFromMessages()
  {
    genericResponse_unsubscribe(m_lcm, genericRespSub);
    telemetryDouble_unsubscribe(m_lcm, telDouble);
  }

private:
  lcm_t *m_lcm;
  SASAdaptor *m_sasAdaptor;
  genericResponse_subscription_t *genericRespSub;
  telemetryDouble_subscription_t *telDouble;
};

#endif
