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
	   $(MBEDTLS_INCLUDES)

CFLAGS += $(MBEDTLS_DEFINES)

# CIRCLE_STDLIB_LIBS contains the libc++ and compiler-rt libraries when
# configured with --libcxx, which clang links with -nostdlib.
LIBS += \
	$(CIRCLE_STDLIB_DIR)/src/circle-mbedtls/libcircle-mbedtls.a \
	$(MBEDTLS_LIBS) \
	$(CIRCLEHOME)/addon/wlan/hostap/wpa_supplicant/libwpa_supplicant.a \
	$(CIRCLEHOME)/addon/wlan/libwlan.a \
	$(CIRCLE_STDLIB_LIBS)

-include $(DEPS)
