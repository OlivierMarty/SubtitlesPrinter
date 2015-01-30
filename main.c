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

#include "parser.h"
#include "time.h"
#include "printer.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void displayUsage(char *name)
{
  printf("Usage : %s file.srt\n", name);
  printf("Options :\n");
  printf("  -s sec\t: skip the first x seconds\n");
  printf("  -d sec\t: wait x seconds before starting (default : 5)\n");
  printf("  -t x\t\t: time factor x1000\n");
  printf("  -m px\t\t: margin with the bottom of the screen\n");
  printf("  -p px\t\t: padding of the box\n");
  printf("  -g px\t\t: gap between two lines\n");
  printf("  -f fontname\t: name of the font to use\n");
  printf("  -i fontname\t: name of the italic font to use\n");
  printf("  -b fontname\t: name of the bold font to use\n");
  printf("  -j fontname\t: name of the bold and italic font to use\n");
  printf("  -h\t\t: display this help and exit\n");
}

void callbackEvent(struct printerEnv* env, int key, void* a) {
  if(key == ' ') {
    // display a message
    if(!timeIsPaused())
      printerShow(env, "(paused - press space to resume)", 0);
    else
      printerClean(*env);
    
    // toggle pause
    timePause(!timeIsPaused());
    // if paused, wait for an event
    while(timeIsPaused())
    {
      waitEvent(env);
      manageEvent(env, callbackEvent, a);
    }
  }
}

int main(int argc, char **argv)
{
  int i, shift = 0, delay = 5, margin_bottom = 50, padding = 5, gap = 5;
  float factor = 1.0;
  char *font = NULL, *font_i = NULL, *font_b = NULL, *font_bi = NULL;
  FILE *f = NULL;
  
  // parse arguments
  int c;
  while((c = getopt (argc, argv, "s:d:t:m:p:g:f:i:b:j:h")) != -1)
    switch(c)
    {
      case 's':
        shift = atoi(optarg);
        break;
      case 'd':
        delay = atoi(optarg);
        break;
      case 't':
        factor = (float)atoi(optarg)/1000;
        break;
      case 'm':
        margin_bottom = atoi(optarg);
        break;
      case 'p':
        padding = atoi(optarg);
        break;
      case 'g':
        gap = atoi(optarg);
        break;
      case 'f':
        font = malloc(strlen(optarg)+1);
        strcpy(font, optarg);
        break;
      case 'i':
        font_i = malloc(strlen(optarg)+1);
        strcpy(font_i, optarg);
        break;
      case 'b':
        font_b = malloc(strlen(optarg)+1);
        strcpy(font_b, optarg);
        break;
      case 'j':
        font_bi = malloc(strlen(optarg)+1);
        strcpy(font_bi, optarg);
        break;
      case 'h':
        displayUsage(argv[0]);
        return 0;
      //case '?':
      default:
        return 1;
    }
  
  if(optind >= argc)
  {
    fprintf(stderr, "Missing filename.\n");
    displayUsage(argv[0]);
    return 1;
  }
  
  f = fopen(argv[optind], "r");
  if(f == NULL)
  {
    perror("fopen()");
    exit(1);
  }
  
  // open the window
  struct printerEnv penv = printerOpenWindow(font, font_i, font_b, font_bi);
  
  // set attributes
  penv.margin_bottom = margin_bottom;
  penv.padding = padding;
  penv.gap = gap;
  
  // show a counter before start the clock
  for(i = delay; i > 0; i--)
  {
    char t[16];
    sprintf(t, "%d...\n", i);
    printf("%s", t);
    printerShow(&penv, t, T_ITALIC);
    sleep(1);
  }
  printf("0 !\n");
  printerClean(penv);
  timeInitialize(-factor*shift);
  
  struct SubtitleLine sline;
  int id = 0;
  while(!feof(f))
  {
    id = next(f, id+1, &sline);
    if(timeInFuture(timeFactor(sline.end, factor)))
    {
      timeSleepUntil(timeFactor(sline.begin, factor));
      
      printf("%ds\n", sline.begin.tv_sec);
      // show
      printf("%s\n", sline.text);
      printerShow(&penv, sline.text, 0);
      
      // hide
      timeSleepUntil(timeFactor(sline.end, factor));
      // TODO manage when the next subtitle appear before
      printf("\n");
      printerClean(penv);
      manageEvent(&penv, callbackEvent, NULL);
    }
    else
    {
      printf("skipped :\n");
      printf("%s\n", sline.text);
    }
  }
  
  printerCloseWindow(penv);
  fclose(f);
}

