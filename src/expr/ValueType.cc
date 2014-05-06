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

namespace PLEXIL
{

  const std::string &valueTypeName(ValueType ty)
  {
    switch (ty) {
    case BOOLEAN_TYPE:
      static const std::string sl_boolean("Boolean");
      return sl_boolean;

    case INTEGER_TYPE:
      static const std::string sl_integer("Integer");
      return sl_integer;

    case REAL_TYPE:
      static const std::string sl_real("Real");
      return sl_real;
      
    case STRING_TYPE:
      static const std::string sl_string("String");
      return sl_string;

    case DATE_TYPE:
      static const std::string sl_date("Date");
      return sl_date;

    case DURATION_TYPE:
      static const std::string sl_duration("Duration");
      return sl_duration;

      // Array types
    case BOOLEAN_ARRAY_TYPE:
      static const std::string sl_boolean_array("BooleanArray");
      return sl_boolean_array;

    case INTEGER_ARRAY_TYPE:
      static const std::string sl_integer_array("IntegerArray");
      return sl_integer_array;

    case REAL_ARRAY_TYPE:
      static const std::string sl_real_array("RealArray");
      return sl_real_array;

    case STRING_ARRAY_TYPE:
      static const std::string sl_string_array("StringArray");
      return sl_string_array;

      // Internal types
    case NODE_STATE_TYPE:
      static const std::string sl_node_state("NodeState");
      return sl_node_state;

    case OUTCOME_TYPE:
      static const std::string sl_outcome("Outcome");
      return sl_outcome;

    case FAILURE_TYPE:
      static const std::string sl_failure("Failure");
      return sl_failure;

    case COMMAND_HANDLE_TYPE:
      static const std::string sl_command_handle("CommandHandle");
      return sl_failure;

    default:
      static const std::string sl_unknown("UNKNOWN_TYPE");
      return sl_unknown;
    }
  }

  template <typename T>
  void printValue(const std::vector<T> &val, std::ostream &s)
  {
    s << "#(";
    size_t len = val.size();
    for (size_t i = 0; i < len; ) {
      s << val[i];
      if (++i < len)
        s << ' ';
    }
    s << ')';
  }

  template <typename T>
  void printValue(const T &val, std::ostream &s)
  {
    s << val;
  }

}
