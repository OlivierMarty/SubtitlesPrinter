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

#include "printer.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>

struct printerEnv printerOpenWindow(int width, int height, int padding_bottom)
{
  struct printerEnv env;

  // open connection
  env.d = XOpenDisplay(NULL);
  if (env.d == NULL) {
      fprintf(stderr, "Unable to open display\n");
      exit(1);
  }
  env.s = DefaultScreen(env.d);

  XVisualInfo vinfo;
  XMatchVisualInfo(env.d, env.s, 32, TrueColor, &vinfo);

  
  // size of the screen
  XWindowAttributes RootAttr;
  XGetWindowAttributes(env.d, RootWindow(env.d, env.s), &RootAttr);
  
  // set window attributes
  XSetWindowAttributes attr;
  attr.colormap = XCreateColormap(env.d, RootWindow(env.d, env.s),
    vinfo.visual, AllocNone);
  attr.border_pixel = 0;
  attr.background_pixel = 0;
  attr.override_redirect = 1; // no window manager border
  
  // set colors
  env.color_text = XWhitePixel(env.d, env.s);
  XColor tmp;
  XParseColor(env.d, attr.colormap, "#222222", &tmp);
  XAllocColor(env.d, attr.colormap, &tmp);
  env.color_background = tmp.pixel;
  //env.color_text = -1;
  //env.color_background = (100 * 256 + 100) * 256 + 100;
  
  // create the window
  env.width = (width < 0) ? RootAttr.width : width;
  env.height = height;
  env.w = XCreateWindow(env.d, RootWindow(env.d, env.s),
    (RootAttr.width - env.width)/2,
    RootAttr.height - env.height - padding_bottom, env.width, env.height, 0,
    vinfo.depth, InputOutput, vinfo.visual,
    CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attr);
  
  // set the font in a new GC
  XGCValues gr_values;
  env.fontinfo = XLoadQueryFont(env.d, "*x24");
  gr_values.font = env.fontinfo->fid;
  gr_values.foreground = XWhitePixel(env.d, env.s);
  gr_values.background = XBlackPixel(env.d, env.s);
  env.gc = XCreateGC(env.d, env.w, GCFont | GCForeground, &gr_values);
  
  // event to be listened
  //XSelectInput(env.d, env.w, ExposureMask | KeyPressMask);
  
  // display the window
  XMapWindow(env.d, env.w);
  XFlush(env.d);
  
  return env;
}

void printerCloseWindow(struct printerEnv env)
{
  XCloseDisplay(env.d); // window is destroyed
}

int printLines(struct printerEnv env, char *text, int gap,
  int width, int height, int padding) // for the frame
{
  if(*text == '\0')
  {
    XSetForeground(env.d, env.gc, env.color_background);
    XFillRectangle(env.d, env.w, env.gc, (env.width - width)/2 - padding,
      env.height - height - padding, width + 2*padding, height + 2*padding);
    XSetForeground(env.d, env.gc, env.color_text);
    return env.height - padding;
  }
  
  char *next;
  int size;
  
  next = strchr(text, '\n');
  if(next == NULL)
  {
    size = strlen(text);
    next = text + size;
  }
  else
  {
    size = next-text;
    if(*(next-1) == '\r')
      size--; // hide this character
    next++; // forget \n
  }
  
  // compute the size of the text
  int font_direction, font_ascent, font_descent, text_y;
  XCharStruct text_structure;
  XTextExtents(env.fontinfo, text, size,
               &font_direction, &font_ascent, &font_descent,
               &text_structure);
  
  // print next line, and this one above
  if(text_structure.width > width)
    width = text_structure.width;
  
  text_y = printLines(env, next, gap, width,
    height + text_structure.ascent + text_structure.descent + gap, padding);
  text_y -= text_structure.descent;
  
  XDrawString(env.d, env.w, env.gc, (env.width - text_structure.width)/2,
    text_y, text, size);
  
  return text_y - text_structure.ascent - gap;
}

void printerShow(struct printerEnv env, char* text)
{
  XClearWindow(env.d, env.w);
  printLines(env, text, 5, 0, 0, 5);
  XFlush(env.d);
}

void printerClean(struct printerEnv env)
{
  XClearWindow(env.d, env.w);
  XFlush(env.d);
}

