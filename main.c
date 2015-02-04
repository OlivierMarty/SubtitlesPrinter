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
#include "rich_text.h"
#include "events.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum _t_state { S_RUNNING, S_PAUSED, S_STOP };
typedef enum _t_state t_state;



void displayUsage(char *name)
{
  printf("Usage : %s file.srt\n", name);
  printf("Options :\n");
  printf("  -s sec\t: skip the first x seconds\n");
  printf("  -d sec\t: wait x seconds before starting (default : 5)\n");
//  printf("  -t x\t\t: time factor x1000\n");
  printf("  -m px\t\t: margin with the bottom of the screen\n");
  printf("  -p px\t\t: padding of the box\n");
  printf("  -g px\t\t: gap between two lines\n");
  printf("  -k px\t\t: gap between two subtitles\n");
  printf("  -f fontname\t: name of the font to use\n");
  printf("  -i fontname\t: name of the italic font to use\n");
  printf("  -b fontname\t: name of the bold font to use\n");
  printf("  -j fontname\t: name of the bold and italic font to use\n");
  printf("  -h\t\t: display this help and exit\n");
}

// display a message and program its end (and free) (one at a time)
void display(struct printerEnv *env, struct richText *rt, int time, t_events *events)
{
  static int id = -1;
  t_event e;
  printerHide(env, id);
  id--;
  e.type = T_HIDE;
  e.hide.time = timeGetRelative();
  e.hide.time.tv_sec += time;
  e.hide.id = id;
  e.hide.rt = rt;
  printerShow(env, rt, id);
  eventsPush(events, e);
}

