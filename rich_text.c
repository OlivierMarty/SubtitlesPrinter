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
#include <string.h>
#include <stdlib.h>


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

/*char *strnstr(const char *s, const char *find, size_t slen)
{
  return (char*)memmem((void*)s, slen, (void*)find, strlen(find));
}*/

struct richText* newRt(struct richText *left, struct richText *right,
  char* pos, int size, enum t_type type)
{
  struct richText *rt = malloc(sizeof(struct richText));
  if(rt == NULL)
  {
    perror("malloc()");
    exit(1);
  }
  rt->left = left;
  rt->right = right;
  rt->pos = pos;
  rt->size = size;
  rt->type = type;
  return rt;
}

// rt must be of type data (pos and size)
// inflate rt
void parseAux(struct richText *rt) {
  char *text = rt->pos;
  int size = rt->size;
  char *i = strnstr(text, "<i>", size),
    *b = strnstr(text, "<b>", size);
  char *end, *begin = NULL;
  if(i == text || b == text) // text start with <i> or <b>
  {
    enum t_type type = rt->type;
    if(i == text)
    {
      type |= T_ITALIC;
      end = strnstr(text, "</i>", size); // TODO reverse for nested tags
    }
    else
    {
      type |= T_BOLD;
      end = strnstr(text, "</b>", size);
    }
    if(end != NULL)
    {
      if(end == text + size - 4) // text end with end tag
      {
        // we do not create a node
        rt->pos = text+3;
        rt->size = end-text-3;
        rt->type = type;
        parseAux(rt);
      }
      else
      {
        rt->left = newRt(NULL, NULL, text+3, end-text-3, type);
        rt->right = newRt(NULL, NULL, end+4, text+size-end-4, rt->type);
        parseAux(rt->left);
        parseAux(rt->right);
      }
    }
    else // tags do not match
    {
      rt->left = newRt(NULL, NULL, text, 3, rt->type);
      rt->right = newRt(NULL, NULL, text+3, size-3, rt->type);
      // rt->left is not inflated
      parseAux(rt->right);
    }
  }
  else if(i != NULL || b != NULL) // there is a tag in the text
  {
    char *pos = i;
    if(i == NULL || (b != NULL && b < i))
      pos = b;
    // pos : first tag
    rt->left = newRt(NULL, NULL, text, pos-text, rt->type);
    rt->right = newRt(NULL, NULL, pos, text+size-pos, rt->type);
    parseAux(rt->right);
  }
}

struct richText* richTextParse(char* data) {
  struct richText *rt = newRt(NULL, NULL, data, strlen(data), T_REGULAR);
  parseAux(rt);
  rt->raw = data;
  return rt;
}

void richTextFree(struct richText *rt) {
  if(rt->left != NULL)
    richTextFree(rt->left);
  if(rt->right != NULL)
    richTextFree(rt->right);
  free(rt);
}
