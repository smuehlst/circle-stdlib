#
# Makefile
#

CIRCLE_STDLIB_DIR = ../../..

OBJS	= main.o kernel.o htmlscanner.o

EXTRACLEAN = *.crt

include ../Rules.mk

certs:
	wget -q http://cacerts.digicert.com/CloudflareIncECCCA-3.crt