int main(int argc, char **argv)
{
  int i, delay = 5, margin_bottom = 50, padding = 5, gap = 5, gap2 = 20;
  double factor = 1.0, shift = 0.;
  char *font = NULL, *font_i = NULL, *font_b = NULL, *font_bi = NULL;
  FILE *f = NULL;
  t_state state = S_RUNNING;
  
  // parse arguments
  int c;
  while((c = getopt (argc, argv, "s:d:t:m:p:g:k:f:i:b:j:h")) != -1)
    switch(c)
    {
      case 's':
        shift = atof(optarg);
        break;
      case 'd':
        delay = atoi(optarg);
        break;
      /*case 't':
        factor = atoi(optarg)/1000.;
        break;
      */
      case 'm':
        margin_bottom = atoi(optarg);
        break;
      case 'p':
        padding = atoi(optarg);
        break;
      case 'g':
        gap = atoi(optarg);
        break;
      case 'k':
        gap2 = atoi(optarg);
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
  penv.gap2 = gap2;
  
  // show a counter before start the clock
  for(i = delay; i > 0; i--)
  {
    struct richText *rt;
    char t[16];
    sprintf(t, "<i>%d...</i>\n", i);
    printf("%s\n", t);
    rt = richTextParse(t);
    printerShow(&penv, rt, 0);
    sleep(1);
    printerHide(&penv, 0);
    richTextFree(rt);
  }
  printerRender(&penv);
  printf("0 !\n");
  timeInitialize();
  timeShift(-factor*shift);
  
  int id = 0;
  struct richText *pausert = richTextParse("<i>paused...</i>\n");
  t_events events = eventsInit(8);
  t_event event;
  event.type = T_SHOW;
  event.show.id = -1; // first
  while(1)
  {
    switch(state)
    {
      case S_RUNNING:
        switch(event.type)
        {
          case T_KEYPRESSED:
            switch(event.keyPressed.key)
            {
              case ' ':
                state = S_PAUSED;
                printf("paused...\n");
                printerShow(&penv, pausert, 0);
                timePause(1);
                break;
              case 65363: // right shift
                shift += 0.05;
                printf("shift : +0.05s (total : %.2fs)\n", shift);
                timeShift(-0.05);
                {
                  char *msg = malloc(64);
                  sprintf(msg, "<i>shift %.2fs</i>\n", shift);
                  display(&penv, richTextParse(msg), 1, &events);
                }
                break;
              case 65361: // left shift
                shift -= 0.05;
                printf("shift : -0.05s (total : %.2fs)\n", shift);
                timeShift(-0.05);
                {
                  char *msg = malloc(64);
                  sprintf(msg, "<i>shift %.2fs</i>\n", shift);
                  display(&penv, richTextParse(msg), 1, &events);
                }
                break;
            }
            break;
          case T_HIDE:
            printerHide(&penv, event.hide.id);
            printerRender(&penv);
            free(event.hide.rt->raw);
            richTextFree(event.hide.rt);
            printf("\n");
            break;
          case T_SHOW:
            if(event.hide.id >= 0)
            {
              printf("%ds\n", timeFactor(event.show.time, 1./factor).tv_sec);
              printf("%s", event.show.rt->raw);
              printerShow(&penv, event.show.rt, event.show.id);
            }
            // grab next subtitles
            while(1)
            {
              struct SubtitleLine sline;
              if(feof(f))
                break;
              id = next(f, id+1, &sline);
              if(timeInFuture(timeFactor(sline.end, factor)))
              {
                char *copy = NULL;
                struct richText *rt;
                copy = malloc(sizeof(char[strlen(sline.text)+1]));
                if(copy == NULL)
                {
                  perror("malloc()");
                  exit(1);
                }
                strcpy(copy, sline.text);
                rt = richTextParse(copy);
                // show event
                t_event show, hide;
                show.type = T_SHOW;
                show.show.id = id;
                show.show.rt = rt;
                show.show.time = timeFactor(sline.begin, factor);
                // hide event
                hide.type = T_HIDE;
                hide.hide.id = id;
                hide.hide.rt = rt;
                hide.hide.time = timeFactor(sline.end, factor);
                eventsPush(&events, show);
                eventsPush(&events, hide);
                break;
              }
              else
              {
                printf("skipped :\n");
                printf("%s\n", sline.text);
              }
            }
            break;
        }
        break;
      case S_PAUSED:
        switch(event.type)
        {
          case T_KEYPRESSED:
            if(event.keyPressed.key == ' ')
            {
              state = S_RUNNING;
              printf("end\n");
              printerHide(&penv, 0);
              printerRender(&penv);
              timePause(0);
            }
            break;
        }
        break;
    }
    
    if(eventsEmpty(events))
      state = S_STOP;
    if(state == S_STOP)
      break;
    
    // sleep until next event
    mytime next_event = eventsNextTime(events);
    while(1)
    {
      int value;
      fd_set in_fds;
      FD_ZERO(&in_fds);
      FD_SET(penv.d_fd, &in_fds);
      if(XPending(penv.d)) // event waiting (pselect does no see events that come
      // during the beginning of the program
        value = 1;
      else if(state == S_PAUSED) // blocking select
        value = pselect(penv.d_fd+1, &in_fds, NULL, NULL, NULL, NULL);
      else // timeout
      {
        struct timespec to_wait = timeDiff(next_event, timeGetRelative());
        if(to_wait.tv_sec < 0)
        {
          to_wait.tv_sec = 0;
          to_wait.tv_nsec = 0;
        }
        value = pselect(penv.d_fd+1, &in_fds, NULL, NULL, &to_wait, NULL);
      }
      if(value == -1) // TODO tester interruption par un signal : return 0 ou -1
      // dans les deux cas c'est foireux
      {
        perror("pselect()");
        exit(1);
      }
      else if(value > 0) // x event
      {
        while(XPending(penv.d))
        {
          event = manageEvent(&penv);
          if(event.type == T_KEYPRESSED)
            break;
        }
        if(event.type == T_KEYPRESSED)
          break;
      }
      else // deadline
      {
        event = eventsPop(&events);
        break;
      }
    }
  }
  
  richTextFree(pausert);
  printerCloseWindow(penv);
  fclose(f);
}
