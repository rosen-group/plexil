## Copyright (c) 2006-2022, Universities Space Research Association (USRA).
##  All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##     * Redistributions of source code must retain the above copyright
##       notice, this list of conditions and the following disclaimer.
##     * Redistributions in binary form must reproduce the above copyright
##       notice, this list of conditions and the following disclaimer in the
##       documentation and/or other materials provided with the distribution.
##     * Neither the name of the Universities Space Research Association nor the
##       names of its contributors may be used to endorse or promote products
##       derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
## WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
## MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
## DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
## INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
## BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
## OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
## ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
## TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
## USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# TCA-IPC utilities library submodule for use with PlexilExec

add_library(IpcUtils ${PlexilExec_SHARED_OR_STATIC}
  IpcFacade.cc)

install(TARGETS IpcUtils
  DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(PlexilExec_SHLIB_INSTALL_RPATH)
  set_target_properties(IpcUtils
    PROPERTIES INSTALL_RPATH ${PlexilExec_SHLIB_INSTALL_RPATH})
endif()

target_include_directories(IpcUtils PUBLIC
  ${PlexilExec_SOURCE_DIR}/utils
  ${PlexilExec_SOURCE_DIR}/value
  ${PlexilExec_SOURCE_DIR}/third-party/ipc/src
  )

target_link_libraries(IpcUtils PUBLIC
  PlexilUtils PlexilValue ipc
  )

# Public includes
install(FILES
  IpcFacade.hh ipc-data-formats.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
