/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#include "NodeTimepointValue.hh"

#include "ConcreteExpressionFactory.hh"
#include "Node.hh"
#include "PlexilPlan.hh"

namespace PLEXIL
{
  NodeTimepointValue::NodeTimepointValue(NodeId const &node,
                                         NodeState state,
                                         bool isEnd)
    : m_name(nodeStateName(state) + (isEnd ? ".END" : ".START")), // FIXME: use table lookup, don't generate
      m_node(node),
      m_state(state),
      m_end(isEnd)
  {
    m_node->getStateVariable()->addListener(this->getId());
  }

  NodeTimepointValue::~NodeTimepointValue()
  {
    m_node->getStateVariable()->removeListener(this->getId());
  }

  std::string const &NodeTimepointValue::getName() const
  {
    return m_name;
  }
   
  const char *NodeTimepointValue::exprName() const
  {
    return "NodeTimepointValue";
  }

  ValueType const NodeTimepointValue::valueType() const
  {
    return DATE_TYPE;
  }

  bool NodeTimepointValue::isKnown() const
  {
    double dummy;
    return m_node->getStateTransitionTime(m_state, m_end, dummy);
  }

  bool NodeTimepointValue::getValueImpl(double &result) const // FIXME
  {
    return m_node->getStateTransitionTime(m_state, m_end, result);
  }

  bool NodeTimepointValue::getValuePointerImpl(double const *&ptr) const // FIXME
  {
    return m_node->getStateTransitionTimePointer(m_state, m_end, ptr);
  }

  void NodeTimepointValue::print(std::ostream &s) const
  {
    // TODO
  }

  void NodeTimepointValue::printValue(std::ostream &s) const
  {
    double tym;
    if (getValueImpl(tym))
      s << tym; // FIXME: needs better format
    else
      s << UNKNOWN_STR;
  }

  // Default method is adequate for now.
  // void NodeTimepointValue::handleChange(ExpressionId src)
  // {
  //   // TODO
  // }

} // namespace PLEXIL
