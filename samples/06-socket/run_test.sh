../../testdata/setup-sd.sh
make

# /home/stm/local/qemu-9.1.1/bin/qemu-system-aarch64 -M raspi3b -kernel kernel8.img

$CFG_QEMU_BIN -M $CFG_QEMU_MACHINE $CFG_QEMU_OPT $CFG_QEMU_IMG \
    -vnc :0 -semihosting -sd sdcard.img \
    -netdev user,id=net0,hostfwd=tcp::8080-:80 \
    -device usb-net,netdev=net0
