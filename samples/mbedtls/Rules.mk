#
# Rules.mk
#

CIRCLE_STDLIB_DIR ?= ../../..

-include $(CIRCLE_STDLIB_DIR)/Config.mk

MBEDTLS_DIR ?= $(CIRCLE_STDLIB_DIR)/libs/mbedtls
NEWLIBDIR   ?= $(CIRCLE_STDLIB_DIR)/install/$(NEWLIB_ARCH)
CIRCLEHOME  ?= $(CIRCLE_STDLIB_DIR)/libs/circle

include $(CIRCLEHOME)/Rules.mk

INCLUDE += \
	   -I $(CIRCLE_STDLIB_DIR)/include \
	   -I $(NEWLIBDIR)/include \
	   -I $(MBEDTLS_DIR)/include

CFLAGS += -DMBEDTLS_CONFIG_FILE='<circle-mbedtls/config-circle-mbedtls.h>'

LIBS += \
	$(CIRCLE_STDLIB_DIR)/src/circle-mbedtls/libcircle-mbedtls.a \
	$(MBEDTLS_DIR)/library/libmbedtls.a \
  	$(MBEDTLS_DIR)/library/libmbedx509.a \
  	$(MBEDTLS_DIR)/library/libmbedcrypto.a \
  	$(NEWLIBDIR)/lib/libm.a \
	$(NEWLIBDIR)/lib/libc.a \
	$(NEWLIBDIR)/lib/libcirclenewlib.a \
	$(CIRCLEHOME)/addon/SDCard/libsdcard.a \
  	$(CIRCLEHOME)/lib/usb/libusb.a \
 	$(CIRCLEHOME)/lib/input/libinput.a \
 	$(CIRCLEHOME)/addon/fatfs/libfatfs.a \
 	$(CIRCLEHOME)/lib/fs/libfs.a \
  	$(CIRCLEHOME)/lib/net/libnet.a \
  	$(CIRCLEHOME)/lib/sched/libsched.a \
	$(CIRCLEHOME)/lib/libcircle.a

-include $(DEPS)
