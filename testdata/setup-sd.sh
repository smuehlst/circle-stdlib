#!/bin/bash
SCRIPT_PATH="$(realpath "$0")"       # if realpath exists
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

rm -f sdcard.img
unzip $SCRIPT_DIR/sdcard.img.zip
