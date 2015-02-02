
#
#  Copyright (C) 2014  Olivier Marty <olivier.marty.m at gmail.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

SRCS=time.c parser.c rich_text.c events.c printer.c main.c
CC=gcc
CFLAGS=
LIBS=$(shell pkg-config --cflags --libs x11) -lrt
INSTALL_DIR=/usr/local/bin

OBJS = $(SRCS:.c=.o)

default: subtitlesPrinter

subtitlesPrinter: $(OBJS)
	$(CC) -o subtitlesPrinter $^ $(CFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

install: subtitlesPrinter
	install --mode=755 subtitlesPrinter $(INSTALL_DIR)

.PHONY: clean default install

clean:
	rm -f *.o subtitlesPrinter

