#
# Makefile
#

CIRCLEHOME = ../../libs/circle
NEWLIBDIR = ../../install/arm-none-circle

OBJS	= main.o kernel.o

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -I /home/stm/local/gcc-arm-none-eabi-6-2017-q1-update/arm-none-eabi/include/c++/6.3.1/ -I /home/stm/local/gcc-arm-none-eabi-6-2017-q1-update/arm-none-eabi/include/c++/6.3.1/arm-none-eabi -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -I ../../include
LIBS := "$(NEWLIBDIR)/lib/libc.a" "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libcirclenewlib.a" \
 	$(CIRCLEHOME)/addon/SDCard/libsdcard.a \
 	$(CIRCLEHOME)/lib/input/libinput.a \
 	$(CIRCLEHOME)/lib/fs/fat/libfatfs.a \
 	$(CIRCLEHOME)/lib/fs/libfs.a \
  	$(CIRCLEHOME)/lib/net/libnet.a \
  	$(CIRCLEHOME)/lib/sched/libsched.a \
  	$(CIRCLEHOME)/lib/libcircle.a
