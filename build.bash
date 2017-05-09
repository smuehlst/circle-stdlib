#!/bin/bash

function usage() {
    echo "usage: build.bash [ <option> ... ]" >&2
}

# From the bash FAQ: How to use pathnames relative to the script
cd "${BASH_SOURCE%/*}" || exit 1
TOPDIR="$PWD"

TEMP=$(getopt -o cdr: --long clean,debug,no-circle-build,ino-newlib-build,raspberrypi:,script-debug \
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

NEWLIB_INSTALL_DIR="$TOPDIR/install"
NEWLIB_BUILD_DIR="$TOPDIR/build/circle-newlib"

while true ; do
    case "$1" in
	-c|--clean) CLEAN=1 ; shift;;
	-d|--debug) DEBUG=1 ; shift;;
	--no-circle-build) CIRCLE_BUILD=0 ; shift;;
	--no-circle-build) CIRCLE_BUILD=0 ; shift;;
	--no-newlib-build) NEWLIB_BUILD=0 ; shift;;
	-r|--raspberrypi) RASPBERRYPI="$2" ; shift 2;;
	--script-debug) set -x ; shift;;
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1;;
    esac
done

echo "RASPBERRYPI=$RASPBERRYPI"
echo "CIRCLE_BUILD=$CIRCLE_BUILD"
echo "NEWLIB_BUILD=$NEWLIB_BUILD"
echo "RASPBERRYPI=$RASPBERRYPI"

echo "Remaining arguments:"
for arg do echo '--> '"\`$arg'" ; done

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

# Create Circle's Config.mk file
(
    echo "RASPPI=$RASPBERRYPI"
    if [ $DEBUG -eq 1 ]
    then
	echo "OPTIMIZE = -O0 -g"
    fi
) > libs/circle/Config.mk

# Retrieve the resulting compiler flags from Cicle's top-level Makefile rules
ARCH=$(make -n -p -f libs/circle/Rules.mk CIRCLEHOME=libs/circle | grep ^ARCH)
ARCH=${ARCH#ARCH = }

GCC_PREFIX=arm-none-eabi
export \
    CC_FOR_TARGET=${GCC_PREFIX}-gcc \
    GCC_FOR_TARGET=${GCC_PREFIX}-gcc \
    AR_FOR_TARGET=${GCC_PREFIX}-gcc-ar \
    AS_FOR_TARGET=${GCC_PREFIX}-gcc-as \
    LD_FOR_TARGET=${GCC_PREFIX}-gcc-ld \
    RANLIB_FOR_TARGET=${GCC_PREFIX}-gcc-ranlib \
    OBJCOPY_FOR_TARGET=${GCC_PREFIX}-gcc-objcopy \
    OBJDUMP_FOR_TARGET=${GCC_PREFIX}-gcc-objdump

if [ $DEBUG -eq 1 ]
then
    CFLAGS_FOR_TARGET="$ARCH -O0 -g"
else
    CFLAGS_FOR_TARGET="$ARCH"
fi
export CFLAGS_FOR_TARGET

(
    cd $NEWLIB_BUILD_DIR
    "$TOPDIR/libs/circle-newlib/configure" --target arm-none-circleraspi0 --disable-multilib \
        --prefix "$NEWLIB_INSTALL_DIR"
)

(
    cd libs/circle
    ./makeall -j
)

(
    cd "$NEWLIB_BUILD_DIR"
    make -j
)
