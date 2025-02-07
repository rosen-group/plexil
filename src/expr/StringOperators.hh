// Copyright (c) 2006-2021, Universities Space Research Association (USRA).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Universities Space Research Association nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef PLEXIL_STRING_OPERATORS_HH
#define PLEXIL_STRING_OPERATORS_HH

#include "Expression.hh"
#include "OperatorImpl.hh"

namespace PLEXIL
{

  //! \class StringConcat
  //! \brief Implements the Concat operator.
  //! \ingroup Expressions
  class StringConcat final : public OperatorImpl<String>
  {
  public:

    //! \brief Virtual destructor.
    virtual ~StringConcat() = default;

    //! \brief Check that the number of arguments is valid for this Operator.
    //! \param count The number of arguments.
    //! \return true if valid, false if not.
    bool checkArgCount(size_t count) const override;

    //! \brief Check that the argument types are valid for this Operator.
    //! \param typeVec The vector of argument types.
    //! \return true if valid, false if not.
    bool checkArgTypes(std::vector<ValueType> const &typeVec) const override;

    //! \brief Perform the operation, and store the result.
    //! \param result Reference to the result variable.
    //! \return true if the result is known, false if not.
    //! \note Not sure if this is actually used.
    bool operator()(String &result) const;

    //! \brief Perform the operation on the expression, and store the result.
    //! \param result Reference to the result variable.
    //! \param arg Const pointer to the argument Expression.
    //! \return true if the result is known, false if not.
    bool operator()(String &result, Expression const *arg) const override;

    //! \brief Perform the operation on the expressions, and store the result.
    //! \param result Reference to the result variable.
    //! \param argA Const pointer to the first subexpression.
    //! \param argB Const pointer to the second subexpression.
    //! \return true if the result is known, false if not.
    bool operator()(String &result,
                    Expression const *argA,
                    Expression const *argB) const override;

    //! \brief Perform the operation on the Function's subexpressions, and store the result.
    //! \param result Reference to the result variable.
    //! \param args Const reference to the Function.
    //! \return true if the result is known, false if not.
    bool operator()(String &result, Function const &args) const override;

    //! \brief Singleton accessor.
    DECLARE_OPERATOR_STATIC_INSTANCE(StringConcat);

  private:

    //! \brief Private default constructor.  Accessible to singleton accessor only.
    StringConcat();

    // Disallow copy, assignment
    StringConcat(StringConcat const &) = delete;
    StringConcat(StringConcat &&) = delete;
    StringConcat &operator=(StringConcat const &) = delete;
    StringConcat &operator=(StringConcat &&) = delete;
  };

  class StringLength final : public OperatorImpl<Integer>
  {
  public:

    //! \brief Virtual destructor.
    virtual ~StringLength() = default;

    //! \brief Check that the number of arguments is valid for this Operator.
    //! \param count The number of arguments.
    //! \return true if valid, false if not.
    bool checkArgCount(size_t count) const override;

    //! \brief Check that the argument types are valid for this Operator.
    //! \param typeVec The vector of argument types.
    //! \return true if valid, false if not.
    bool checkArgTypes(std::vector<ValueType> const &typeVec) const override;

    //! \brief Operate on the given Expression, and store the result in a variable.
    //! \param result Reference to the variable.
    //! \param arg Const pointer to the expression.
    //! \return true if the result is known, false if not.
    //! \note Delegates to the corresponding calc() method.
    bool operator()(Integer &result, Expression const *arg) const override;

    //! \brief Singleton accessor.
    DECLARE_OPERATOR_STATIC_INSTANCE(StringLength);

  private:

    //! \brief Private default constructor.  Accessible to singleton accessor only.
    StringLength();

    // Disallow copy, assignment
    StringLength(const StringLength &) = delete;
    StringLength(StringLength &&) = delete;
    StringLength &operator=(const StringLength &) = delete;
    StringLength &operator=(StringLength &&) = delete;
  };

  // more to come

} // namespace PLEXIL

#endif // PLEXIL_STRING_OPERATORS_HH
