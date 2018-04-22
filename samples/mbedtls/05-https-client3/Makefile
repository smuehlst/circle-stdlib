#
# Makefile
#

CIRCLE_STDLIB_DIR = ../../..

OBJS	= main.o kernel.o

EXTRACLEAN = *.pem

include ../Rules.mk

certs:
	wget -q https://letsencrypt.org/certs/letsencryptauthorityx3.pem
