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

#include "SimpleBooleanVariable.hh"

#include "Error.hh"
#include "Value.hh"

namespace PLEXIL
{

  SimpleBooleanVariable::SimpleBooleanVariable()
    : Notifier(),
    m_name(nullptr),
    m_value(false)
  {
  }

  SimpleBooleanVariable::SimpleBooleanVariable(char const *name)
    : Notifier(),
    m_name(name),
    m_value(false)
  {
  }

  // Don't delete m_name as it is a pointer to a constant string.
  SimpleBooleanVariable::~SimpleBooleanVariable()
  {
  }

  bool SimpleBooleanVariable::isPropagationSource() const
  {
    return true;
  }

  //
  // Expression API
  //

  char const *SimpleBooleanVariable::exprName() const
  {
    return "InternalVariable";
  }

  char const *SimpleBooleanVariable::getName() const
  {
    if (m_name)
      return m_name;
    return "";
  }

  void SimpleBooleanVariable::printSpecialized(std::ostream &str) const
  {
    str << getName() << ' ';
  }

  //
  // Notifier API
  //

  void SimpleBooleanVariable::handleActivate()
  {
    m_value = false;
  }

  //
  // GetValue API
  //
  
  // A SimpleBooleanVariable's value is known whenever it is active.
  bool SimpleBooleanVariable::isKnown() const
  {
    return true;
  }

  bool SimpleBooleanVariable::getValue(Boolean &result) const
  {
    result = m_value;
    return true;
  }

  //
  // SetValue API
  //

  void SimpleBooleanVariable::setValue(Boolean const &val)
  {
    if (this->isActive() && m_value != val) {
      m_value = val;
      publishChange();
    }
  }

} // namespace PLEXIL
