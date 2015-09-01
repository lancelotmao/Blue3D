/* Copyright (C) 2000-2002, 2005-2006, 2008-2009 Free Software Foundation, Inc.
   This file is part of the GNU LIBICONV Library.

   The GNU LIBICONV Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   The GNU LIBICONV Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU LIBICONV Library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Creates the flags.h include file. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Consider all encodings, including the system dependent ones. */
#define USE_AIX
#define USE_OSF1
#define USE_DOS
#define USE_EXTRA

struct loop_funcs {};
struct iconv_fallbacks {};
struct iconv_hooks {};
#include "converters.h"

static void emit_encoding (struct wctomb_funcs * ofuncs, const char* c_name)
{
  /* Prepare a converter struct. */
  struct conv_struct conv;
  memset(&conv,'\0',sizeof(conv));
  conv.ofuncs = *ofuncs;

  {
    /* See whether the encoding can encode the accents and quotation marks. */
    ucs4_t probe[6] = { 0x0060, 0x00b4, 0x2018, 0x2019, 0x3131, 0x3163, };
    int res[6];
    int i;
    for (i = 0; i < 6; i++) {
      unsigned char buf[10];
      memset(&conv.ostate,'\0',sizeof(state_t));
      res[i] = (conv.ofuncs.xxx_wctomb(&conv,buf,probe[i],sizeof(buf)) != RET_ILUNI);
    }
    printf("#define ei_%s_oflags (",c_name);
    {
      int first = 1;
      if (res[0] && res[1]) {
        printf("HAVE_ACCENTS");
        first = 0;
      }
      if (res[2] && res[3]) {
        if (!first) printf(" | ");
        printf("HAVE_QUOTATION_MARKS");
        first = 0;
      }
      if (res[4] && res[5]) {
        if (!first) printf(" | ");
        printf("HAVE_HANGUL_JAMO");
        first = 0;
      }
      if (first) printf("0");
    }
    printf(")\n");
  }
}
