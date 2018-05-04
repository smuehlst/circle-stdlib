#
# Makefile
#

CIRCLE_STDLIB_DIR = ../../..

OBJS	= main.o kernel.o mqttpublisher.o mqttsubscriber.o

EXTRACLEAN = *.der

include ../Rules.mk

certs:
	wget -q http://test.mosquitto.org/ssl/mosquitto.org.der
