#!/bin/bash

echo "This script is deprecated and will be removed in a future release!"
echo "Please use configure and make instead"
sleep 3

set -e

function usage() {
    echo "usage: build.bash [ <option> ... ]"
    echo "Build Circle with newlib standard C library."
    echo
    echo "Options:"
    echo "  -c, --clean                    clean build results and exit"
    echo "  -d, --debug                    build with debug information, without optimizer"
    echo "  -h, --help                     show usage message"
    echo "  -n, --no-cpp                   do not support C++ standard library"
    echo "  --no-parallel-build            don't do parallel build"
    echo "  -p <string>, --prefix <string> prefix of the toolchain commands (default: arm-none-eabi-)"
    echo "  -r <number>, --raspberrypi <number>"
    echo "                                 Circle Raspberry Pi model number (1, 2, 3, default: 1)"
    echo "  --softfp                       use float ABI setting \"softfp\" instead of \"hard\""
    echo "  -s <path>, --stddefpath <path>"
    echo "                                 path where stddef.h header is located (only necessary"
    echo "                                 if  script cannot determine it automatically)"
    echo "  --opt-tls                      build with mbed TLS support"
}

# From the bash FAQ: How to use pathnames relative to the script
cd "${BASH_SOURCE%/*}" || exit 1
TOPDIR="$PWD"

TEMP=$(getopt -o cdhnp:r:s: --long clean,debug,help,no-circle-build,no-cpp,no-parallel-build,opt-tls,prefix:,raspberrypi:,script-debug,softfp,stddefpath: \
     -n 'build.bash' -- "$@")

if [ $? != 0 ] ; then echo usage; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

CLEAN=0
DEBUG=0
RASPBERRYPI=1
CIRCLE_BUILD=1
NEWLIB_BUILD=1
CLEAN=0
TOOLPREFIX=arm-none-eabi-
STDDEF_INCPATH=""
STDLIB_SUPPORT=3
PARALLEL="-j"
FLOAT_ABI=hard
OPTTLS=0

NEWLIB_INSTALL_DIR="$TOPDIR/install"
NEWLIB_BUILD_DIR="$TOPDIR/build/circle-newlib"
CIRCLEHOME="$TOPDIR/libs/circle"

while true ; do
    case "$1" in
	-c|--clean) CLEAN=1 ; shift;;
	-d|--debug) DEBUG=1 ; shift;;
	-h|--help) usage ; exit 0;;
	-n|--no-cpp) STDLIB_SUPPORT=2 ; shift;;
	--no-parallel-build) PARALLEL= ; shift;;
	--opt-tls) OPTTLS=1 ; shift;;
	-p|--prefix) TOOLPREFIX="$2" ; shift 2;;
	-r|--raspberrypi) RASPBERRYPI="$2" ; shift 2;;
	--script-debug) set -x ; shift;;
	--softfp) FLOAT_ABI=softfp ; shift;;
	-s|--stddefpath) STDDEF_INCPATH="$2" ; shift 2;;
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1;;
    esac
done

echo "RASPBERRYPI=$RASPBERRYPI"
echo "TOOLPREFIX=$TOOLPREFIX"
echo "CIRCLE_BUILD=$CIRCLE_BUILD"
echo "FLOAT_ABI=$FLOAT_ABI"
echo "NEWLIB_BUILD=$NEWLIB_BUILD"
echo "RASPBERRYPI=$RASPBERRYPI"
echo "STDLIB_SUPPORT=$STDLIB_SUPPORT"
echo "NEWLIB_INSTALL_DIR=$NEWLIB_INSTALL_DIR"
echo "NEWLIB_BUILD_DIR=$NEWLIB_BUILD_DIR"
echo "CLEAN=$CLEAN"
echo "DEBUG=$DEBUG"

echo "Remaining arguments:"
for arg do echo '--> '"\`$arg'" ; done

