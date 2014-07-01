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

#include "CachedValueImpl.hh"

#include "ArrayImpl.hh"
#include "Error.hh"
#include "ExternalInterface.hh"
#include "Value.hh"

namespace PLEXIL
{
  // Placeholder object
  VoidCachedValue::VoidCachedValue()
    : CachedValue()
  {
  }

  VoidCachedValue::~VoidCachedValue()
  {
  }

  CachedValue &VoidCachedValue::operator=(CachedValue const &other)
  {
    assertTrue_2(dynamic_cast<VoidCachedValue const *>(&other),
                 "VoidCachedValue: assigning from incompatible CachedValue type");
    return static_cast<CachedValue &>(*this);
  }

  ValueType const VoidCachedValue::valueType() const
  {
    return UNKNOWN_TYPE;
  }

  unsigned int VoidCachedValue::getTimestamp() const
  {
    return 0;
  }

  bool VoidCachedValue::isKnown() const
  {
    return false;
  }

  CachedValue *VoidCachedValue::clone() const
  {
    return new VoidCachedValue();
  }

  bool VoidCachedValue::operator==(CachedValue const &other) const
  {
    return !other.isKnown();
  }

  /**
   * @brief Retrieve the cached value.
   * @param The appropriately typed place to put the result.
   * @return True if known, false if unknown or invalid.
   * @note The expression value is not copied if the return value is false.
   * @note Derived classes should implement only the appropriate methods.
   */

  bool VoidCachedValue::getValue(bool &) const
  {
    return false;
  }

  bool VoidCachedValue::getValue(double &) const
  {
    return false;
  }

  bool VoidCachedValue::getValue(uint16_t &) const
  {
    return false;
  }

  bool VoidCachedValue::getValue(int32_t &) const
  {
    return false;
  }

  bool VoidCachedValue::getValue(std::string &) const
  {
    return false;
  }


  /**
   * @brief Retrieve a pointer to the (const) cached value.
   * @param ptr Reference to the pointer variable to receive the result.
   * @return True if known, false if unknown or invalid.
   * @note The pointer is not copied if the return value is false.
   * @note Derived classes should implement only the appropriate method.
   * @note Default methods return an error in every case.
   */
  bool VoidCachedValue::getValuePointer(std::string const *&ptr) const
  {
    return false;
  }

  bool VoidCachedValue::getValuePointer(Array const *&ptr) const // generic
  {
    return false;
  }

  bool VoidCachedValue::getValuePointer(BooleanArray const *&ptr) const // specific
  {
    return false;
  }

  bool VoidCachedValue::getValuePointer(IntegerArray const *&ptr) const //
  {
    return false;
  }

  bool VoidCachedValue::getValuePointer(RealArray const *&ptr) const    //
  {
    return false;
  }

  bool VoidCachedValue::getValuePointer(StringArray const *&ptr) const  //
  {
    return false;
  }


  Value VoidCachedValue::toValue() const
  {
    return Value();
  }


  /**
   * @brief Set the state to unknown.
   * @return true if changed, false otherwise.
   */
  bool VoidCachedValue::setUnknown(unsigned int /* timestamp */)
  {
    return false;
  }

