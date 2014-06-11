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

#include "ValueType.hh"

#include "Array.hh"

#include <cerrno>
#include <cstdlib> // for strtol()
#include <iostream>
#include <sstream>

namespace PLEXIL
{
  // Initialize type name strings
  std::string const BOOL_STR = "Boolean";
  std::string const INTEGER_STR = "Integer";
  std::string const REAL_STR = "Real";
  std::string const DATE_STR = "Date";
  std::string const DURATION_STR = "Duration";
  std::string const STRING_STR = "String";
  std::string const ARRAY_STR = "Array";
  std::string const BOOLEAN_ARRAY_STR = "BooleanArray";
  std::string const INTEGER_ARRAY_STR = "IntegerArray";
  std::string const REAL_ARRAY_STR = "RealArray";
  std::string const STRING_ARRAY_STR = "StringArray";
  std::string const NODE_STATE_STR = "NodeState";
  std::string const NODE_OUTCOME_STR = "NodeOutcome";
  std::string const NODE_FAILURE_STR = "NodeFailure";
  std::string const NODE_COMMAND_HANDLE_STR = "NodeCommandHandle";
  std::string const UNKNOWN_STR = "UNKNOWN";
  std::string const VARIABLE_STR = "Variable";
  std::string const VALUE_STR = "Value";

  const std::string &valueTypeName(ValueType ty)
  {
    switch (ty) {
    case BOOLEAN_TYPE:
      return BOOL_STR;

    case INTEGER_TYPE:
      return INTEGER_STR;

    case REAL_TYPE:
      return REAL_STR;
      
    case STRING_TYPE:
      return STRING_STR;

    case DATE_TYPE:
      return DATE_STR;

    case DURATION_TYPE:
      return DURATION_STR;

      // Array types
    case BOOLEAN_ARRAY_TYPE:
      return BOOLEAN_ARRAY_STR;

    case INTEGER_ARRAY_TYPE:
      return INTEGER_ARRAY_STR;

    case REAL_ARRAY_TYPE:
      return REAL_ARRAY_STR;

    case STRING_ARRAY_TYPE:
      return STRING_ARRAY_STR;

      // Internal types
    case NODE_STATE_TYPE:
      return NODE_STATE_STR;

    case OUTCOME_TYPE:
      return NODE_OUTCOME_STR;

    case FAILURE_TYPE:
      return NODE_FAILURE_STR;

    case COMMAND_HANDLE_TYPE:
      return NODE_COMMAND_HANDLE_STR;

    default:
      return UNKNOWN_STR;
    }
  }

  const std::string &typeNameAsValue(ValueType ty)
  {
    switch (ty) {
    case BOOLEAN_TYPE:
      static std::string const sl_boolval = BOOL_STR + VALUE_STR;
      return sl_boolval;

    case INTEGER_TYPE:
      static std::string const sl_intval = INTEGER_STR + VALUE_STR;
      return sl_intval;

    case REAL_TYPE:
      static std::string const sl_realval = REAL_STR + VALUE_STR;
      return sl_realval;
      
    case STRING_TYPE:
      static std::string const sl_stringval = STRING_STR + VALUE_STR;
      return sl_stringval;

    case DATE_TYPE:
      static std::string const sl_dateval = DATE_STR + VALUE_STR;
      return sl_dateval;

    case DURATION_TYPE:
      static std::string const sl_durval = DURATION_STR + VALUE_STR;
      return sl_durval;

      // Array types
    case BOOLEAN_ARRAY_TYPE:
      static std::string const sl_boolarrval = BOOLEAN_ARRAY_STR + VALUE_STR;
      return sl_boolarrval;

    case INTEGER_ARRAY_TYPE:
      static std::string const sl_intarrval = INTEGER_ARRAY_STR + VALUE_STR;
      return sl_intarrval;

    case REAL_ARRAY_TYPE:
      static std::string const sl_realarrval = REAL_ARRAY_STR + VALUE_STR;
      return sl_realarrval;

    case STRING_ARRAY_TYPE:
      static std::string const sl_stringarrval = STRING_ARRAY_STR + VALUE_STR;
      return sl_stringarrval;

      // Internal types
    case NODE_STATE_TYPE:
      static std::string const sl_nsval = NODE_STATE_STR + VALUE_STR;
      return sl_nsval;

    case OUTCOME_TYPE:
      static std::string const sl_outcomeval = NODE_OUTCOME_STR + VALUE_STR;
      return sl_outcomeval;

    case FAILURE_TYPE:
      static std::string const sl_failval = NODE_FAILURE_STR + VALUE_STR;
      return sl_failval;

    case COMMAND_HANDLE_TYPE:
      static std::string const sl_handleval = NODE_COMMAND_HANDLE_STR + VALUE_STR;
      return sl_handleval;

    default:
      return VALUE_STR;
    }
  }

