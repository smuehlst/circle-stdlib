#
# Makefile
#

CIRCLEHOME = ../../libs/circle
NEWLIBDIR = ../../install/arm-none-circleraspi0

OBJS	= main.o kernel.o

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH)
LIBS := "$(NEWLIBDIR)/lib/libc.a" "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libcirclenewlib.a" $(CIRCLEHOME)/lib/libcircle.a
