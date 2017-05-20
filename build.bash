#!/bin/bash

set -e

function usage() {
    echo "usage: build.bash [ <option> ... ]"
    echo "Build Circle with newlib standard C library."
    echo
    echo "Options:"
    echo "  -c, --clean                    clean build results and exit"
    echo "  -d, --debug                    build with debug information, without optimizer"
    echo "  -h, --help                     show usage message"
    echo "  -r <number>, --raspberrypi <number>"
    echo "                                 Circle Raspberry Pi model number (1, 2, 3, default: 1)"
    echo "  -s <path>, --stddefpath <path>"
    echo "                                 path where stddef.h header is located (only necessary"
    echo "                                 if  script cannot determine it automatically)"
}

# From the bash FAQ: How to use pathnames relative to the script
cd "${BASH_SOURCE%/*}" || exit 1
TOPDIR="$PWD"

TEMP=$(getopt -o cdhr:s: --long clean,debug,help,no-circle-build,no-newlib-build,raspberrypi:,script-debug,stddefpath: \
     -n 'build.bash' -- "$@")

if [ $? != 0 ] ; then echo usage; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

: ${CC:=arm-none-eabi-gcc}

CLEAN=0
DEBUG=0
RASPBERRYPI=1
CIRCLE_BUILD=1
NEWLIB_BUILD=1
CLEAN=0
STDDEF_INCPATH=""

NEWLIB_INSTALL_DIR="$TOPDIR/install"
NEWLIB_BUILD_DIR="$TOPDIR/build/circle-newlib"
CIRCLEHOME="$TOPDIR/libs/circle"

while true ; do
    case "$1" in
	-c|--clean) CLEAN=1 ; shift;;
	-d|--debug) DEBUG=1 ; shift;;
	-h|--help) usage ; exit 0;;
	# --no-circle-build) CIRCLE_BUILD=0 ; shift;;
	# --no-newlib-build) NEWLIB_BUILD=0 ; shift;;
	-r|--raspberrypi) RASPBERRYPI="$2" ; shift 2;;
	--script-debug) set -x ; shift;;
	-s|--stddefpath) STDDEF_INCPATH="$2" ; shift 2;;
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1;;
    esac
done

echo "RASPBERRYPI=$RASPBERRYPI"
echo "CIRCLE_BUILD=$CIRCLE_BUILD"
echo "NEWLIB_BUILD=$NEWLIB_BUILD"
echo "RASPBERRYPI=$RASPBERRYPI"
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
	cd libs/circle && ./makeall clean
    )
    (
	cd "$NEWLIB_BUILD_DIR" && make clean
    )
    rm -rf "$NEWLIB_BUILD_DIR"/*
    rm -rf "$NEWLIB_INSTALL_DIR"/*
    exit 0
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

# Create Circle's Config.mk file
(
    echo "RASPPI = $RASPBERRYPI"
    echo "STDDEF_INCPATH = \"$STDDEF_INCPATH\""
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
    CPPFLAGS_FOR_TARGET="-I\"$CIRCLEHOME/include\"" \
    CC_FOR_TARGET=${GCC_PREFIX}-gcc \
    CXX_FOR_TARGET=${GCC_PREFIX}-g++ \
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
    "$TOPDIR/libs/circle-newlib/configure" --target arm-none-circle --disable-multilib \
        --prefix "$NEWLIB_INSTALL_DIR"
)

(
    cd libs/circle && ./makeall -j
)

(
    cd "$NEWLIB_BUILD_DIR" && make -j && make install
)

exit 0
