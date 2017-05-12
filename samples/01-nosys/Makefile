#
# Makefile
#

CIRCLEHOME = ../../libs/circle
NEWLIBDIR = ../../install/arm-none-circleraspi0
GCCDIR = /home/stm/local/gcc-arm-none-eabi-6-2017-q1-update/lib/gcc/arm-none-eabi/6.3.1

OBJS	= main.o kernel.o

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -I "$(NEWLIBDIR)/include" -I "$(GCCDIR)/include"
LIBS := "$(NEWLIBDIR)/lib/libc.a" "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libcirclenewlib.a" $(CIRCLEHOME)/lib/libcircle.a