  const std::string &typeNameAsVariable(ValueType ty)
  {
    switch (ty) {
    case BOOLEAN_TYPE:
      static std::string const sl_boolvar = BOOL_STR + VARIABLE_STR;
      return sl_boolvar;

    case INTEGER_TYPE:
      static std::string const sl_intvar = INTEGER_STR + VARIABLE_STR;
      return sl_intvar;

    case REAL_TYPE:
      static std::string const sl_realvar = REAL_STR + VARIABLE_STR;
      return sl_realvar;
      
    case STRING_TYPE:
      static std::string const sl_stringvar = STRING_STR + VARIABLE_STR;
      return sl_stringvar;

    case DATE_TYPE:
      static std::string const sl_datevar = DATE_STR + VARIABLE_STR;
      return sl_datevar;

    case DURATION_TYPE:
      static std::string const sl_durvar = DURATION_STR + VARIABLE_STR;
      return sl_durvar;

      // Array types
    case BOOLEAN_ARRAY_TYPE:
      static std::string const sl_boolarrvar = BOOLEAN_ARRAY_STR + VARIABLE_STR;
      return sl_boolarrvar;

    case INTEGER_ARRAY_TYPE:
      static std::string const sl_intarrvar = INTEGER_ARRAY_STR + VARIABLE_STR;
      return sl_intarrvar;

    case REAL_ARRAY_TYPE:
      static std::string const sl_realarrvar = REAL_ARRAY_STR + VARIABLE_STR;
      return sl_realarrvar;

    case STRING_ARRAY_TYPE:
      static std::string const sl_stringarrvar = STRING_ARRAY_STR + VARIABLE_STR;
      return sl_stringarrvar;

      // Internal types
    case NODE_STATE_TYPE:
      static std::string const sl_nsvar = NODE_STATE_STR + VARIABLE_STR;
      return sl_nsvar;

    case OUTCOME_TYPE:
      static std::string const sl_outcomevar = NODE_OUTCOME_STR + VARIABLE_STR;
      return sl_outcomevar;

    case FAILURE_TYPE:
      static std::string const sl_failvar = NODE_FAILURE_STR + VARIABLE_STR;
      return sl_failvar;

    case COMMAND_HANDLE_TYPE:
      static std::string const sl_handlevar = NODE_COMMAND_HANDLE_STR + VARIABLE_STR;
      return sl_handlevar;

    default:
      return VARIABLE_STR;
    }
  }
  
  bool isUserType(ValueType ty)
  {
    return (ty > UNKNOWN_TYPE && ty < ARRAY_TYPE_MAX);
  }
  
  bool isInternalType(ValueType ty)
  {
    return (ty > INTERNAL_TYPE_OFFSET && ty < TYPE_MAX);
  }

  bool isScalarType(ValueType ty)
  {
    return (ty > UNKNOWN_TYPE && ty < SCALAR_TYPE_MAX);
  }

  bool isArrayType(ValueType ty)
  {
    return (ty > ARRAY_TYPE && ty < ARRAY_TYPE_MAX);
  }

  ValueType arrayElementType(ValueType ty)
  {
    if (ty <= ARRAY_TYPE || ty >= ARRAY_TYPE_MAX)
      return UNKNOWN_TYPE;
    return (ValueType) (ty - ARRAY_TYPE);
  }

  ValueType arrayType(ValueType elTy)
  {
    if (elTy <= UNKNOWN_TYPE || elTy > STRING_TYPE)
      return UNKNOWN_TYPE;
    return (ValueType) (elTy + ARRAY_TYPE);
  }

  template <typename T>
  void printValue(const T &val, std::ostream &s)
  {
    s << val;
  }

  template <typename T>
  void printValue(const ArrayImpl<T> &val, std::ostream &s)
  {
    // TODO - should delegate to array itself
    s << "printValue not yet implemented for arrays";
  }

  /**
   * @brief Parse one value from the incoming stream.
   * @param s Input stream.
   * @param result Reference to the place to store the result.
   * @return True if known, false if unknown.
   * @note If false, the result variable will not be modified.
   */

  template <typename NUM>
  bool parseValue(std::string const &s, NUM &result)
    throw (ParserException)
  {
    if (s.empty() || s == "UNKNOWN")
      return false;

    NUM temp;
    std::istringstream is(s);
    is >> temp;
    checkParserException(!is.fail() && is.eof(),
                         "parseValue: \"" << s << "\" is an invalid value for this type");
    result = temp;
    return true;
  }

  template <>
  bool parseValue(std::string const &s, bool &result)
    throw (ParserException)
  {
    switch (s.length()) {
    case 0:
      return false;
      
    case 1:
      if (s == "0") {
        result = false;
        return true;
      }
      if (s == "1") {
        result = true;
        return true;
      }
      return false;

    case 4:
      if (s == "true" || s == "TRUE") {
        result = true;
        return true;
      }
      return false;

    case 5:
      if (s == "false" || s == "FALSE") {
        result = false;
        return true;
      }
      // fall thru to...

    default:
      checkParserException(s == "UNKNOWN",
                           "parseValue: \"" << s << "\" is not a valid Boolean value");
      return false;
    }
  }

  template <>
  bool parseValue<int32_t>(std::string const &s, int32_t &result)
    throw (ParserException)
  {
    if (s.empty() || s == "UNKNOWN")
      return false;

    char * ends;
    long temp = strtol(s.c_str(), &ends, 0);
    checkParserException(ends != s.c_str() && *ends == '\0',
                         "parseValue: \"" << s << "\" is an invalid value for this type");
    checkParserException(errno == 0 && temp <= INT32_MAX && temp >= INT32_MIN,
                         "parseValue: " << s << " is out of range for an integer");
    result = (int32_t) temp;
    return true;
  }

  // Empty string is valid
  template <>
  bool parseValue(std::string const &s, std::string &result)
    throw (ParserException)
  {
    result = s;
    return true;
  }

  //
  // Explicit instantiation
  //
  template void printValue(bool const &, std::ostream &);
  template void printValue(int32_t const &, std::ostream &);
  template void printValue(double const &, std::ostream &);
  template void printValue(std::string const &, std::ostream &);
  // array types NYI

  template bool parseValue(std::string const &, bool &);
  template bool parseValue(std::string const &, int32_t &);
  template bool parseValue(std::string const &, double &);
  template bool parseValue(std::string const &, std::string &);
  // array types NYI

}
