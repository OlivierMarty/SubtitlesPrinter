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

#ifndef H_RICH_TEXT
#define H_RICH_TEXT

enum t_type {T_REGULAR = 0, T_ITALIC = 1, T_BOLD = 2};

struct richText
{
  struct richText *left; // left and right must be NULL or non-NULL at the same time
  struct richText *right;

  // it left and right are NULL :
  enum t_type type;
  char *pos;
  int size;

  // if first node :
  char *raw;
};

// data is not copied, field raw contains a pointer to it
struct richText* richTextParse(char* data);
void richTextFree(struct richText *rt);

#endif
