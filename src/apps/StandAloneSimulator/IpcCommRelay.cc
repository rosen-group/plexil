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

#include "IpcCommRelay.hh"
#include "ResponseMessage.hh"
#include "GenericResponse.hh"
#include "Simulator.hh"
#include "Value.hh"

#include "Debug.hh"
#include "Error.hh"
#include "ThreadSpawn.hh"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/**
 * @brief Constructor. Opens the connection and spawns a listener thread.
 */
IpcCommRelay::IpcCommRelay(const std::string& id)
  : CommRelayBase(id),
    m_ipcFacade(),
    m_stateUIDMap(),
    m_listener(*this)
{
}

/**
 * @brief Destructor. Shuts down the listener thread and closes the connection.
 */
IpcCommRelay::~IpcCommRelay() {
}

//! Start IPC.
//! @param centralhost The location of the IPC 'central' server.
//! @return True if succeeded, false if not.
bool IpcCommRelay::initialize(const std::string& centralhost)
{
  if (IPC_OK != m_ipcFacade.initialize(m_Identifier.c_str(), centralhost.c_str())) {
    warn("IpcCommRelay: Unable to initialize ipc to central server at " << centralhost);
    return false;
  }

  // Spawn listener thread
  if (IPC_OK != m_ipcFacade.start()) {
    warn("IpcCommRelay constructor: Unable to start IPC dispatch thread");
    return false;
  }

  // Subscribe only to messages we care about
  m_ipcFacade.subscribe(&m_listener, PlexilMsgType_Command);
  m_ipcFacade.subscribe(&m_listener, PlexilMsgType_LookupNow);

  debugMsg("IpcCommRelay:initialise", " succeeded");
  return true;
}

/**
 * @brief Send a response from the sim back to the UE.
 */

// *** TODO: isolate this method from the format of the response base!

void IpcCommRelay::sendResponse(const ResponseMessage* respMsg) {
  // Get the response message
  const GenericResponse* gr = dynamic_cast<const GenericResponse*> (respMsg->getResponseBase());
  assertTrueMsg(gr,
                "IpcCommRelay::sendResponse: invalid ResponseBase object");
  const std::vector<PLEXIL::Value>& values = gr->getReturnValue();
  std::vector<PLEXIL::Value> ret_list(values.begin(), values.end());

  // Format the leader
  switch (respMsg->getMessageType()) {
  case MSG_COMMAND:
  case MSG_LOOKUP: {
    // Return values message
    debugMsg("IpcCommRelay:sendResponse",
        " sending " << values.size() << " return value(s) for "
        << ((respMsg->getMessageType() == MSG_COMMAND) ? "command" : "lookup")
        << " \"" << respMsg->getName() << "\"");
    const IpcMessageId* transId = static_cast<const IpcMessageId*> (respMsg->getId());
    m_ipcFacade.publishReturnValues(transId->second, transId->first, values.front());
  }
    break;

  case MSG_TELEMETRY: {
    // Telemetry values message
    debugMsg("IpcCommRelay:sendResponse",
        " sending telemetry message for \"" << respMsg->getName() << "\"");
    m_ipcFacade.publishTelemetry(respMsg->getName(), ret_list);
  }
    break;

  default:
    errorMsg("IpcCommRelay::sendResponse: invalid message type "
             << respMsg->getMessageType());
    break;    
  }
  debugMsg("IpcCommRelay:sendResponse", " completed");
}

/**
 * @brief Send a command to the simulator
 */
void IpcCommRelay::processCommand(const std::vector<PlexilMsgBase*>& msgs) {
  std::string cmdName(((const PlexilStringValueMsg*) msgs[0])->stringValue);
  IpcMessageId* transId = new IpcMessageId(msgs[0]->senderUID, msgs[0]->serial);
  m_Simulator->scheduleResponseForCommand(cmdName, static_cast<void*> (transId));
}

/**
 * @brief Deal with a LookupNow request
 */
void IpcCommRelay::processLookupNow(const std::vector<PlexilMsgBase*>& msgs) {
  std::string stateName(((const PlexilStringValueMsg*) msgs[0])->stringValue);
  debugMsg("IpcCommRelay:lookupNow", " for " << stateName);
  if (msgs[0]->count != 0)
    debugMsg("IpcCommRelay:lookupNow",
        " ignoring parameters for state \"" << stateName << "\"");
  IpcMessageId* transId = new IpcMessageId(msgs[0]->senderUID, msgs[0]->serial);
  ResponseMessage* response = m_Simulator->getLookupNowResponse(stateName, static_cast<void*> (transId));
  if (response) {
    // Simply send the response
    debugMsg("IpcCommRelay:lookupNow", " sending response for " << stateName);
  } else {
    // Create a bogus response that returns 0 values (i.e. unknown)
    debugMsg("IpcCommRelay:lookupNow", " " << stateName << " not found, returning UNKNOWN");
    static GenericResponse gr(std::vector<PLEXIL::Value>(1));
    response = new ResponseMessage(&gr, static_cast<void*> (transId), MSG_LOOKUP);
  }
  // Simply send the response
  sendResponse(response); // deletes response
}


IpcCommRelay::MessageListener::MessageListener(IpcCommRelay& adapter) :
  m_adapter(adapter) {
}
IpcCommRelay::MessageListener::~MessageListener() {
}
void IpcCommRelay::MessageListener::ReceiveMessage(const std::vector<PlexilMsgBase*>& msgs) {
  PlexilMsgBase* leader = msgs[0];
  switch (leader->msgType) {
  case PlexilMsgType_Command:
    m_adapter.processCommand(msgs);
    break;

  case PlexilMsgType_LookupNow:
    m_adapter.processLookupNow(msgs);
    break;

  default:
    errorMsg("IpcCommRelay::processMessageSequence: invalid leader message type "
             << leader->msgType);
    break;
  }
}
