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

#include "time.h"
#include <errno.h>
#include <stdlib.h>

void normalize(struct timespec *t)
{
  // TODO compute directly ?
  while(t->tv_nsec < 0)
  {
    t->tv_nsec += 1000000000;
    t->tv_sec -= 1;
  }
  while(t->tv_nsec >= 1000000000)
  {
    t->tv_nsec -= 1000000000;
    t->tv_sec += 1;
  }
}

struct timespec timeDiff(struct timespec a, struct timespec b)
{
  struct timespec r;
  r.tv_sec = a.tv_sec - b.tv_sec;
  r.tv_nsec = a.tv_nsec - b.tv_nsec;
  normalize(&r);
  return r;
}

// f should be >= 0
struct timespec timeFactor(struct timespec a, double f)
{
  /* bogued code :
  struct timespec r;
  r.tv_sec = f*a.tv_sec;
  r.tv_nsec = f*a.tv_nsec;
  while(r.tv_nsec > 1000000000)
  {
    r.tv_nsec -= 1000000000;
    r.tv_sec += 1;
  }
  return r;
  */
  return a;
}

struct timespec begin;
struct timespec tPause; // beginning of the pause
int pause;
void timeInitialize()
{
  pause = 0;
  if(clock_gettime(CLOCK_REALTIME, &begin) < 0)
  {
    perror("clock_gettime()");
    exit(1);
  }
}

void timeShift(double rel)
{
  begin.tv_nsec += rel*1000000000.;
  normalize(&begin);
}

struct timespec timeGetRelative()
{
  if(pause)
    return tPause;

  struct timespec r;
  if(clock_gettime(CLOCK_REALTIME, &r) < 0)
  {
    perror("clock_gettime()");
    exit(1);
  }
  return timeDiff(r, begin);
}

struct timespec timeCreate(time_t s, long ns)
{
  struct timespec r;
  r.tv_sec = s;
  r.tv_nsec = ns;
  return r;
}

int timeInFuture(struct timespec t)
{
  struct timespec tmp = timeGetRelative();
  tmp = timeDiff(t, tmp);
  return tmp.tv_sec >= 0;
}

void timePause(int b)
{
  if(b)
  {
    tPause = timeGetRelative();
    pause = 1;
  }
  else
  {
    pause = 0;
    begin = timeDiff(begin, timeDiff(tPause, timeGetRelative()));
  }
}

int timeIsPaused()
{
  return pause;
}
