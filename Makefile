.NOTPARALLEL:

include Config.mk

all: circle newlib $(MBEDTLS)

build-samples: build-stdlib-samples $(MBEDTLS_SAMPLES)

circle:
	cd libs/circle && ./makeall --nosample
	$(MAKE) -C libs/circle/addon/SDCard
	$(MAKE) -C libs/circle/addon/fatfs
	$(MAKE) -C libs/circle/addon/qemu
	
newlib:
	CPPFLAGS_FOR_TARGET='$(CPPFLAGS_FOR_TARGET)' \
	CC_FOR_TARGET='$(CC_FOR_TARGET)' \
	CXX_FOR_TARGET='$(CXX_FOR_TARGET)' \
	GCC_FOR_TARGET='$(GCC_FOR_TARGET)' \
	AR_FOR_TARGET='$(AR_FOR_TARGET)' \
	AS_FOR_TARGET='$(AS_FOR_TARGET)' \
	LD_FOR_TARGET='$(LD_FOR_TARGET)' \
	RANLIB_FOR_TARGET='$(RANLIB_FOR_TARGET)' \
	OBJCOPY_FOR_TARGET='$(OBJCOPY_FOR_TARGET)' \
	OBJDUMP_FOR_TARGET='$(OBJDUMP_FOR_TARGET)' \
	$(MAKE) -C $(NEWLIB_BUILD_DIR) && \
	$(MAKE) -C $(NEWLIB_BUILD_DIR) install

build-stdlib-samples:
	$(MAKE) -C samples/01-nosys
	$(MAKE) -C samples/02-stdio-hello
	$(MAKE) -C samples/03-stdio-fatfs
	$(MAKE) -C samples/04-std

MBEDTLS_INCLUDE = -I../../../include -I../../circle/include
MBED_DEFINE = -DMBEDTLS_CONFIG_FILE='<circle-mbedtls/config-circle-mbedtls.h>'

mbedtls:
	CC=$(CC) \
	CFLAGS="$(ARCH) -fsigned-char -ffreestanding -O2 -Wno-parentheses -g $(MBEDTLS_INCLUDE) $(MBED_DEFINE)" \
	$(MAKE) -C libs/mbedtls/library && \
	$(MAKE) -C src/circle-mbedtls

build-mbedtls-samples:
	$(MAKE) -C samples/mbedtls/01-https-client1
	$(MAKE) -C samples/mbedtls/02-https-client2
	$(MAKE) -C samples/mbedtls/03-https-server1
	$(MAKE) -C samples/mbedtls/04-https-server2
	$(MAKE) -C samples/mbedtls/05-https-client3
	$(MAKE) -C samples/mbedtls/06-webclient
	$(MAKE) -C samples/mbedtls/07-mqttclient
	
clean:
	-cd libs/circle && ./makeall --nosample PREFIX=$(TOOLPREFIX) clean
	-$(MAKE) -C libs/circle/addon/SDCard PREFIX=$(TOOLPREFIX) clean
	-$(MAKE) -C libs/circle/addon/qemu PREFIX=$(TOOLPREFIX) clean
	-$(MAKE) -C $(NEWLIB_BUILD_DIR) clean
	-test -n "$(NEWLIB_INSTALL_DIR)" && rm -rf "$(NEWLIB_INSTALL_DIR)"/*
	-$(MAKE) -C samples/01-nosys clean
	-$(MAKE) -C samples/02-stdio-hello clean
	-$(MAKE) -C samples/03-stdio-fatfs clean
	-$(MAKE) -C samples/04-std clean
	-$(MAKE) -C samples/05-smoketest clean
	-$(MAKE) -C libs/mbedtls/library clean
	-$(MAKE) -C src/circle-mbedtls clean

mrproper: clean
	-rm -f Config.mk
	-rm -rf build/circle-newlib/*