if [ $CLEAN -eq 1 ]
then
    echo "Cleaning ..."
    (
	cd libs/circle && bash -x ./makeall --nosample PREFIX=$TOOLPREFIX clean
    )
    (
	cd libs/circle/addon/SDCard && make PREFIX=$TOOLPREFIX clean
    )
    (
	cd "$NEWLIB_BUILD_DIR" && make -i clean
    )
    rm -rf "$NEWLIB_BUILD_DIR"/*
    rm -rf "$NEWLIB_INSTALL_DIR"/*
    (
	cd libs/mbedtls/library && make clean
    )
    (
	cd src/circle-mbedtls && make clean
    )
    exit 0
fi

: ${CC:="$TOOLPREFIX"gcc}

if [ ! -x "`which $CC 2> /dev/null`" ]
then
    echo "Error: Invalid toolchain prefix, exiting" >&2
    echo "TOOLPREFIX is \"$TOOLPREFIX\"" >&2
    exit 1
fi

if [ "$STDDEF_INCPATH" = "" ]
then
    # TODO this is probably very GCC-specific
    STDDEF_INCPATH=$(echo | "$CC" -v -x c -E - 2>&1 | grep "^ /.*include$" | head -1 | sed -e "s/ //")
fi

if [ ! -f "$STDDEF_INCPATH/stddef.h" ]
then
    echo "Error: Unable to determine include path for stddef.h, exiting" >&2
    echo "STDDEF_INCPATH is \"$STDDEF_INCPATH\"" >&2
    exit 1
fi

echo "STDDEF_INCPATH=$STDDEF_INCPATH"

case "$RASPBERRYPI" in
    1|2|3) ;;
    *) echo "Unknown Raspberry Pi target" >&2; exit 1;;
esac

# Sanity checks that sub-modules are checked out
if [ ! -f "libs/circle/Rules.mk" ]
then
    echo "Circle sub-module not found" >&2
    exit 1
fi

if [ ! -f "libs/circle-newlib/README.md" ]
then
    echo "newlib sub-module not found" >&2
    exit 1
fi

if [ $OPTTLS -eq 1 ]
then
    if [ ! -f "libs/mbedtls/README.md" ]
    then
        echo "mbedtls sub-module not found" >&2
        exit 1
    fi
fi

# Create Circle's Config.mk file
(
    echo "RASPPI = $RASPBERRYPI"
    echo "PREFIX = $TOOLPREFIX"
    echo "FLOAT_ABI = $FLOAT_ABI"
    echo "STDLIB_SUPPORT = $STDLIB_SUPPORT"
    echo "STDDEF_INCPATH = \"$STDDEF_INCPATH\""
    if [ $DEBUG -eq 1 ]
    then
	echo "OPTIMIZE = -O0 -g"
    fi
) > libs/circle/Config.mk

# Retrieve the resulting compiler flags from Cicle's top-level Makefile rules
ARCH=$(make -n -p -f libs/circle/Rules.mk CIRCLEHOME=libs/circle | grep ^ARCH | \
    sed -e "s/\$(FLOAT_ABI)/$FLOAT_ABI/" -e 's/ARCH = //')

GCC_PREFIX=$TOOLPREFIX
export \
    CPPFLAGS_FOR_TARGET="-I\"$CIRCLEHOME/include\" -I\"$TOPDIR/include\"" \
    CC_FOR_TARGET=${GCC_PREFIX}gcc \
    CXX_FOR_TARGET=${GCC_PREFIX}g++ \
    GCC_FOR_TARGET=${GCC_PREFIX}gcc \
    AR_FOR_TARGET=${GCC_PREFIX}gcc-ar \
    AS_FOR_TARGET=${GCC_PREFIX}gcc-as \
    LD_FOR_TARGET=${GCC_PREFIX}gcc-ld \
    RANLIB_FOR_TARGET=${GCC_PREFIX}gcc-ranlib \
    OBJCOPY_FOR_TARGET=${GCC_PREFIX}gcc-objcopy \
    OBJDUMP_FOR_TARGET=${GCC_PREFIX}gcc-objdump

if [ $DEBUG -eq 1 ]
then
    CFLAGS_FOR_TARGET="$ARCH -O0 -g"
else
    CFLAGS_FOR_TARGET="$ARCH"
fi
export CFLAGS_FOR_TARGET

(
    cd $NEWLIB_BUILD_DIR
    "$TOPDIR/libs/circle-newlib/configure" --target arm-none-circle --disable-multilib \
        --prefix "$NEWLIB_INSTALL_DIR"
)

(
    cd libs/circle && ./makeall --nosample $PARALLEL
)

(
    # The sample 02-stdio-fatfs needs the SDCard library
    cd libs/circle/addon/SDCard && make
)

(
    cd "$NEWLIB_BUILD_DIR" && make $PARALLEL && make install
)

if [ $OPTTLS -eq 1 ]
then
   (
      export CC=${TOOLPREFIX}gcc
      INCLUDE="-I../../../include"
      DEFINE="-DMBEDTLS_CONFIG_FILE='<circle-mbedtls/config-circle-mbedtls.h>'"
      export CFLAGS="$ARCH -fsigned-char -ffreestanding -O2 -g $INCLUDE $DEFINE"
      cd libs/mbedtls/library && make $PARALLEL
   )
   (
      cd src/circle-mbedtls && make $PARALLEL
   )
fi

exit 0
