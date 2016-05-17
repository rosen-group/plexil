/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

#include "ArrayFwd.hh"
#include "Expression.hh"
#include "NodeOperatorImpl.hh"
#include "PlexilTypeTraits.hh"

namespace PLEXIL
{
  // Default method for unspecialized types
  template <typename R>
  ValueType NodeOperatorImpl<R>::valueType() const
  {
    return PlexilValueType<R>::value;
  }

  // Convenience methods

  template <typename R>
  bool NodeOperatorImpl<R>::calcNative(void *cache, Node const *node) const
  {
    return (*this)(*(static_cast<R *>(cache)), node);
  }

  template <typename R>
  bool NodeOperatorImpl<ArrayImpl<R> >::calcNative(void *cache, Node const *node) const
  {
    return (*this)(*(static_cast<ArrayImpl<R> *>(cache)), node);
  }

  template <typename R>
  void NodeOperatorImpl<R>::printValue(std::ostream &s, void *cache, Node const *node) const
  {
    if (calcNative(cache, node))
      PLEXIL::printValue(*(static_cast<R const *>(cache)), s);
    else
      s << "[unknown_value]";
  }

  template <typename R>
  void NodeOperatorImpl<ArrayImpl<R> >::printValue(std::ostream &s, void *cache, Node const *node) const
  {
    if (calcNative(cache, node))
      PLEXIL::printValue(*(static_cast<ArrayImpl<R> const *>(cache)), s);
    else
      s << "[unknown_value]";
  }

  template <typename R>
  Value NodeOperatorImpl<R>::toValue(void *cache, Node const *node) const
  {
    bool known = calcNative(cache, node);
    if (known)
      return Value(*(static_cast<R const *>(cache)));
    else
      return Value();
  }

  template <typename R>
  Value NodeOperatorImpl<ArrayImpl<R> >::toValue(void *cache, Node const *node) const
  {
    bool known = calcNative(cache, node);
    if (known)
      return Value(*(static_cast<ArrayImpl<R> const *>(cache)));
    else
      return Value();
  }

  // Default methods
  template <typename R>
  bool NodeOperatorImpl<R>::calc(R &result, Node const * /* node */) const
  {
    assertTrueMsg(ALWAYS_FAIL,
                  "Operator " << this->getName() << " not implemented for this return type");
    return false;
  }

  template <typename R>
  bool NodeOperatorImpl<ArrayImpl<R> >::calc(ArrayImpl<R> &result, Node const * /* node */) const
  {
    assertTrueMsg(ALWAYS_FAIL,
                  "Operator " << this->getName() << " not implemented for this return type");
    return false;
  }

  // Conversion methods
 
  template <>
  template <>
  bool NodeOperatorImpl<Integer>::calc(Real &result, Node const *node) const
  {
    Integer temp;
    if (!this->calc(temp, node))
      return false;
    result = (Real) temp;
    return true;
  }

  //
  // Explicit instantiations
  //

  template class NodeOperatorImpl<Boolean>;
  template class NodeOperatorImpl<NodeState>;
  template class NodeOperatorImpl<NodeOutcome>;
  template class NodeOperatorImpl<FailureType>;
  template class NodeOperatorImpl<CommandHandleValue>;
  template class NodeOperatorImpl<Integer>;
  template class NodeOperatorImpl<Real>;
  template class NodeOperatorImpl<String>;

  // later?
  // template class NodeOperatorImpl<BooleanArray>;
  // template class NodeOperatorImpl<IntegerArray>;
  // template class NodeOperatorImpl<RealArray>;
  // template class NodeOperatorImpl<StringArray>;

} // namespace PLEXIL
