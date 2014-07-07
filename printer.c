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

#include "printer.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <string.h>

char *strnstr(const char *s, const char *find, size_t slen)
{
	char c, sc;
	size_t len;

	if ((c = *find++) != '\0') {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == '\0' || slen-- < 1)
					return (NULL);
			} while (sc != c);
			if (len > slen)
				return (NULL);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}

void loadFont(struct printerEnv *env, XFontStruct **font, char* fontname)
{
  *font = XLoadQueryFont(env->d, fontname);
  if(!*font)
  {
    fprintf (stderr, "unable to load font %s\n", fontname);
    *font = env->fontinfo;
  }
  else
  {
    if((*font)->ascent > env->maxascent)
      env->maxascent = (*font)->ascent;
    if((*font)->descent > env->maxdescent)
      env->maxdescent = (*font)->descent;
  }
}

struct printerEnv printerOpenWindow(int width, int height, int margin_bottom,
  char *font, char *font_i, char *font_b, char *font_bi)
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
    RootAttr.height - env.height - margin_bottom, env.width, env.height, 0,
    vinfo.depth, InputOutput, vinfo.visual,
    CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attr);
  
  // setting fonts
  char *fontname = (font == NULL) ? "*x24*" : font;
  env.fontinfo    = XLoadQueryFont(env.d, fontname);
  if(!env.fontinfo)
  {
    fprintf (stderr, "unable to load font %s: using fixed\n", fontname);
    env.fontinfo = XLoadQueryFont(env.d, "fixed");
  }
  env.maxascent = env.fontinfo->ascent;
  env.maxdescent = env.fontinfo->descent;
  loadFont(&env, &env.fontinfo_i, (font_i == NULL) ? "*charter-medium-i-normal*"
    : font_i);
  loadFont(&env, &env.fontinfo_b, (font_b == NULL) ? "*charter-bold-r-normal*"
    : font_b);
  loadFont(&env, &env.fontinfo_bi, (font_bi == NULL) ? "*charter-bold-i-normal*"
    : font_bi);
  
  // create GC
  XGCValues gr_values;
  gr_values.font = env.fontinfo->fid;
  gr_values.foreground = XWhitePixel(env.d, env.s);
  gr_values.background = XBlackPixel(env.d, env.s);
  env.gc = XCreateGC(env.d, env.w, GCFont | GCForeground, &gr_values);
  
  // event to be listened
  //XSelectInput(env.d, env.w, ExposureMask | KeyPressMask);
  /*
  XEvent e;
  while(XCheckWindowEvent(env.d, env.w, KeyPressMask, &e))
  {
    if(e.type == KeyPress && XLookupKeysym(&(e.xkey), 0) == XK_space)
      printf("PAUSE\n");
  }
  */
  
  // display the window
  XMapWindow(env.d, env.w);
  XFlush(env.d);
  
  return env;
}

void printerCloseWindow(struct printerEnv env)
{
  XCloseDisplay(env.d); // window is automatically destroyed
}

XFontStruct* getFont(struct printerEnv env, enum t_type flags)
{
  if(flags & T_ITALIC && flags & T_BOLD)
    return env.fontinfo_bi;
  if(flags & T_ITALIC)
    return env.fontinfo_i;
  if(flags & T_BOLD)
    return env.fontinfo_b;
  return env.fontinfo;
}

