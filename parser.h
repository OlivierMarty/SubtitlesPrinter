/*
  Copyright (C) 2014  Olivier Marty <olivier.marty.m at gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef H_PARSER
#define H_PARSER

#include <stdio.h>
#include "time.h"

struct SubtitleLine
{
  int id;
  char text[4096];
  mytime begin;
  mytime end;
};

// read the header, if any
int start_srt(FILE *f);
int start_sub(FILE *f);
int start_sub2(FILE *f);
int start_vtt(FILE *f);

// read the next subtitle, expected to be expected-th
// return the number of the readed subtitle
int next_srt(FILE *f, int expected, struct SubtitleLine *r);
int next_sub(FILE *f, int expected, struct SubtitleLine *r);
int next_sub2(FILE *f, int expected, struct SubtitleLine *r);
int next_vtt(FILE *f, int expected, struct SubtitleLine *r);

// test if a string is an empty line
int empty_line(char *f);

#endif
