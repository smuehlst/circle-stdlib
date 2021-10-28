#
# Makefile
#

CIRCLE_STDLIB_DIR = ../../..

OBJS	= main.o kernel.o htmlscanner.o

EXTRACLEAN = *.crt *.der

include ../Rules.mk

certs:
	wget -q -O ISRG-Root-X1.der http://x1.i.lencr.org/