// return the width of the text drawn
// look for i and b html tags
// bug when there is several i (or several v) nested
// set in rflags the flags at the end of the text
int drawText(struct printerEnv env, char *text, int size,
  int x, int y, enum t_type flags, enum t_type *rflags, int draw)
{
  char *i = strnstr(text, "<i>", size),
       *b = strnstr(text, "<b>", size),
       *begin = NULL, *end = NULL;
  int  wopentag = 0, wclosetag = 0,
       width;
  enum t_type nflags = flags;
  *rflags = flags;
  if(i != NULL && (b == NULL || i < b)) // i is the first
  {
    wopentag = 3;
    begin = i;
    end = strnstr(i, "</i>", size - (i - text));
    nflags |= T_ITALIC;
    if(end != NULL)
      wclosetag = 4;
    else
      *rflags |= T_ITALIC;
  }
  else if(b != NULL) // b is the first
  {
    wopentag = 3;
    begin = b;
    end = strnstr(b, "</b>", size - (b - text));
    nflags |= T_BOLD;
    if(end != NULL)
      wclosetag = 4;
    else
      *rflags |= T_BOLD;
  }
  else // no opening tag
  {
    if(flags & T_ITALIC) // we look for it
      i = strnstr(text, "</i>", size);
    if(flags & T_BOLD)
      b = strnstr(text, "</b>", size);
    if(i != NULL && (b == NULL || i < b))
    {
      wclosetag = 4;
      begin = text;
      end = i;
      nflags |= T_ITALIC;
      *rflags &= ~T_ITALIC;
    }
    else if(b != NULL)
    {
      wclosetag = 4;
      begin = text;
      end = b;
      nflags |= T_BOLD;
      *rflags &= ~T_BOLD;
    }
  }
  
  enum t_type dummy;
  if(begin != NULL && end != NULL)
  {
    // before
    width =  drawText(env, text, begin - text, x, y, flags, &dummy, draw);
    // middle
    width += drawText(env, begin + wopentag, end - begin - wopentag,
      x + width, y, nflags, &dummy, draw);
    // after
    width += drawText(env, end + wclosetag, size - (end + wclosetag - begin),
      x + width, y, flags, rflags, draw);
  }
  else if(begin != NULL)
  {
    // before
    width =  drawText(env, text, begin - text, x, y, flags, &dummy, draw);
    // middle
    width += drawText(env, begin + wopentag, size - (begin + wopentag - text),
      x + width, y, nflags, &dummy, draw);
    *rflags |= dummy;
  }
  else
  {
    int font_direction, font_ascent, font_descent;
    XCharStruct text_structure;
    XTextExtents(getFont(env, flags), text, size,
                 &font_direction, &font_ascent, &font_descent,
                 &text_structure);
    width = text_structure.width;
    
    if(draw)
    {
      XSetFont(env.d, env.gc, getFont(env, flags)->fid);
      XDrawString(env.d, env.w, env.gc, x, y, text, size);
    }
  }
  
  return width;
}

// return a the max width
int printLines(struct printerEnv env, char *text, int gap, int y,
  enum t_type flags, int draw)
{
  char *next;
  int size;
  int width = 0;
  
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
  enum t_type dummy;
  int tmp = drawText(env, text, size, 0, 0, flags, &dummy, 0);
  
  // print line
  if(draw)
    drawText(env, text, size, (env.width - tmp)/2, y, flags, &flags, 1);
  else
    flags = dummy;
  
  // print next lines
  if(*next != '\0')
    width = printLines(env, next, gap, y + env.maxascent + env.maxdescent + gap,
      flags, draw);
  
  if(tmp > width)
    return tmp;
  return width;
}

void printerShow(struct printerEnv env, char* text, enum t_type font)
{
  int gap = 5, padding = 5;
  int nlines = 1,
      width, height;
  char *p = text;
  while((p = strchr(p, '\n')) != NULL)
  {
    p++;
    if(*p == '\0')
      break;
    nlines++;
  }
  
  XClearWindow(env.d, env.w);
  //  max width
  width = printLines(env, text, gap, 0, font, 0);
  height = nlines * (env.maxascent + env.maxdescent + gap) - gap;
  
  // frame
  XSetForeground(env.d, env.gc, env.color_background);
  XFillRectangle(env.d, env.w, env.gc, (env.width - width)/2 - padding,
    env.height - 2*padding - height, width + 2*padding, height + 2*padding);
  XSetForeground(env.d, env.gc, env.color_text);
  
  // text
  printLines(env, text, gap,
    env.height - padding - height + env.maxascent, font, 1);
  XFlush(env.d);
}

void printerClean(struct printerEnv env)
{
  XClearWindow(env.d, env.w);
  XFlush(env.d);
}

