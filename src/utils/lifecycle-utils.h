/* Copyright (c) 2006-2021, Universities Space Research Association (USRA).
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

#ifndef LIFECYCLE_UTILS__H
#define LIFECYCLE_UTILS__H

/**
 * \file lifecycle-utils.h
 * \brief Functions to support orderly cleanup when an application exits.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * \typedef lc_operator
 * \brief Shorthand for a pointer to a function which takes a void * argument
 *        and returns void.
 * \ingroup Utils
 */
typedef void (*lc_operator)() ;

/**
 * \brief Add the given function to the list of functions to run at program exit.
 * \param op Pointer to the function.
 * \ingroup Utils
 */
void plexilAddFinalizer(lc_operator op);

/**
 * \brief Run all the functions registered by plexilAddFinalizer, 
 *        in last-in, first-out order.
 * \note The application should call this function immediately before exiting.
 * \ingroup Utils
 */
void plexilRunFinalizers();

#ifdef __cplusplus
}
#endif

#endif /* LIFECYCLE_UTILS__H */