  /**
   * @brief Update the cache entry with the given new value.
   * @param timestamp Sequence number.
   * @param val The new value.
   * @note The caller is responsible for deleting the object pointed to upon return.
   */
  bool VoidCachedValue::update(unsigned int /* timestamp */, bool const &val)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }
      
  bool VoidCachedValue::update(unsigned int /* timestamp */, int32_t const &val)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::update(unsigned int /* timestamp */, double const &val)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::update(unsigned int /* timestamp */, std::string const &val)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::updatePtr(unsigned int /* timestamp */, std::string const *valPtr)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::updatePtr(unsigned int /* timestamp */, BooleanArray const *valPtr)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::updatePtr(unsigned int /* timestamp */, IntegerArray const *valPtr)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::updatePtr(unsigned int /* timestamp */, RealArray const *valPtr)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::updatePtr(unsigned int /* timestamp */, StringArray const *valPtr)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  bool VoidCachedValue::update(unsigned int /* timestamp */, Value const &val)
  {
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");
    return false;
  }

  CachedValue *VOID_CACHED_VALUE()
  {
    static VoidCachedValue sl_value;
    return &sl_value;
  }

  //
  // Typed implementation
  //

  template <typename T>
  CachedValueImpl<T>::CachedValueImpl()
    : CachedValueShim<CachedValueImpl<T> >()
  {
  }

  template <typename T>
  CachedValueImpl<ArrayImpl<T> >::CachedValueImpl()
    : CachedValueShim<CachedValueImpl<ArrayImpl<T> > >()
  {
  }

  template <typename T>
  CachedValueImpl<T>::CachedValueImpl(CachedValueImpl<T> const &orig)
    : CachedValueShim<CachedValueImpl<T> >(),
      m_value(orig.m_value),
      m_timestamp(orig.m_timestamp),
      m_known(orig.m_known)
  {
  }

  template <typename T>
  CachedValueImpl<ArrayImpl<T> >::CachedValueImpl(CachedValueImpl<ArrayImpl<T> > const &orig)
    : CachedValueShim<CachedValueImpl<ArrayImpl<T> > >(),
      m_value(orig.m_value),
      m_timestamp(orig.m_timestamp),
      m_known(orig.m_known)
  {
  }

  template <typename T>
  CachedValueImpl<T>::~CachedValueImpl()
  {
  }

  template <typename T>
  CachedValueImpl<ArrayImpl<T> >::~CachedValueImpl()
  {
  }

  template <typename T>
  CachedValue &CachedValueImpl<T>::operator=(CachedValue const &other)
  {
    CachedValueImpl<T> const *otherPtr = 
      dynamic_cast<CachedValueImpl<T> const * > (&other);
    assertTrue_2(otherPtr, "CachedValue: invalid assignment from other type");
    return static_cast<CachedValue &>(operator=(*otherPtr));
  }

  template <typename T>
  CachedValue &CachedValueImpl<ArrayImpl<T> >::operator=(CachedValue const &other)
  {
    CachedValueImpl<ArrayImpl<T> > const *otherPtr = 
      dynamic_cast<CachedValueImpl<ArrayImpl<T> > const *>(&other);
    assertTrue_2(otherPtr, "CachedValue: invalid assignment from other type");
    return static_cast<CachedValue &>(operator=(*otherPtr));
  }

  template <typename T>
  CachedValueImpl<T> &CachedValueImpl<T>::operator=(CachedValueImpl<T> const &other)
  {
    m_timestamp = other.m_timestamp;
    if ((m_known = other.m_known))
      m_value = other.m_value;
    return *this;
  }

  template <typename T>
  CachedValueImpl<ArrayImpl<T> > &CachedValueImpl<ArrayImpl<T> >::operator=(CachedValueImpl<ArrayImpl<T> > const &other)
  {
    m_timestamp = other.m_timestamp;
    if ((m_known = other.m_known))
      m_value = other.m_value;
    return *this;
  }

  template <>
  const ValueType CachedValueImpl<bool>::valueType() const
  {
    return BOOLEAN_TYPE;
  }

  template <>
  const ValueType CachedValueImpl<int32_t>::valueType() const
  {
    return INTEGER_TYPE;
  }

  template <>
  const ValueType CachedValueImpl<double>::valueType() const
  {
    return REAL_TYPE;
  }

  template <>
  const ValueType CachedValueImpl<std::string>::valueType() const
  {
    return STRING_TYPE;
  }

  template <>
  const ValueType CachedValueImpl<ArrayImpl<bool> >::valueType() const
  {
    return BOOLEAN_ARRAY_TYPE;
  }

  template <>
  const ValueType CachedValueImpl<ArrayImpl<int32_t> >::valueType() const
  {
    return INTEGER_ARRAY_TYPE;
  }

  template <>
  const ValueType CachedValueImpl<ArrayImpl<double> >::valueType() const
  {
    return REAL_ARRAY_TYPE;
  }

  template <>
  const ValueType CachedValueImpl<ArrayImpl<std::string> >::valueType() const
  {
    return STRING_ARRAY_TYPE;
  }

  template <typename T>
  unsigned int CachedValueImpl<T>::getTimestamp() const
  {
    return m_timestamp;
  }

  template <typename T>
  unsigned int CachedValueImpl<ArrayImpl<T> >::getTimestamp() const
  {
    return m_timestamp;
  }

  template <typename T>
  bool CachedValueImpl<T>::isKnown() const
  {
    return m_known;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::isKnown() const
  {
    return m_known;
  }

  template <typename T>
  bool CachedValueImpl<T>::setUnknown(unsigned int timestamp)
  {
    bool wasKnown = m_known;
    m_known = false;
    m_timestamp = timestamp;
    return wasKnown;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::setUnknown(unsigned int timestamp)
  {
    bool wasKnown = m_known;
    m_known = false;
    m_timestamp = timestamp;
    return wasKnown;
  }

template <typename T>
bool CachedValueImpl<T>::operator==(CachedValue const &other) const
{
  if (!m_known && other.isKnown())
    return true;
  CachedValueImpl<T> const *otherPtr = 
    dynamic_cast<CachedValueImpl<T> const *>(&other);
  if (!otherPtr)
    return false; // different type
  if (m_known != otherPtr->m_known
      || m_value != otherPtr->m_value)
    return false; // different value
  return true;
}

template <typename T>
bool CachedValueImpl<ArrayImpl<T> >::operator==(CachedValue const &other) const
{
  if (!m_known && other.isKnown())
    return true;
  CachedValueImpl<ArrayImpl<T> > const *otherPtr = 
    dynamic_cast<CachedValueImpl<ArrayImpl<T> > const *>(&other);
  if (!otherPtr)
    return false; // different type
  if (m_known != otherPtr->m_known
      || m_value != otherPtr->m_value)
    return false; // different value
  return true;
}

  template <typename T>
  CachedValue *CachedValueImpl<T>::cloneImpl() const
  {
    return new CachedValueImpl<T>(*this);
  }

  template <typename T>
  CachedValue *CachedValueImpl<ArrayImpl<T> >::cloneImpl() const
  {
    return new CachedValueImpl<ArrayImpl<T> >(*this);
  }

  // Only implemented for scalar types (and string)
  template <typename T>
  bool CachedValueImpl<T>::updateImpl(unsigned int timestamp, T const &val)
  {
    if (!m_known || m_value != val) {
      m_value = val;
      m_known = true;
      m_timestamp = timestamp;
      return true;
    }
    return false;
  }

  // Default wrong-type methods.
  template <typename T>
  template <typename U>
  bool CachedValueImpl<T>::updateImpl(unsigned int timestamp, U const & /* val */)
  {
    assertTrue_2(ALWAYS_FAIL, "CachedValue::update: Type error");
    return false;
  }

  template <typename T>
  template <typename U>
  bool CachedValueImpl<ArrayImpl<T> >::updateImpl(unsigned int timestamp, U const & /* val */)
  {
    assertTrue_2(ALWAYS_FAIL, "CachedValue::update: Type error");
    return false;
  }

  // Type conversion method.
  template <>
  template <>
  bool CachedValueImpl<double>::updateImpl(unsigned int timestamp, int32_t const &val)
  {
    return this->updateImpl(timestamp, (double) val);
  }

  // From Value
  // Default for scalar types
  template <typename T>
  bool CachedValueImpl<T>::updateImpl(unsigned int timestamp, Value const &val)
  {
    T nativeVal;
    if (val.getValue(nativeVal))
      return this->updateImpl(timestamp, nativeVal);
    else
      return setUnknown(timestamp);
  }

  // Special case for string
  template <>
  bool CachedValueImpl<std::string>::updateImpl(unsigned int timestamp, Value const &val)
  {
    std::string const *valPtr;
    if (val.getValuePointer(valPtr))
      return this->updatePtrImpl(timestamp, valPtr);
    else
      return setUnknown(timestamp);
  }

  // Array method
  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::updateImpl(unsigned int timestamp, Value const &val)
  {
    ArrayImpl<T> const *valPtr;
    if (val.getValuePointer(valPtr))
      return this->updatePtrImpl(timestamp, valPtr);
    else
      return setUnknown(timestamp);
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::updatePtrImpl(unsigned int timestamp, ArrayImpl<T> const *ptr)
  {
    if (!m_known || m_value != *ptr) {
      m_value = *ptr;
      m_known = true;
      m_timestamp = timestamp;
      return true;
    }
    return false;
  }

  // Default wrong-type method.
  template <typename T>
  template <typename U>
  bool CachedValueImpl<T>::updatePtrImpl(unsigned int /* timestamp */, U const * /* ptr */)
  {
    assertTrue_2(ALWAYS_FAIL, "CachedValue::updatePtr: Type error");
    return false;
  }

  //
  // getValue() and friends
  //

  template <typename T>
  bool CachedValueImpl<T>::getValueImpl(T &result) const
  {
    if (m_known)
      result = m_value;
    return m_known;
  }

  // Default wrong-type method.
  template <typename T>
  template <typename U>
  bool CachedValueImpl<T>::getValueImpl(U & /* result */) const
  {
    assertTrue_2(ALWAYS_FAIL, "CachedValue::getValue: Type error");
    return false;
  }

  // Conversion method
  template <>
  template <>
  bool CachedValueImpl<int32_t>::getValueImpl(double &result) const
  {
    if (m_known)
      result = (double) m_value;
    return m_known;
  }

  // Same-type methods
  template <typename T>
  bool CachedValueImpl<T>::getValuePointerImpl(T const *&ptr) const
  {
    if (m_known)
      ptr = &m_value;
    return m_known;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::getValuePointerImpl(ArrayImpl<T> const *&ptr) const
  {
    if (m_known)
      ptr = &m_value;
    return m_known;
  }

  // Default wrong-type methods.
  template <typename T>
  template <typename U>
  bool CachedValueImpl<T>::getValuePointerImpl(U const *& /* ptr */) const
  {
    assertTrue_2(ALWAYS_FAIL, "CachedValue::getValue: Type error");
    return false;
  }

  template <typename T>
  template <typename U>
  bool CachedValueImpl<ArrayImpl<T> >::getValuePointerImpl(U const *& /* ptr */) const
  {
    assertTrue_2(ALWAYS_FAIL, "CachedValue::getValue: Type error");
    return false;
  }

  // Conversion method
  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::getValuePointerImpl(Array const *& ptr) const
  {
    if (m_known)
      ptr = dynamic_cast<Array const *>(&m_value);
    return m_known;
  }

  template <typename T>
  Value CachedValueImpl<T>::toValue() const
  {
    if (m_known)
      return Value(m_value);
    else
      return Value(0, valueType());
  }

  template <typename T>
  Value CachedValueImpl<ArrayImpl<T> >::toValue() const
  {
    if (m_known)
      return Value(m_value);
    else
      return Value(0, valueType());
  }

  //
  // Factory
  //

  CachedValue *CachedValueFactory(ValueType vtype)
  {
    switch (vtype) {
    case BOOLEAN_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<bool>());

    case INTEGER_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<int32_t>());

    case REAL_TYPE:
    case DATE_TYPE: // FIXME
    case DURATION_TYPE: // FIXME
      return static_cast<CachedValue *>(new CachedValueImpl<double>());

    case STRING_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<std::string>());

    case BOOLEAN_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<BooleanArray>());

    case INTEGER_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<IntegerArray>());

    case REAL_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<RealArray>());

    case STRING_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<StringArray>());

    default:
      assertTrue_2(ALWAYS_FAIL, "CachedValueFactory: Invalid or unimplemented value type");
      return NULL;
    }
  }

  //
  // Explicit instantiation (possibly redundant with factory above)
  //

  template class CachedValueImpl<bool>;
  template class CachedValueImpl<int32_t>;
  template class CachedValueImpl<double>;
  template class CachedValueImpl<std::string>;
  template class CachedValueImpl<BooleanArray>;
  template class CachedValueImpl<IntegerArray>;
  template class CachedValueImpl<RealArray>;
  template class CachedValueImpl<StringArray>;

} // namespace PLEXIL
