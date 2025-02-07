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

#include "Array.hh"
#include "PlanError.hh"
#include "PlexilTypeTraits.hh"

#include <algorithm> // std::fill()
#include <memory>    // std::move()

namespace PLEXIL
{
  Array::Array()
  {
  }

  Array::Array(Array const &orig)
    : m_known(orig.m_known)
  {
  }

  Array::Array(Array &&orig)
    : m_known(std::move(orig.m_known))
  {
  }

  Array::Array(size_t size, bool known)
  : m_known(size, known)
  {
  }

  Array::~Array()
  {
  }

  Array &Array::operator=(Array const &other)
  {
    m_known = other.m_known;
    return *this;
  }

  Array &Array::operator=(Array &&other)
  {
    m_known = std::move(other.m_known);
    return *this;
  }

  size_t Array::size() const
  {
    return m_known.size();
  }

  bool Array::elementKnown(size_t index) const
  {
    checkPlanError(checkIndex(index),
                   "Array::elementKnown: Index exceeds array size");
    return m_known[index];
  }

  void Array::resize(size_t size)
  {
    m_known.resize(size, false);
  }

  void Array::setElementUnknown(size_t index)
  {
    checkPlanError(checkIndex(index),
                   "Array::setElementUnknown: Index exceeds array size");
    m_known[index] = false;
  }

  void Array::reset()
  {
    std::fill(m_known.begin(), m_known.end(), false);
  }

  bool Array::operator==(Array const &other) const
  {
    return m_known == other.m_known;
  }

  //
  // TODO:
  // - Define boundary case size == 0 for any/allElementsKnown
  //

  bool Array::allElementsKnown() const
  {
    for (size_t i = 0; i < m_known.size(); ++i)
      if (!m_known[i])
        return false;
    return true;
  }

  bool Array::anyElementsKnown() const
  {
    for (size_t i = 0; i < m_known.size(); ++i)
      if (m_known[i])
        return true;
    return false;
  }

  // Default methods throw PlanError
#define DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(_TYPE_) \
  bool Array::getElement(size_t /* index */, _TYPE_ & /* result */) const \
  { \
    reportPlanError("Type error: can't get element of type "    \
                    << PlexilValueType<_TYPE_>::typeName        \
                    << " from array of "                        \
                    << valueTypeName(this->getElementType()));  \
    return false; \
  }

  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(Boolean)
  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(Integer)
  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(Real)
  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(String)

#undef DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD

  bool Array::getElementPointer(size_t /* index */, String const *& /* result */) const
  {
    reportPlanError("Type error: can't get pointer to String element "
                    << " from array of "
                    << valueTypeName(this->getElementType()));
    return false;
  }

#define DEFINE_SET_ELEMENT_TYPE_ERROR_METHOD(_TYPE_) \
  void Array::setElement(size_t /* index */, _TYPE_ const & /* newval */) \
  { \
    reportPlanError("Type error: can't assign element of type " \
                    << valueTypeName(PlexilValueType<_TYPE_>::value)    \
                    << " to array of "                                  \
                    << valueTypeName(this->getElementType()));          \
  }

  DEFINE_SET_ELEMENT_TYPE_ERROR_METHOD(Boolean)
  DEFINE_SET_ELEMENT_TYPE_ERROR_METHOD(Integer)
  DEFINE_SET_ELEMENT_TYPE_ERROR_METHOD(Real)
  DEFINE_SET_ELEMENT_TYPE_ERROR_METHOD(String)

#undef DEFINE_SET_ELEMENT_TYPE_ERROR_METHOD

  std::string Array::toString() const
  {
    std::ostringstream strm;
    this->print(strm);
    return strm.str();
  }

  std::ostream &operator<<(std::ostream &str, Array const &ary)
  {
    ary.print(str);
    return str;
  }

  template <>
  char *serialize<Array>(Array const &val, char *buf)
  {
    return val.serialize(buf);
  }

  template <>
  char const *deserialize<Array>(Array &val, char const *buf)
  {
    return val.deserialize(buf);
  }

  template <> size_t serialSize(Array const &val)
  {
    return val.serialSize();
  }

} // namespace PLEXIL
