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

#ifndef H_PRINTER
#define H_PRINTER

#include <X11/Xlib.h>

struct printerEnv
{
  Display *d;
  int s;
  Window w;
  GC gc;
  XFontStruct *fontinfo;    // regular
  XFontStruct *fontinfo_i;  // italic
  XFontStruct *fontinfo_b;  // bold
  XFontStruct *fontinfo_bi; // both
  int maxascent, maxdescent;
  int width, height;
  unsigned long color_background, color_text;
};

enum t_type {T_ITALIC = 1, T_BOLD = 2};

// if width < 0 the window will be as larger as possible
struct printerEnv printerOpenWindow(int width, int height, int padding_bottom);
void printerCloseWindow(struct printerEnv env);

void printerShow(struct printerEnv env, char* text, enum t_type font);
void printerClean(struct printerEnv env);

#endif

