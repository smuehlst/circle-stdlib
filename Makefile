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

build-samples:
	$(MAKE) -C samples/01-nosys
	$(MAKE) -C samples/02-stdio-hello
	$(MAKE) -C samples/03-stdio-fatfs
	$(MAKE) -C samples/04-std

clean:
	-cd libs/circle && ./makeall --nosample PREFIX=$(TOOLPREFIX) clean
	-$(MAKE) -C libs/circle/addon/SDCard PREFIX=$(TOOLPREFIX) clean
	-$(MAKE) -C $(NEWLIB_BUILD_DIR) clean
	-test -n "$(NEWLIB_INSTALL_DIR)" && rm -rf "$(NEWLIB_INSTALL_DIR)"/*
	-$(MAKE) -C samples/01-nosys clean
	-$(MAKE) -C samples/02-stdio-hello clean
	-$(MAKE) -C samples/03-stdio-fatfs clean
	-$(MAKE) -C samples/04-std clean
