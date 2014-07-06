/*
  Copyright (C) 2014  Olivier Marty <olivier.marty.m@gmail.com>

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

int next(FILE *f, int expected, struct SubtitleLine *r)
{
  int t_h, t_m, t_s, t_ms;
  fscanf(f, "%d\n", &r->id);
  if(r->id != expected)
    fprintf(stderr, "expected : %d; found : %d\n", expected, r->id);
  
  fscanf(f, "%d:%d:%d,%d --> ", &t_h, &t_m, &t_s, &t_ms);
  r->begin = timeCreate(t_h*3600 + t_m*60 + t_s, t_ms*1000000);
  // TODO and if there are 4 digits ?
  fscanf(f, "%d:%d:%d,%d\n", &t_h, &t_m, &t_s, &t_ms);
  r->end = timeCreate(t_h*3600 + t_m*60 + t_s, t_ms*1000000);

  *(r->text) = '\0';
  char line[1024];
  while(1)
  {
    fgets(line, 1024, f);
    if(feof(f) || line[0] == '\n' || line[0] == '\0' || (line[0] == '\r' && line[1] == '\n'))
      break;
    strcat(r->text, line);
  }
  
  return r->id;
}

