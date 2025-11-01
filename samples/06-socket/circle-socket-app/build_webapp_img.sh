#!/bin/bash
# Build the SvelteKit webapp and package it into a FAT32 disk image.

set -e
npm run build

imgfile="sdcard.img"
dd if=/dev/zero of="${imgfile}" bs=1M count=16
mkfs.vfat sdcard.img
loopback_device=$(sudo losetup -f --show "${imgfile}")
tmpdir=$(mktemp -d)
sudo mount "$loopback_device" "$tmpdir"
sudo cp -r build "${tmpdir}/circle-socket-app"
sudo umount "$tmpdir"
sudo losetup -d "$loopback_device"
rmdir "$tmpdir"

echo "Webapp image built: ${imgfile}"
rm -f "${imgfile}.zip"
zip "${imgfile}.zip" "${imgfile}"
echo "Zipped image: ${imgfile}.zip"
