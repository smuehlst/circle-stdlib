#
# Makefile
#

CIRCLE_STDLIB_DIR = ../../..

OBJS	= main.o kernel.o htmlscanner.o

EXTRACLEAN = *.crt *.der *.pem

include ../Rules.mk

certs:
	wget -q https://letsencrypt.org/certs/letsencryptauthorityx3.pem
