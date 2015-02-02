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

#ifndef H_EVENTS
#define H_EVENTS

#include "rich_text.h"
#include "time.h"

enum _e_type { T_NONE, T_KEYPRESSED, T_SHOW, T_HIDE };
typedef enum _e_type e_type;

struct eventAny {
  e_type type;
  mytime time;
};

struct eventKeyPressed {
  e_type type;
  mytime time;
  int key;
};

struct eventShow {
  e_type type;
  mytime time;
  int id;
  struct richText *rt;
};

struct eventHide {
  e_type type;
  mytime time;
  int id;
  struct richText *rt;
};

union _t_event {
  e_type type;
  struct eventAny any;
  struct eventKeyPressed keyPressed;
  struct eventShow show;
  struct eventHide hide;
};
typedef union _t_event t_event;

// should be implemented as a priority queue
struct _t_events {
  int size;
  int maxsize;
  t_event *events;
};
typedef struct _t_events t_events;

mytime eventsNextTime(t_events events);

// events must be non empty
void eventsPush(t_events *events, t_event e);
t_event eventsPop(t_events *events);

t_events eventsInit(int initSize);
int eventsEmpty(t_events events);

#endif

