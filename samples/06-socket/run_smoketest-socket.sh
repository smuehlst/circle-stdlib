./reset-sd.sh
# make -C ../.. && make

./socket_test.sh &

# ~/local/qemu-6.0.0/bin/qemu-system-aarch64 -M raspi3 -kernel kernel8.img  -display none -nographic -semihosting -sd sdcard.img -netdev "user,id=net0,hostfwd=tcp::5000-:5000,guestfwd=tcp:127.0.0.1:5001-cmd:./socat_server.sh" -device usb-net,netdev=net0
~/local/qemu-9.1.1/bin/qemu-system-aarch64 -M raspi3b -kernel kernel8.img \
    -display none -nographic -semihosting -sd sdcard.img \
    -netdev "user,id=net0,hostfwd=tcp::5000-:5000,guestfwd=tcp:10.0.2.100:5001-cmd:yes simulated server" \
    -device usb-net,netdev=net0 \
    -vnc :0 # -s -S
