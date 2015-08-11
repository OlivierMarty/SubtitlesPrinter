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

#ifndef H_PRINTER
#define H_PRINTER

#include "rich_text.h"
#include "events.h"
#include <X11/Xlib.h>

struct message
{
  struct richText *t;
  int id;
  int posy;
  int height;
  int width;
};

struct printerEnv
{
  Display *d;
  int d_fd;
  int s;
  Window w;
  Window w_focused;
  GC gc;
  XFontStruct *fontinfo;    // regular
  XFontStruct *fontinfo_i;  // italic
  XFontStruct *fontinfo_b;  // bold
  XFontStruct *fontinfo_bi; // both
  int maxascent, maxdescent;
  int padding, gap, gap2;
  int root_width, root_height;
  int margin_bottom;
  unsigned long color_background, color_text;

  struct message *texts;
  int size;
  int maxsize;
};

struct printerEnv printerOpenWindow(char *font, char *font_i, char *font_b,
  char *font_bi);
void printerCloseWindow(struct printerEnv env);

// rt is not copied
void printerShow(struct printerEnv *env, struct richText *rt, int id);
void printerHide(struct printerEnv *env, int id);

// automatically called in printerShow
void printerRender(struct printerEnv *env);

// read next event
t_event manageEvent(struct printerEnv *env);

#endif
