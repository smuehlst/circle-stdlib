#!/bin/bash
set -e
# set -x

../../testdata/setup-sd.sh
make

./socket_test.sh &
server_pid=$!

$CFG_QEMU_BIN -M $CFG_QEMU_MACHINE $CFG_QEMU_OPT $CFG_QEMU_IMG \
    -display none -nographic -semihosting -sd sdcard.img \
    -netdev "user,id=net0,hostfwd=tcp::5000-:5000,guestfwd=tcp:10.0.2.100:5001-cmd:yes simulated server" \
    -device usb-net,netdev=net0 -s -S
exit_code=$?

kill $server_pid 2>/dev/null || true

exit $exit_code