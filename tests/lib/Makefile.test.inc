#
# Makefile
#

include $(TOPDIR)/Config.mk

CIRCLEHOME = $(TOPDIR)/libs/circle
NEWLIBDIR = $(TOPDIR)/install/$(NEWLIB_ARCH)

OBJS	= main.o

include $(CIRCLEHOME)/Rules.mk

INCLUDE += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -I $(TOPDIR)/include -I $(TOPDIR)/tests/lib -I $(TOPDIR)/libs/doctest
DEFINE	+= -D_POSIX_C_SOURCE=200112L
LIBS := "$(TOPDIR)/tests/lib/libdoctest.a" \
    "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libc.a" "$(NEWLIBDIR)/lib/libcirclenewlib.a" \
 	$(CIRCLEHOME)/addon/SDCard/libsdcard.a \
  	$(CIRCLEHOME)/lib/usb/libusb.a \
 	$(CIRCLEHOME)/lib/input/libinput.a \
 	$(CIRCLEHOME)/addon/fatfs/libfatfs.a \
 	$(CIRCLEHOME)/lib/fs/libfs.a \
  	$(CIRCLEHOME)/lib/net/libnet.a \
  	$(CIRCLEHOME)/lib/sched/libsched.a \
	$(CIRCLEHOME)/addon/qemu/libqemusupport.a \
  	$(CIRCLEHOME)/lib/libcircle.a
