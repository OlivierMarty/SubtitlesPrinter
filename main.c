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

#include "parser.h"
#include "time.h"
#include "printer.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  int i;
  FILE *f= NULL;
  
  if(argc < 2)
  {
    fprintf(stderr, "Usage : subtitles file.srt [shift in seconds [waiting time in seconds]]\n");
    return 1;
  }
  
  f = fopen(argv[1], "r");
  if(f == NULL)
  {
    perror("fopen()");
    exit(1);
  }
  
  // open the window
  struct printerEnv penv = printerOpenWindow(-1, 240, 0);
  
  // show a counter before start the clock
  for(i = (argc > 3) ? atoi(argv[3]) : 5; i > 0; i--)
  {
    char t[16];
    sprintf(t, "%d...\n", i);
    printf("%s", t);
    printerShow(penv, t, T_ITALIC);
    sleep(1);
  }
  printf("0 !\n");
  printerClean(penv);
  timeInitialize((argc > 2) ? -atoi(argv[2]) : 0);
  
  struct SubtitleLine sline;
  int id = 0;
  while(!feof(f))
  {
    id = next(f, id+1, &sline);
    timeSleepUntil(sline.begin);
    
    // show
    printf("%s\n", sline.text);
    printerShow(penv, sline.text, 0);
    
    // hide
    timeSleepUntil(sline.end);
    // TODO manage when the next subtitle appear before
    printf("\n");
    printerClean(penv);
  }
  
  printerCloseWindow(penv);
  fclose(f);
}

