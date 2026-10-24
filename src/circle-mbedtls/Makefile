#
# Makefile
#

CIRCLE_STDLIB_DIR = ../..

-include $(CIRCLE_STDLIB_DIR)/Config.mk

MBEDTLS_DIR ?= $(CIRCLE_STDLIB_DIR)/libs/mbedtls
NEWLIBDIR   ?= $(CIRCLE_STDLIB_DIR)/install/$(NEWLIB_ARCH)
CIRCLEHOME  ?= $(CIRCLE_STDLIB_DIR)/libs/circle

OBJS	= httpclient.o mqttclient.o mqttreceivepacket.o mqttsendpacket.o \
	  tlssocket.o tlssimpleclientsocket.o adaptedsocket.o net_sockets.o \
	  x509certchain.o entropymodule.o threadingmodule.o cryptomodule.o

libcircle-mbedtls.a: $(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)

include $(CIRCLEHOME)/Rules.mk

INCLUDE += \
	   -I $(CIRCLE_STDLIB_DIR)/include \
	   -I $(NEWLIBDIR)/include \
	   $(MBEDTLS_INCLUDES)

CFLAGS += $(MBEDTLS_DEFINES)
