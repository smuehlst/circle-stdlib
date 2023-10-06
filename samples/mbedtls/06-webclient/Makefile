#
# Makefile
#

CIRCLE_STDLIB_DIR = ../../..

OBJS	= main.o kernel.o htmlscanner.o

EXTRACLEAN = *.crt *.der *.pem

include ../Rules.mk

certs:
	wget -q -O BaltimoreCyberTrustRoot.crt.pem https://cacerts.digicert.com/BaltimoreCyberTrustRoot.crt.pem
