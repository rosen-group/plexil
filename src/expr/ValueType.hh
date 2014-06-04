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

#ifndef PLEXIL_VALUE_TYPE_HH
#define PLEXIL_VALUE_TYPE_HH

#include <iosfwd>
#include <string>
#include <vector>

namespace PLEXIL
{
  // Forward reference
  template <typename T> class ArrayImpl;

  //
  // PLEXIL expression data types
  //

  enum ValueType {
      UNKNOWN_TYPE = 0,
      // User scalar types
      BOOLEAN_TYPE,
      INTEGER_TYPE,
      REAL_TYPE,
      STRING_TYPE,
      DATE_TYPE,     // TODO: what format?
      DURATION_TYPE, //  ""    ""    ""
      // more to come
      SCALAR_TYPE_MAX,

      // User array types
      ARRAY_TYPE = 16, // Not a valid type, but an offset from scalar types
      BOOLEAN_ARRAY_TYPE,
      INTEGER_ARRAY_TYPE,
      REAL_ARRAY_TYPE,
      STRING_ARRAY_TYPE,
      // more to come?

      ARRAY_TYPE_MAX,

      // Internal types
      INTERNAL_TYPE_OFFSET = 48, // Not a valid type
      NODE_STATE_TYPE,
      OUTCOME_TYPE,
      FAILURE_TYPE,
      COMMAND_HANDLE_TYPE,
      // more?
      TYPE_MAX
    };

  // Utility functions
  const std::string &valueTypeName(ValueType ty);

  bool isUserType(ValueType ty);
  bool isInternalType(ValueType ty);

  bool isScalarType(ValueType ty);
  bool isArrayType(ValueType ty);
  ValueType arrayElementType(ValueType ty);
  ValueType arrayType(ValueType elTy);

  template <typename T>
  void printValue(ArrayImpl<T> &val, std::ostream &s);

  template <typename T>
  void printValue(T const &val, std::ostream &s);

}

#endif // PLEXIL_VALUE_TYPE_HH
