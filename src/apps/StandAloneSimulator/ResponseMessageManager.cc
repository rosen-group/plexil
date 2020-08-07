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
#include "ResponseMessageManager.hh"
#include "ResponseMessage.hh"
#include "ResponseBase.hh"

#include <sys/time.h>

ResponseMessageManager::ResponseMessageManager(const std::string& id)
  : m_Identifier(id), 
    m_DefaultResponse(NULL)
{
}

ResponseMessageManager::~ResponseMessageManager()
{
  delete m_DefaultResponse;
}

const std::string& ResponseMessageManager::getIdentifier() const 
{
  return m_Identifier;
}

const ResponseBase* ResponseMessageManager::getDefaultResponse()
{
  return m_DefaultResponse;
}

//
// Default methods
//

/**
 * @brief Schedule the events dictated by this manager.
 * @note The default method does nothing.
 */ 
void ResponseMessageManager::scheduleInitialEvents(Simulator* /* sim */)
{
}

/**
 * @brief Report that this message has been sent.
 * @note The default method does nothing.
 */
void ResponseMessageManager::notifyMessageSent(const ResponseBase* /* resp */)
{
}

const ResponseBase* ResponseMessageManager::getLastResponse() const
{
  return NULL;
}

const ResponseBase* ResponseMessageManager::getResponses(timeval& tDelay)
{
  return NULL;
}
