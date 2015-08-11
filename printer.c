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
#include <X11/XKBlib.h>
#include <string.h>

void loadFont(struct printerEnv *env, XFontStruct **font, char* fontname)
{
  *font = XLoadQueryFont(env->d, fontname);
  if(!*font)
  {
    fprintf (stderr, "unable to load font %s - using normal font\n", fontname);
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

struct printerEnv printerOpenWindow(char *font, char *font_i, char *font_b,
  char *font_bi)
{
  struct printerEnv env;

  // open connection
  env.d = XOpenDisplay(NULL);
  if (env.d == NULL) {
      fprintf(stderr, "Unable to open display\n");
      exit(1);
  }
  env.d_fd = ConnectionNumber(env.d);
  env.s = DefaultScreen(env.d);

  XVisualInfo vinfo;
  XMatchVisualInfo(env.d, env.s, 32, TrueColor, &vinfo);

  // size of the screen
  XWindowAttributes RootAttr;
  XGetWindowAttributes(env.d, RootWindow(env.d, env.s), &RootAttr);
  env.root_width = RootAttr.width;
  env.root_height = RootAttr.height;

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
  //unsigned short r = 34, g = 34, b = 34, a = 192; // TODO where is the doc ?
  //env.color_background = a*256*256*256 + r*256*256 + g*256 + b;

  // create the window
  env.w = XCreateWindow(env.d, RootWindow(env.d, env.s), 0, 0, 1, 1, 0, vinfo.depth, InputOutput, vinfo.visual,
    CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attr);

  // setting fonts
  char *fontname = (font == NULL) ?
    "-bitstream-bitstream charter-medium-r-normal--*-280-100-100-p-*-iso8859-1"
    : font;
  env.fontinfo    = XLoadQueryFont(env.d, fontname);
  if(!env.fontinfo)
  {
    fprintf (stderr, "unable to load font %s: using fixed\n", fontname);
    env.fontinfo = XLoadQueryFont(env.d, "fixed");
  }
  env.maxascent = env.fontinfo->ascent;
  env.maxdescent = env.fontinfo->descent;
  loadFont(&env, &env.fontinfo_i, (font_i == NULL) ?
    "-bitstream-bitstream charter-medium-i-normal--*-280-100-100-p-*-iso8859-1"
    : font_i);
  loadFont(&env, &env.fontinfo_b, (font_b == NULL) ?
    "-bitstream-bitstream charter-bold-r-normal--*-280-100-100-p-*-iso8859-1"
    : font_b);
  loadFont(&env, &env.fontinfo_bi, (font_bi == NULL) ?
    "-bitstream-bitstream charter-bold-i-normal--*-280-100-100-p-*-iso8859-1"
    : font_bi);

  // create GC
  XGCValues gr_values;
  gr_values.font = env.fontinfo->fid;
  gr_values.foreground = XWhitePixel(env.d, env.s);
  gr_values.background = XBlackPixel(env.d, env.s);
  env.gc = XCreateGC(env.d, env.w, GCFont | GCForeground, &gr_values);

  // focused window (for keyboard)
  int revert;
  XGetInputFocus(env.d, &env.w_focused, &revert);

  // event to be listened
  XSelectInput(env.d, env.w_focused, KeyPressMask | FocusChangeMask | ExposureMask);

  // empty message set
  env.maxsize = 0;
  env.size = 0;
  env.texts = NULL;
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

int drawTextRaw(struct printerEnv env, char *text, int size, enum t_type font,
    int *x, int *y, int draw)
{
  int font_direction, font_ascent, font_descent;
  XCharStruct text_structure;
  XTextExtents(getFont(env, font), text, size,
               &font_direction, &font_ascent, &font_descent,
               &text_structure);
  if(draw)
  {
    XSetFont(env.d, env.gc, getFont(env, font)->fid);
    XDrawString(env.d, env.w, env.gc, *x, *y, text, size);
  }
  *x += text_structure.width;
}

int drawTextLines(struct printerEnv env, char *text, int size, enum t_type font,
    int initx, int *x, int *y, int *maxw, int draw)
{
  char *nl = memchr(text, '\n', size);
  if(nl == NULL)
  {
    drawTextRaw(env, text, size, font, x, y, draw);
    if(*x > *maxw)
      *maxw = *x;
  }
  else
  {
    drawTextRaw(env, text, nl-text, font, x, y, draw);
    if(*x > *maxw)
      *maxw = *x;
    *x = initx; // TODO center
    *y += env.maxascent + env.maxdescent + env.gap;
    drawTextLines(env, nl+1, text-nl-1+size, font, initx, x, y, maxw, draw);
  }
}

// update x and y
int drawText(struct printerEnv env, struct richText *rt,
    int initx, int *x, int *y, int *maxw, int draw)
{
  if(rt->left != NULL && rt->right != NULL)
  {
    drawText(env, rt->left, initx, x, y, maxw, draw);
    drawText(env, rt->right, initx, x, y, maxw, draw);
  }
  else
    drawTextLines(env, rt->pos, rt->size, rt->type, initx, x, y, maxw, draw);
}

void printerRender(struct printerEnv *env)
{
  if(env->size == 0)
  {
    XClearWindow(env->d, env->w);
    XUnmapWindow(env->d, env->w);
  }
  else
  {
    int i;
    int w = 0, h = 0;
    // remap the window
    XMapWindow(env->d, env->w);
    XClearWindow(env->d, env->w);

    // resize the window
    for(i = 0; i < env->size; i++)
    {
      if(env->texts[i].width > w)
        w = env->texts[i].width;
      if(-env->texts[i].posy > h)
        h = -env->texts[i].posy;
    }
    w += 2*env->padding;
    h += 2*env->padding;
    XMoveResizeWindow(env->d, env->w, (env->root_width - w)/2,
      env->root_height - env->margin_bottom - h, w, h);

    // draw texts
    for(i = 0; i < env->size; i++)
    {
      int x = (w-env->texts[i].width)/2;
      int y = h+env->texts[i].posy - env->padding;
      // frame
      XSetForeground(env->d, env->gc, env->color_background);
      XFillRectangle(env->d, env->w, env->gc, x - env->padding, y - env->padding,
        env->texts[i].width + 2*env->padding,
        env->texts[i].height + 2*env->padding);
      XSetForeground(env->d, env->gc, env->color_text);
      // text
      y += env->maxascent;
      drawText(*env, env->texts[i].t, x, &x, &y, &x, 1);
    }
  }
  XFlush(env->d);
}

void printerHide(struct printerEnv *env, int id)
{
  int i;
  for(i = 0; i < env->size; i++)
    if(env->texts[i].id == id)
      break;
  if(i < env->size)
  {
    // delete it
    memmove(env->texts + i, env->texts + (i + 1),
      sizeof(struct message[env->size-i-1]));
    env->size--;
  }
}

int getPos(struct printerEnv *env, int h)
{
  int pos = -h, i;
  // change pos until it works
  while(1)
  {
    // is it ok betwen pos and pos+h ?
    int i, gap = env->gap2 + 2 * env->padding;
    for(i = 0; i < env->size; i++)
    {
      int pos2 = env->texts[i].posy;
      int h2 = env->texts[i].height;
      if(h2 + pos2 <= pos - gap)
        continue;
      if(pos2 >= pos + h + gap)
        continue;
      pos = pos2 - h - gap;
      break;
    }
    if(i == env->size)
      return pos;
  }
}

void printerShow(struct printerEnv *env, struct richText *rt, int id)
{
  int w = 0, h = 0, x = 0;
  // hide previous message with this id
  printerHide(env, id);
  if(env->maxsize == 0) // first call
  {
    env->maxsize = 2;
    env->texts = malloc(sizeof(struct message[env->maxsize]));
  }
  else if(env->size == env->maxsize) // too small
  {
    env->maxsize *= 2;
    env->texts = realloc(env->texts, sizeof(struct message[env->maxsize]));
  }
  if(env->texts == NULL)
  {
    perror("[re|m]alloc()");
    exit(1);
  }
  env->texts[env->size].t = rt;
  env->texts[env->size].id = id;

  // compute w and h
  drawText(*env, rt, 0, &x, &h, &w, 0);

  // 0 is the bottom
  env->texts[env->size].posy = getPos(env, h);
  env->texts[env->size].height = h;
  env->texts[env->size].width = w;
  env->size++;
  printerRender(env);
}

t_event manageEvent(struct printerEnv *env)
{
  XEvent event;
  int r;
  XComposeStatus comp;
  t_event e;

  XNextEvent(env->d, &event);
  switch(event.type)
  {
    case Expose:
      if(event.xexpose.count == 0) // last expose event
        printerRender(env);
      break;

    case FocusOut:
      XGetInputFocus(env->d, &env->w_focused, &r);
      if(env->w_focused == PointerRoot)
        env->w_focused = RootWindow(env->d, env->s); // TODO why ?
      XSelectInput(env->d, env->w_focused, KeyPressMask|FocusChangeMask);
      event.type = T_NONE;
      break;

    case KeyPress:
      e.type = T_KEYPRESSED;
      e.keyPressed.key = (int)XkbKeycodeToKeysym(env->d, event.xkey.keycode, 0,
        event.xkey.state & ShiftMask ? 1 : 0);
      // e.keyPress.time = TODO (event.xkey.time is the number of milliseconds (from uptime ?)
      break;

    default:
      e.type = T_NONE;
      break;
  }
  return e;
}
