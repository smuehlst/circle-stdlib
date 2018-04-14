#
# Makefile
#

CIRCLE_STDLIB_DIR = ../..

-include $(CIRCLE_STDLIB_DIR)/Config.mk

MBEDTLS_DIR ?= $(CIRCLE_STDLIB_DIR)/libs/mbedtls
NEWLIBDIR   ?= $(CIRCLE_STDLIB_DIR)/install/arm-none-circle
CIRCLEHOME  ?= $(CIRCLE_STDLIB_DIR)/libs/circle

OBJS	= net_sockets.o adaptedsocket.o entropymodule.o

libcircle-mbedtls.a: $(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)

include $(CIRCLEHOME)/Rules.mk

INCLUDE += \
	   -I $(CIRCLE_STDLIB_DIR)/include \
	   -I $(NEWLIBDIR)/include \
	   -I $(MBEDTLS_DIR)/include

CFLAGS += -DMBEDTLS_CONFIG_FILE='<circle-mbedtls/config-circle-mbedtls.h>'
