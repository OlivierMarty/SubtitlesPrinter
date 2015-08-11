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

#include <string.h>

#include "parser.h"

int start_srt(FILE *f)
{
  return 1;
}

int next_srt(FILE *f, int expected, struct SubtitleLine *r)
{
  int t_h, t_m, t_s, t_ms;
  fscanf(f, "%*d "); // we ignore it

  fscanf(f, "%d:%d:%d,%d --> ", &t_h, &t_m, &t_s, &t_ms);
  r->begin = timeCreate(t_h*3600 + t_m*60 + t_s, t_ms*1000000);
  // TODO and if there are 4 digits ?
  fscanf(f, "%d:%d:%d,%d ", &t_h, &t_m, &t_s, &t_ms);
  r->end = timeCreate(t_h*3600 + t_m*60 + t_s, t_ms*1000000);

  *(r->text) = '\0';
  char line[1024];
  while(1)
  {
    if(fgets(line, 1024, f) == NULL || empty_line(line))
      break;
    strcat(r->text, line);
    if(feof(f))
    {
      strcat(r->text, "\n");
      break;
    }
  }

  r->id = expected;
  return r->id;
}
