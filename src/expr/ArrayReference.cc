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

#include "ArrayReference.hh"

#include "Array.hh"
#include "PlanError.hh"
#include "PlexilTypeTraits.hh"
#include "UserVariable.hh"

namespace PLEXIL
{
  ArrayReference::ArrayReference(Expression *ary,
                                 Expression *idx,
                                 bool aryIsGarbage,
                                 bool idxIsGarbage)
    : NotifierImpl(),
      m_array(ary),
      m_index(idx),
      m_arrayIsGarbage(aryIsGarbage),
      m_indexIsGarbage(idxIsGarbage),
      m_namePtr(new std::string())
  {
    m_array->addListener(this);
    m_index->addListener(this);
  }

  ArrayReference::~ArrayReference()
  {
    m_array->removeListener(this);
    m_index->removeListener(this);
    if (m_arrayIsGarbage)
      delete m_array;
    if (m_indexIsGarbage)
      delete m_index;
    delete m_namePtr;
  }

  char const *ArrayReference::getName() const
  {
    Expression const *base = getBaseExpression();
    if (base) {
      std::ostringstream s;
      s << base->getName() << '[' << m_index->valueString() << ']';
      *m_namePtr = s.str();
      return m_namePtr->c_str();
    }
    static char const *sl_dummy = "";
    return sl_dummy;
  }

  char const *ArrayReference::exprName() const
  {
    return "ArrayReference";
  }

  ValueType ArrayReference::valueType() const
  {
    ValueType aryType = m_array->valueType();
    if (!isArrayType(aryType))
      return UNKNOWN_TYPE;
    else
      return arrayElementType(aryType);
  }

  bool ArrayReference::isKnown() const
  {
    Array const *dummyAry;
    size_t dummyIdx;
    return selfCheck(dummyAry, dummyIdx);
  }

  bool ArrayReference::isConstant() const
  {
    return m_array->isConstant() && m_index->isConstant();
  }

  bool ArrayReference::isAssignable() const
  {
    return false;
  }

  Expression const *ArrayReference::getBaseExpression() const
  {
    return m_array->getBaseExpression();
  }

  Expression *ArrayReference::getBaseExpression()
  {
    return m_array->getBaseExpression();
  }

  void ArrayReference::printValue(std::ostream &s) const
  {
    Array const *ary;
    size_t idx;
    if (!selfCheck(ary, idx)) {
      s << "[unknown_value]";
      return;
    }

    // Punt for now
    s << ary->getElementValue(idx);
  }

  bool ArrayReference::selfCheck(Array const *&valuePtr,
                                 size_t &idx) const
  {
    int32_t idxTemp;
    if (!m_index->getValue(idxTemp))
      return false; // index is unknown
    checkPlanError(idxTemp >= 0, "Array index " << idxTemp << " is negative");
    idx = (size_t) idxTemp;
    if (!m_array->getValuePointer(valuePtr))
      return false; // array unknown or invalid
    std::vector<bool> const &kv = valuePtr->getKnownVector();
    checkPlanError(idx < kv.size(),
                   "Array index " << idx
                   << " equals or exceeds array size " << kv.size());
    return kv[idx];
  }

  template <typename R>
  bool ArrayReference::getValueImpl(R &result) const
  {
    Array const *ary;
    size_t idx;
    if (!selfCheck(ary, idx))
      return false;
    return ary->getElement(idx, result);
  }

  // getValueImpl explicit instantiations
  template bool ArrayReference::getValueImpl(Boolean &) const;
  template bool ArrayReference::getValueImpl(NodeState &) const;
  template bool ArrayReference::getValueImpl(NodeOutcome &) const;
  template bool ArrayReference::getValueImpl(FailureType &) const;
  template bool ArrayReference::getValueImpl(CommandHandleValue &) const;
  template bool ArrayReference::getValueImpl(Integer &) const;
  template bool ArrayReference::getValueImpl(Real &) const;
  template bool ArrayReference::getValueImpl(String &) const;

  bool ArrayReference::getValuePointer(String const *&ptr) const
  {
    Array const *ary;
    size_t idx;
    if (!selfCheck(ary, idx))
      return false;
    return ary->getElementPointer(idx, ptr);
  }

  // Error for arrays and any other types we can think of.
  template <typename T>
  bool ArrayReference::getValuePointerImpl(T const *&ptr) const
  {
    assertTrueMsg(ALWAYS_FAIL,
		  "getValuePointer: trying to get a " << PlexilValueType<T>::typeName
		  << " pointer value from an ArrayReference");
    return false;
  }

  Value ArrayReference::toValue() const
  {
    Array const *ary;
    size_t idx;
    if (!selfCheck(ary, idx))
      return Value(); // unknown
    else
      return ary->getElementValue(idx);
  }
  
