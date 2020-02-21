# Copyright (c) 2006-2020, Universities Space Research Association (USRA).
#  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Universities Space Research Association nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Universal targets for Plexil Makefiles.  This is not a standalone make file,
# and must be included to complete a full make file.

# This file assumes the following macros are properly defined prior to
# this file's inclusion.  Some are the programmer's responsibility, and some
# are found in "standard-defs.make"
#
#       INC - .H files
#       SRC - .C files
#       TAG - the command to create an Emacs tags table
#       SVN_FILES - a list of all files under SVN control
#       INCLUDES - "-I" flags
#       RM - command to remove files
#       TARGET - the name of the product of compilation (library or executable)
#       LIBRARY - the base name of the binary library file

##### Wrapup defs

# This works for any file suffix, e.g. .c, .cc, .cpp, .C, ...
OBJ     = $(addsuffix .o,$(basename $(SRC)))
DEPS    = $(OBJ:.o=.d)

##### Internal Targets -- not typically invoked explicitly.

ifneq ($(LIBRARY),)

ifneq ($(PLEXIL_SHARED),)
## Build a shared library (SHLIB)

SHLIB	= lib$(LIBRARY)$(SUFSHARE)

plexil-default: $(LIB_DIR)/$(SHLIB)

$(LIB_DIR)/$(SHLIB): $(SHLIB)
	@ if [ -d $(LIB_DIR) ] ; \
	then \
	 $(RM) $(LIB_DIR)/$(SHLIB) ; \
	else \
	 $(MKDIR) $(LIB_DIR) ; \
	fi
	$(CP) $(SHLIB) $(LIB_DIR)/$(SHLIB)

$(SHLIB): $(OBJ)
	$(LD) $(SHARED_FLAGS) $(EXTRA_LD_SO_FLAGS) $(EXTRA_FLAGS) -o $(SHLIB) $(OBJ) $(LIB_PATH_FLAGS) $(LIB_FLAGS)

localclean::
	-$(RM) $(SHLIB) $(LIB_DIR)/$(SHLIB)
endif

ifneq ($(PLEXIL_STATIC),)
## Build an archive library (.a file)

ARCHIVE = lib$(LIBRARY).a

plexil-default: $(LIB_DIR)/$(ARCHIVE)

$(LIB_DIR)/$(ARCHIVE): $(ARCHIVE)
	@ if [ -d $(LIB_DIR) ] ; \
	then \
	 $(RM) $(LIB_DIR)/$(ARCHIVE) ; \
	else \
	 $(MKDIR) $(LIB_DIR) ; \
	fi
	$(CP) $(ARCHIVE) $(LIB_DIR)/$(ARCHIVE)

# This will update an existing archive library with any object files newer
# than it, or create the library from existing objects if it does not exist.

$(ARCHIVE): $(OBJ)
	$(AR) crus $(ARCHIVE) $(OBJ)

localclean::
	-$(RM) $(ARCHIVE) $(LIB_DIR)/$(ARCHIVE)
endif

endif # $(LIBRARY)

ifneq ($(EXECUTABLE),)

plexil-default: executable

# handle case of multiple targets in EXECUTABLE
# see src/interfaces/Sockets/test/Makefile
executable $(foreach exec,$(EXECUTABLE),$(BIN_DIR)/$(exec)): $(EXECUTABLE) $(BIN_DIR)
	$(CP) $(EXECUTABLE) $(BIN_DIR)

$(BIN_DIR):
	$(MKDIR) -p $(BIN_DIR)

## Build an executable
# note that this does NOT yet correctly handle multiple targets in EXECUTABLE!
$(EXECUTABLE): $(OBJ)
	$(LD) $(EXTRA_EXE_FLAGS) $(EXTRA_FLAGS) -o $(EXECUTABLE) $(OBJ) $(LIB_PATH_FLAGS) $(LIB_FLAGS)

localclean::
	-$(RM) $(EXECUTABLE) $(foreach e,$(EXECUTABLE),$(BIN_DIR)/$(e))
endif

##### Delete extraneous by-products of compilation.

localdust::
	$(RM) $(OBJ) $(DEPS)

##### Rebuild an Emacs tags table (the TAGS file).

tags:	$(SVN_FILES)
	$(TAG) $?

##### Generate documentation

#doc: $(DOC)


##### Test Directory
# These targets apply to any directory that has a 'test' subdirectory.

## Build library and test directory
all: plexil-default

## Build test directory
test: plexil-default
	@ if [ -d test ]; \
	then \
		$(MAKE) -C test; \
	fi

## Clean module and test directories
clean: dust localclean
	@ if [ -d test ]; \
	then \
		$(MAKE) -C test $@; \
	fi

## Dust module and test directories
dust: localdust
	@ if [ -d test ]; \
	then \
		$(MAKE) -C test $@; \
	fi

##### Dependencies

# Automatic generation of dependency files
# See http://scottmcpeak.com/autodepend/autodepend.html and
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

%.o: %.c
%.o: %.c %.d
	@$(COMPILE.c) $< -o $*.o
	@$(CC) $(MKDEP_FLAGS) $(CPPFLAGS) $(CFLAGS) $< | sed -e '1s|$@:|$@ $*.d:|' > $*.d

%.o: %.cc
%.o: %.cc %.d
	@$(COMPILE.cc) $< -o $*.o
	@$(CXX) $(MKDEP_FLAGS) $(CPPFLAGS) $(CXXFLAGS) $< | sed -e '1s|$@:|$@ $*.d:|' > $*.d

%.o: %.cpp
%.o: %.cpp %.d
	@$(COMPILE.cpp) $< -o $*.o
	@$(CXX) $(MKDEP_FLAGS) $(CPPFLAGS) $(CXXFLAGS) $< | sed -e '1s|$@:|$@ $*.d:|' > $*.d

%.o: %.C
%.o: %.C %.d
	@$(COMPILE.C) $< -o $*.o
	@$(CXX) $(MKDEP_FLAGS) $(CPPFLAGS) $(CXXFLAGS) $< | sed -e '1s|$@:|$@ $*.d:|' > $*.d

%.d: ;

-include $(DEPS)

.PHONY: all plexil-default clean dust localclean localdust
