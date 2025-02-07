# PLEXIL

This is the top level directory of the PLEXIL distribution.

See https://plexil-group.github.io/plexil_docs/ for information about
this software, including its user manuals.  There is more information
in the README files found in the subdirectories.

The Versions file describes previous releases of Plexil, as well as
the latest features not yet released in a binary distribution.

The CAVEATS file describes known problems and issues in this release.

## Software requirements

The complete PLEXIL suite is maintained on recent versions of Linux
and macOS.  It should build and run on other Unix-like environments
(e.g. Microsoft Windows Subsystem for Linux, BSD family) without much
effort.

**This version of PLEXIL requires a C++14 compliant compiler!**

This release depends on the feature set introduced in the C++14
language standard.

Building the PLEXIL tool suite from source requires the following
software:

* POSIX-compliant shell (e.g. GNU bash, dash, zsh, etc.)
* GNU Make and autotools (autoconf, automake, libtool, m4) or
* CMake
* GNU gperf
* gcc/g++, clang/clang++, or other C99 and C++14 compliant compilers
* Java 8 or newer JDK (e.g. openjdk-11-jdk)
* Apache ant, including the antlr module

If you downloaded a tarball, the GNU autotools and gperf are not
needed.

The robosim example program also requires the X11 libraries freeglut,
libxi, libxmu and their header files.

## How to build PLEXIL - Simple version

To build the PLEXIL distribution:

1. To build everything, including the robosim and sample-app examples,
change to this top level directory and type:

```
make everything
```

2. If you do not wish to build the examples:

```
make tools
```

## How to run PLEXIL - Simple version

1. Add the following lines to your shell init file (e.g. `~/.bashrc`).
Set `PLEXIL_HOME` to the directory containing this README.md file.

```
export PLEXIL_HOME='/where/i/cloned/plexil'
. "$PLEXIL_HOME/scripts/plexil-setup.sh"
```

2. Source the init file you just edited:

```
. ~/.bashrc
```

3. The `plexilc` script compiles a PLEXIL plan or a Plexilscript
simulation script to its XML representation:

```
plexilc my-plan.ple
```

produces the Core Plexil file `my-plan.plx`.  Similarly,

```
plexilc my-script.pst
```

produces the Plexilscript file `my-script.psx`

4. The `plexiltest` script runs the Test Executive on a plan and a
Plexilscript.  This is the easiest way to get started learning the
PLEXIL language.

```
plexiltest -p my-plan.plx -s my-script.psx
```

5. The `plexilexec` script runs the Universal Executive on a plan and
requires an interface configuration file. See the Sourceforge
documentation for more information.

```
plexilexec -c interface-config.xml -p my-plan.plx`
```

6. The 'examples' directory contains a number of examples, and is a
   good place to start exploring.

## Problem solving

* If you have just updated your git clone from a previous build, you may
need to precede `make all` with either:

```
make clean
```

or:

```
make distclean
```

The latter is sometimes needed when make or autotools files have
changed.

## How to build PLEXIL Executive - Custom configuration with autotools

1. Create the script 'src/configure':

```
cd "$PLEXIL_HOME"
make src/configure
```

(This will have been done already if you downloaded a release tarball.)

2. Change into the 'src' directory and configure the build:

```
cd src
./configure --prefix="/where/to/install" ... options ...
```

The example below includes all the optional PLEXIL components as
built in the previous section, with binaries and libraries installed
in the PLEXIL installation directory.  You can omit or change
options as desired.

```
./configure --prefix="$PLEXIL_HOME" --disable-static --enable-ipc \
 --enable-sas --enable-test-exec --enable-udp
```

For a complete list of options, type:

```
./configure --help
```

Please see the CAVEATS file in this directory for advice on options
to `configure`.

3. Change back to the top level directory and build the system:

```
cd ..
make
```

Using CMake
-----------

The PLEXIL Executive also supports building with CMake, for simpler
integration into CMake-based projects.  CMake version 3.6 or newer is
required.  The instructions here describe an out-of-tree build, which
is the preferred approach.  In-tree builds should also work, but are
not recommended.

1. Create a build directory and change into it.

```
mkdir plexil-build
cd plexil-build
```

2. Configure the build using CMake.

```
cmake "path/to/plexil/src" -DCMAKE_INSTALL_PREFIX="/install/here" ... options ...
```

The example below includes all the optional PLEXIL components as built
in the previous section, with binaries and libraries installed in the
PLEXIL installation directory.  You can omit or change options as
desired.

```
cmake path/to/plexil/src -DCMAKE_INSTALL_PREFIX="$PLEXIL_HOME" \
 -DSTANDALONE_SIMULATOR=ON -DTEST_EXEC=ON -DUDP_ADAPTER=ON
 ```

Please see the CAVEATS file in this directory for advice on CMake options.

4. Build and install the system:

```
make install
```

## Cross-compiling the PLEXIL Executive

Cross-compilation of the PLEXIL Executive as a component of other
projects is straightforward, using either the GNU autotools or CMake.

The file `$PLEXIL_HOME/src/build-for-buildroot.sh` is an example of
cross-compiling the PLEXIL Executive for use with the `buildroot`
embedded Linux tool suite on an ARM processor.  This example uses the
GNU autotools.

Similarly, by supplying an appropriate toolchain file with the
`-DCMAKE_TOOLCHAIN_FILE` option, CMake can build the PLEXIL Executive
for targets other than the host system.
