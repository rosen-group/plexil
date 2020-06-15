/* Copyright (c) 2006-2020, Universities Space Research Association (USRA).
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
#ifndef RESPONSE_MESSAGE_HH
#define RESPONSE_MESSAGE_HH

#include <string>

#if defined(HAVE_CSTDDEF)
#include <cstddef> // NULL
#elif defined(HAVE_STDDEF_H)
#include <stddef.h> // NULL
#endif

class ResponseBase;

/**
 * @brief Enumeration value representing the kind of message.
 */
enum MsgType
  {
    MSG_COMMAND=0, 
    MSG_TELEMETRY,
    MSG_LOOKUP
  };

/**
 * @brief ResponseMessage represents an outgoing message that has been scheduled for output.
 */
class ResponseMessage
{
public:
  ResponseMessage(const ResponseBase* _base,
		  void* _id = NULL, 
		  int _type=MSG_COMMAND);
  virtual ~ResponseMessage();

  const ResponseBase* getResponseBase() const;

  void* getId() const;

  int getMessageType() const;

  const std::string& getName() const;

private:  
  // deliberately not implemented
  ResponseMessage();

  const ResponseBase* base;
  void* id;
  int messageType;
};

#endif // RESPONSE_MESSAGE_HH