  void ArrayReference::addListener(ExpressionListener *l)
  {
    if (!hasListeners()) {
      m_array->addListener(this);
      m_index->addListener(this);
    }
    NotifierImpl::addListener(l);
  }

  void ArrayReference::handleActivate()
  {
    m_array->activate();
    m_index->activate();
  }
  
  void ArrayReference::handleDeactivate()
  {
    m_array->deactivate();
    m_index->deactivate();
  }

  //
  // MutableArrayReference
  //

  MutableArrayReference::MutableArrayReference(Expression *ary,
                                               Expression *idx,
                                               bool aryIsGarbage,
                                               bool idxIsGarbage)
    : ArrayReference(ary, idx, aryIsGarbage, idxIsGarbage),
      Assignable(),
      m_mutableArray(ary->asAssignable()),
      m_saved(false)
  {
    checkPlanError(ary->isAssignable(),
                   "Can't create a writeable array reference on an In array");
  }

  MutableArrayReference::~MutableArrayReference()
  {
  }

  bool MutableArrayReference::isAssignable() const
  {
    return true;
  }

  Assignable const *MutableArrayReference::asAssignable() const
  {
    return dynamic_cast<Assignable const *>(this);
  }

  Assignable *MutableArrayReference::asAssignable()
  {
    return dynamic_cast<Assignable *>(this);
  }

  bool MutableArrayReference::mutableSelfCheck(Array *&valuePtr,
                                               size_t &idx)
  {
    int32_t idxTemp;
    if (!ArrayReference::m_index->getValue(idxTemp))
      return false; // index is unknown
    checkPlanError(idxTemp >= 0,
                   "Array index " << idxTemp << " is negative");
    idx = (size_t) idxTemp;
    if (!m_mutableArray->getMutableValuePointer(valuePtr))
      return false; // array unknown
    checkPlanError(idx < valuePtr->size(),
                   "Array index " << idx
                   << " equals or exceeds array size " << valuePtr->size());
    return true;
  }

  template <typename V>
  void MutableArrayReference::setValueImpl(V const &val)
  {
    Array *ary;
    size_t idx;
    if (!mutableSelfCheck(ary, idx))
      return;
    V oldValue;
    bool known = ary->getElement(idx, oldValue); // error here if wrong type
    bool changed = (!known || (val != oldValue));
    if (changed) {
      ary->setElement(idx, val);
      NotifierImpl::publishChange(this);
      m_mutableArray->getBaseVariable()->notifyChanged(this); // array might be alias
    }
  }

  template <typename V>
  void MutableArrayReference::setValueImpl(ArrayImpl<V> const & /* value */)
  {
    assertTrue_2(ALWAYS_FAIL, "MutableArrayReference::setValue: array types not implemented");
  }

  // Instantiations of the above
  template void MutableArrayReference::setValueImpl(Boolean const &);
  template void MutableArrayReference::setValueImpl(Real const &);
  template void MutableArrayReference::setValueImpl(String const &);

  template void MutableArrayReference::setValueImpl(BooleanArray const &);
  template void MutableArrayReference::setValueImpl(IntegerArray const &value);
  template void MutableArrayReference::setValueImpl(RealArray const &value);
  template void MutableArrayReference::setValueImpl(StringArray const &value);

  // Specialized for Integer
  void MutableArrayReference::setValue(Integer const &value)
  {
    Array *ary;
    size_t idx;
    if (!mutableSelfCheck(ary, idx))
      return;
    // Check for case of assigning integer to real array
    bool changed = false;
    switch (this->m_array->valueType()) {
    case REAL_ARRAY_TYPE: {
      double oldValue;
      bool known = ary->getElement(idx, oldValue);
      double newValue = (double) value;
      if ((changed = (!known || (oldValue != newValue))))
        ary->setElement(idx, newValue);
    }
      break;
        
    case INTEGER_ARRAY_TYPE: {
      int32_t oldValue;
      bool known = ary->getElement(idx, oldValue);
      if ((changed = (!known || (oldValue != value))))
        ary->setElement(idx, value);
    }      
      break;

    default:
      checkPlanError(ALWAYS_FAIL,
                     "Can't assign an Integer value to element of a "
                     << valueTypeName(m_array->valueType()));
      return;
    }
    if (changed) {
      NotifierImpl::publishChange(this);
      m_mutableArray->getBaseVariable()->notifyChanged(this); // array might be alias
    }
  }

  void MutableArrayReference::setValue(NodeState const &)
  {
    assertTrue_2(ALWAYS_FAIL, "MutableArrayReference::setValue: NodeState not implemented");
  }

  void MutableArrayReference::setValue(NodeOutcome const &)
  {
    assertTrue_2(ALWAYS_FAIL, "MutableArrayReference::setValue: NodeState not implemented");
  }

  void MutableArrayReference::setValue(FailureType const &)
  {
    assertTrue_2(ALWAYS_FAIL, "MutableArrayReference::setValue: NodeState not implemented");
  }

