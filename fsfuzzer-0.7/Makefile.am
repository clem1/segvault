# Makefile.am-- 
# Copyright 2006 Steve Grubb <linux_4ever@yahoo.com>. 
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#

CONFIG_CLEAN_FILES = *.rej *.orig 
bin_SCRIPTS = fsfuzz run_test run_last
EXTRA_DIST = AUTHORS ChangeLog COPYING INSTALL NEWS README TODO $(bin_SCRIPTS)

AM_CFLAGS = -g -D_GNU_SOURCE -W -Wall -Wundef
bin_PROGRAMS = mangle fstest

mangle_SOURCES = mangle.c
fstest_SOURCES = fstest.c

clean-generic:
	rm -rf cfs
	rm -rf fs
	rm -rf autom4te.cache 
