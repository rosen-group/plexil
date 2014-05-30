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

#ifndef PLEXIL_ASSIGNABLE_IMPL_HH
#define PLEXIL_ASSIGNABLE_IMPL_HH

#include "Assignable.hh"

namespace PLEXIL
{
  /**
   * @class AssignableShim
   * @brief CRTP shim between virtual base class Assignable and its typed derivations.
   */
  template <class IMPL>
  class AssignableShim : public Assignable
  {
  public:
    AssignableShim()
      : Assignable()
    {
    }

    ~AssignableShim()
    {
    }

    inline void setValue(bool const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(uint16_t const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(int32_t const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(double const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(std::string const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(char const *val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(BooleanArray const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(IntegerArray const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(RealArray const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline void setValue(StringArray const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    // Wrappers
    inline void setValue(ExpressionId const &valex)
    {
      static_cast<IMPL *>(this)->setValueImpl(valex);
    }

    inline void setValue(Value const &val)
    {
      static_cast<IMPL *>(this)->setValueImpl(val);
    }

    inline bool getMutableValuePointer(std::string *& ptr)
    {
      return static_cast<IMPL *>(this)->getMutableValuePointerImpl(ptr);
    }

    inline bool getMutableValuePointer(BooleanArray *& ptr)
    {
      return static_cast<IMPL *>(this)->getMutableValuePointerImpl(ptr);
    }

    inline bool getMutableValuePointer(IntegerArray *& ptr)
    {
      return static_cast<IMPL *>(this)->getMutableValuePointerImpl(ptr);
    }

    inline bool getMutableValuePointer(RealArray *& ptr)
    {
      return static_cast<IMPL *>(this)->getMutableValuePointerImpl(ptr);
    }

    inline bool getMutableValuePointer(StringArray *& ptr)
    {
      return static_cast<IMPL *>(this)->getMutableValuePointerImpl(ptr);
    }

  private:
    // No need for copy, assign
    AssignableShim(AssignableShim<IMPL> const &);
    AssignableShim &operator=(AssignableShim<IMPL> const &);
  };

  /**
   * @class AssignableImpl
   * @brief Typed implementation derived class of Assignable.
   */
  template <typename T>
  class AssignableImpl : public AssignableShim<AssignableImpl<T> >
  {
  public:
    AssignableImpl();
    ~AssignableImpl();

    // To be defined by derived classes.
    virtual void setValueImpl(T const &val) = 0;

    // Type mismatch methods.
    // Can be overridden for conversions.
    virtual void setValueImpl(char const *val);

    template <typename U>
    void setValueImpl(U const &val);

    // Delegate to typed setValueImpl() methods.
    void setValueImpl(ExpressionId const &valex);
    void setValueImpl(Value const &val);

    virtual bool getMutableValuePointerImpl(T *& ptr) = 0;

    template <typename U>
    bool getMutableValuePointerImpl(U *& ptr);

  private:
    // No need for copy, assign
    AssignableImpl(AssignableImpl const &);
    AssignableImpl &operator=(AssignableImpl const &);
  };


} // namespace PLEXIL

#endif // PLEXIL_ASSIGNABLE_IMPL_HH
