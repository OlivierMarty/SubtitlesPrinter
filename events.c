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

#include "rich_text.h"
#include "time.h"
#include "string.h"
#include "events.h"
#include <assert.h>
#include <stdlib.h>

t_events eventsInit(int initSize)
{
  t_events events = {0,initSize,NULL};
  assert(initSize > 0);
  events.events = malloc(sizeof(t_event[initSize]));
  if(events.events == NULL)
  {
    perror("malloc()");
    exit(1);
  }
  return events;
}

int id_min(t_events events)
{
  int min = 0, i;
  mytime tmin;
  if(events.size == 0)
    return -1;
  tmin = events.events[0].any.time;
  for(i = 1; i < events.size; i++)
  {
    if(timeDiff(events.events[i].any.time, tmin).tv_sec < 0)
    {
      min = i;
      tmin = events.events[i].any.time;
    }
  }
  return min;
}

// events must be non empty
mytime eventsNextTime(t_events events)
{
  int id = id_min(events);
  assert(id >= 0);
  return events.events[id].any.time;
}

// events must be non empty
t_event eventsPop(t_events *events)
{
  int id = id_min(*events);
  assert(id >= 0);
  t_event e = events->events[id];
  memmove(events->events+id, events->events+(id+1),
    sizeof(t_event[events->size-id-1]));
  events->size--;
  return e;
}

void eventsPush(t_events *events, t_event e)
{
  if(events->size == events->maxsize)
  {
    events->maxsize *= 2;
    events->events = realloc(events->events, sizeof(t_event[events->maxsize]));
    if(events->events == NULL)
    {
      perror("realloc()");
      exit(1);
    }
  }
  events->events[events->size] = e;
  events->size++;
}

int eventsEmpty(t_events events)
{
  return !events.size;
}
