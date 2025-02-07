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

project(PlexilExec
  DESCRIPTION "An executive for the PLEXIL language"
  LANGUAGES CXX C)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Create plexil-config.h from platform characteristics and options
#

include(plexil-config.cmake)

install(FILES ${CMAKE_BINARY_DIR}/plexil-config.h
  DESTINATION include)

# This is the build time location of plexil-config.h
include_directories(${CMAKE_BINARY_DIR})

# TODO: create compile-time environment file plexil-config.env

if(BUILD_SHARED_LIBS)
  # Set default RUNPATH for executables, and libraries if needed
  if(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    set(PlexilExec_EXE_INSTALL_RPATH "@executable_path/../${CMAKE_INSTALL_LIBDIR}"
      CACHE STRING
      "The rpath value to set when installing Plexil executables")
  else()
    set(PlexilExec_EXE_INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
      CACHE STRING
      "The rpath value to set when installing Plexil executables")
    set(PlexilExec_SHLIB_INSTALL_RPATH "\$ORIGIN"
      CACHE STRING
      "The rpath value to set when installing Plexil shared libraries")
  endif()
endif()

#
# pugixml as an external project
#

# From https://cliutils.gitlab.io/modern-cmake/chapters/projects/submodule.html
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/third-party/pugixml/CMakeLists.txt")
    message(FATAL_ERROR "Submodule pugixml not found at ${CMAKE_CURRENT_LIST_DIR}/third-party/pugixml")
endif()

if(BUILD_SHARED_LIBS)
  set(PlexilExec_SHARED_OR_STATIC SHARED)
else()
  set(PlexilExec_SHARED_OR_STATIC STATIC)
endif()

# Submodules have their own shared libraries
# and unit test executables
add_subdirectory(utils)
add_subdirectory(value)
add_subdirectory(expr)
add_subdirectory(intfc)
add_subdirectory(exec)
add_subdirectory(third-party/pugixml)
add_subdirectory(xml-parser)
add_subdirectory(app-framework)

# Optional modules
if(PLAN_DEBUG_LISTENER)
  add_subdirectory(interfaces/PlanDebugListener)
endif()
if(VIEWER_LISTENER)
  add_subdirectory(interfaces/Sockets)
  add_subdirectory(interfaces/LuvListener)
endif()
if(TEST_EXEC)
  add_subdirectory(apps/TestExec)
endif()
if(UNIVERSAL_EXEC)
  add_subdirectory(universal-exec)
endif()
if(UDP_ADAPTER)
  add_subdirectory(interfaces/UdpAdapter)
endif()

if(IPC_ADAPTER)
  add_subdirectory(third-party/ipc)
  add_subdirectory(interfaces/IpcUtils)
  add_subdirectory(interfaces/IpcAdapter)

  if(STANDALONE_SIMULATOR)
    add_subdirectory(apps/StandAloneSimulator)
  endif()
endif()

# Ensure entire system is built with same compiler settings

get_directory_property(PlexilExec_SUBDIRS DIRECTORY ${CMAKE_SOURCE_DIR} SUBDIRECTORIES)
foreach(dir ${PlexilExec_SUBDIRS})
  get_directory_property(DIR_TARGETS DIRECTORY ${dir} BUILDSYSTEM_TARGETS)
  list(APPEND PlexilExec_TARGETS ${DIR_TARGETS})
endforeach()

foreach(TGT ${PlexilExec_TARGETS})
  get_target_property(TGT_FEATURES ${TGT} INTERFACE_COMPILE_FEATURES)
  if(NOT ${TGT_FEATURES} MATCHES "NOTFOUND")
    list(APPEND PlexilExec_ALL_COMPILE_FEATURES ${TGT_FEATURES})
  endif()
endforeach()

if(PlexilExec_ALL_COMPILE_FEATURES)
  list(REMOVE_DUPLICATES PlexilExec_ALL_COMPILE_FEATURES)

  foreach(TGT ${PlexilExec_TARGETS})
    target_compile_features(${TGT} PUBLIC ${PlexilExec_ALL_COMPILE_FEATURES})
  endforeach()
endif()
