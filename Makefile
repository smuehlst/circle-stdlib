.NOTPARALLEL:

include Config.mk

all: circle newlib

circle:
	cd libs/circle && ./makeall --nosample
	$(MAKE) -C libs/circle/addon/SDCard
	
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

clean:
	cd libs/circle && ./makeall --nosample PREFIX=$(TOOLPREFIX) clean
	$(MAKE) -C libs/circle/addon/SDCard PREFIX=$(TOOLPREFIX) clean
	$(MAKE) -C $(NEWLIB_BUILD_DIR) clean
	test -n "$(NEWLIB_BUILD_DIR)" && echo rm -rf "$(NEWLIB_BUILD_DIR)"/* || :
	test -n "$(NEWLIB_INSTALL_DIR)" && echo rm -rf "$(NEWLIB_INSTALL_DIR)"/* || :
	$(MAKE) -C libs/mbedtls/library clean
	$(MAKE) -C src/circle-mbedtls clean
