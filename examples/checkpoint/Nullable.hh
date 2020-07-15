/* Copyright (c) 2006-2020, Universities Space Research Association (USRA).
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

#include <stdexcept>
#ifndef _H_Nullable
#define _H_Nullable

// This is a class that wraps objects into nullable objects
template<typename T>
class Nullable{
public:
  Nullable(T t) : data(t),      some(true) {}
  Nullable()    : some(false){}
  Nullable(const Nullable<T> &o) : data(o.data), some(o.some){}

  
  //TODO: Possibly free data
  ~Nullable();

  Nullable<T> operator= (Nullable<T> &o){
    this.data = o.data;
    this.some = o.some;
  }
  Nullable<T> operator= (T t){
    data = t;
    some = true;
  }

  bool has_value(){
    return some;
  }

  T value(){
    if(some){
      return data;
    }
    else{
      throw std::logic_error("attempting to get nulled object");
    }
  }

  T value_or(T alternate){
    return some?data:alternate;
  }

  void nullify(){
    some = false;
  }

  
private:
  T data;
  bool some;
};
#endif
