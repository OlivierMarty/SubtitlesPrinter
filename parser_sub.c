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

int start_sub(FILE *f)
{
  return 1;
}

int next_sub(FILE *f, int expected, struct SubtitleLine *r)
{
  int t_h, t_m, t_s, t_ms;
  fscanf(f, " %d : %d : %d . %d , ", &t_h, &t_m, &t_s, &t_ms);
  r->begin = timeCreate(t_h*3600 + t_m*60 + t_s, t_ms*10000000);
  fscanf(f, " %d : %d : %d . %d ", &t_h, &t_m, &t_s, &t_ms);
  r->end = timeCreate(t_h*3600 + t_m*60 + t_s, t_ms*10000000);

  *(r->text) = '\0';
  char line[1024];
  while(1)
  {
    if(fgets(line, 1024, f) == NULL)
      break;
    strcat(r->text, line);
    if(line[strlen(line)-1] == '\n') // read only one line
      break;
    if(feof(f))
    {
      strcat(r->text, "\n");
      break;
    }
  }

  // replace "[br]" by "\n"
  char* pos = r->text;
  char* end = pos + strlen(r->text);
  while(pos < end && (pos = strstr(pos, "[br]")) != NULL)
  {
    *pos++ = '\n';
    *pos++ = 127;
    *pos++ = 127;
    *pos++ = 127;
  }

  r->id = expected;
  return r->id;
}
