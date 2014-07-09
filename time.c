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

int timeSleep(struct timespec t)
{
  int r = -2;
  if(t.tv_sec >= 0)
  {
    do
    {
      r = nanosleep(&t, &t);
    } while(errno == EINTR);
  }
  return r;
}

struct timespec timeDiff(struct timespec a, struct timespec b)
{
  struct timespec r;
  r.tv_sec = a.tv_sec - b.tv_sec;
  r.tv_nsec = a.tv_nsec - b.tv_nsec;
  if(r.tv_nsec < 0)
  {
    r.tv_nsec += 1000000000;
    r.tv_sec -= 1;
  }
  return r;
}

// f should be >= 0
struct timespec timeFactor(struct timespec a, double f)
{
  struct timespec r;
  r.tv_sec = f*a.tv_sec;
  r.tv_nsec = f*a.tv_nsec;
  while(r.tv_nsec > 1000000000)
  {
    r.tv_nsec -= 1000000000;
    r.tv_sec += 1;
  }
  return r;
}

struct timespec begin;
void timeInitialize(int rel)
{
  if(clock_gettime(CLOCK_REALTIME, &begin) < 0)
  {
    perror("clock_gettime()");
    exit(1);
  }
  begin.tv_sec += rel;
}

struct timespec timeGetRelative()
{
  struct timespec r;
  if(clock_gettime(CLOCK_REALTIME, &r) < 0)
  {
    perror("clock_gettime()");
    exit(1);
  }
  return timeDiff(r, begin);
}

int timeSleepUntil(struct timespec t)
{
  struct timespec current = timeGetRelative();
  return timeSleep(timeDiff(t, current));
}

struct timespec timeCreate(time_t s, long ns)
{
  struct timespec r;
  r.tv_sec = s;
  r.tv_nsec = ns;
  return r;
}

