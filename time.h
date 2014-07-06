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

#ifndef H_TIME
#define H_TIME

#include <time.h>

typedef struct timespec mytime;

void timeInitialize(int rel); // add rel seconds to the clock
mytime timeGetRelative();
int timeSleepUntil(mytime t);

mytime timeCreate(time_t s, long ns);

int timeNanoSleep(mytime t);
mytime timeDiff(mytime a, mytime b);

#endif

