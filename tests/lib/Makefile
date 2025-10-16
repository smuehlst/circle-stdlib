#
# Makefile
#
# Circle - A C++ bare metal environment for Raspberry Pi
# Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

TOPDIR = ../..

include $(TOPDIR)/Config.mk

CIRCLEHOME = $(TOPDIR)/libs/circle
NEWLIBDIR = $(TOPDIR)/install/$(NEWLIB_ARCH)

OBJS	= doctest.o doctest_logger.o

libdoctest.a: $(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)

include $(CIRCLEHOME)/Rules.mk

# Important: Extend INCLUDE after including Rules.mk so that the Circle include
# paths come before the newlib include path. Otherwise there are problems
# with Circle's assert macros.
INCLUDE += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -I$(TOPDIR)/libs/doctest
DEFINE	+= -D_POSIX_C_SOURCE=200112L

-include $(DEPS)