  void MutableArrayReference::setValue(CommandHandleValue const &)
  {
    assertTrue_2(ALWAYS_FAIL, "MutableArrayReference::setValue: NodeState not implemented");
  }

  void MutableArrayReference::setValue(char const *value)
  {
    setValue(String(value));
  }

  void MutableArrayReference::setValue(GetValue const &valex)
  {
    if (valex.isKnown())
      switch (valex.valueType()) {
      case BOOLEAN_TYPE: {
	Boolean b;
	valex.getValue(b);
	this->setValue(b);
      }
	break;
	
      case INTEGER_TYPE: {
	Integer i;
	valex.getValue(i);
	this->setValue(i);
      }
	break;

      case REAL_TYPE: {
	Real r;
	valex.getValue(r);
	this->setValue(r);
      }
	break;

      case STRING_TYPE: {
	String const *ptr;
	valex.getValuePointer(ptr);
	this->setValue(*ptr);
      }
	break;

      default:
	assertTrueMsg(ALWAYS_FAIL,
		      "ArrayReference:setValue: illegal or unimplemented type "
		      << valueTypeName(valex.valueType()));
	break;
      }
    else
      setUnknown();
  }

  void MutableArrayReference::setValue(Value const &value)
  {
    Array *ary;
    size_t idx;
    if (!mutableSelfCheck(ary, idx))
      return;
    Value oldValue = ary->getElementValue(idx);
    if (value != oldValue) {
      ary->setElementValue(idx, value);
      NotifierImpl::publishChange(this);
      m_mutableArray->getBaseVariable()->notifyChanged(this); // array might be alias
    }
  }

  void MutableArrayReference::setUnknown()
  {
    Array *ary;
    size_t idx;
    if (!mutableSelfCheck(ary, idx))
      return;
    bool changed = ary->elementKnown(idx);
    ary->setElementUnknown(idx);
    if (changed) {
      NotifierImpl::publishChange(this);
      m_mutableArray->getBaseVariable()->notifyChanged(this); // array might be alias
    }
  }

  bool MutableArrayReference::getMutableValuePointer(std::string *&ptr)
  {
    Array *ary;
    size_t idx;
    if (!mutableSelfCheck(ary, idx))
      return false;
    return ary->getMutableElementPointer(idx, ptr);
  }

  bool MutableArrayReference::getMutableValuePointer(Array *&ptr)
  {
    check_error_2(ALWAYS_FAIL, "MutableArrayReference::getMutableValuePointer: type error");
    return false;
  }

  bool MutableArrayReference::getMutableValuePointer(BooleanArray *&ptr)
  {
    check_error_2(ALWAYS_FAIL, "MutableArrayReference::getMutableValuePointer: type error");
    return false;
  }

  bool MutableArrayReference::getMutableValuePointer(IntegerArray *&ptr)
  {
    check_error_2(ALWAYS_FAIL, "MutableArrayReference::getMutableValuePointer: type error");
    return false;
  }

  bool MutableArrayReference::getMutableValuePointer(RealArray *&ptr)
  {
    check_error_2(ALWAYS_FAIL, "MutableArrayReference::getMutableValuePointer: type error");
    return false;
  }

  bool MutableArrayReference::getMutableValuePointer(StringArray *&ptr)
  {
    check_error_2(ALWAYS_FAIL, "MutableArrayReference::getMutableValuePointer: type error");
    return false;
  }

  void MutableArrayReference::reset()
  {
    // No-op
  }

  void MutableArrayReference::saveCurrentValue()
  {
    Array *ary;
    size_t idx;
    if (!mutableSelfCheck(ary, idx)) {
      // unknown or invalid
      m_saved = false;
      return;
    }
    m_savedValue = ary->getElementValue(idx);
    m_saved = true;
  }

  void MutableArrayReference::restoreSavedValue()
  {
    Array *ary;
    size_t idx;
    if (!mutableSelfCheck(ary, idx) || !m_saved) 
      return;
    if (m_savedValue != ary->getElementValue(idx)) {
      ary->setElementValue(idx, m_savedValue);
      NotifierImpl::publishChange(this);
      m_mutableArray->notifyChanged(this);
    }
    m_saved = false;
  }

  Value MutableArrayReference::getSavedValue() const
  {
    return Value(m_savedValue);
  }

  NodeConnector const *MutableArrayReference::getNode() const
  {
    return getBaseVariable()->getNode();
  }

  NodeConnector *MutableArrayReference::getNode()
  {
    return getBaseVariable()->getNode();
  }

  Assignable *MutableArrayReference::getBaseVariable() 
  {
    return m_mutableArray->getBaseVariable();
  }

  Assignable const *MutableArrayReference::getBaseVariable() const
  {
    return m_mutableArray->getBaseVariable();
  }

} // namespace PLEXIL

