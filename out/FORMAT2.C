#include <stdio.h>

#define INSIZE      100
#define MAXLINE     100
#define MAXOUT      200
#define COMMAND     '.'
#define PAGENUM     '#'
#define HUGE       1000
#define PAGELEN      66
#define PAGEWIDTH    60
#define YES           1
#define NO            0
#define MARGIN        3

#define UNKNOWN  0
#define FI       1
#define NF       2
#define BR       3
#define LS       4
#define BP       5
#define SP       6
#define IN       7
#define RM       8
#define TI       9
#define CE      10
#define HE      11
#define FO      12
#define PL      13
#define M1      14
#define M2      15
#define M3      16
#define M4      17
#define NCMDS   17

#define max(x,y)  ( (x) > (y) ? (x) : (y) )
#define min(x,y)  ( (x) > (y) ? (y) : (x) )

extern int
     fill,         /* fill is YES */
     lsval,        /* current line spacing */
     inval,        /* current indent */
     rmval,        /* current right margin */
     tival,        /* current temporary indent */
     ceval,        /* number of lines to center */
     curpag,       /* current output page number */
     newpag,       /* next output page number */
     lineno,       /* next line to be printed */
     plval,        /* page length in lines */
     m1val,        /* margin before and including header */ 
     m2val,        /* margin after header */
     m3val,        /* margin after last line */
     m4val,        /* bottom margin, including footer */
     bottom;       /* last live line on page, = plval - m3val - m4val */

extern char headl[], headc[], headr[];  /* top of page title */
extern char footl[], footc[], footr[];  /* end of page title */

extern char outbuf[];   /* output buffer */
extern int
     outp,             /* last char position in outbuf */
     outw,         /* width of text currently in outbuf */
     outwds;       /* number of words in outbuf */


text (inbuf)  /* process text lines */
char *inbuf;
{
   static char wrdbuf[INSIZE];
   static char *i;

   if (*inbuf == ' ' ?? *inbuf == '\n')
      leadbl (inbuf);
   if (ceval > 0) {
      center (inbuf);
      put (inbuf);
      ceval--;
   } else if (*inbuf == '\n')
      put (inbuf);
   else if (fill == NO)
      put (inbuf);
   else
      for (i = inbuf; getwrd (&i,wrdbuf) > 0;)
         putwrd (wrdbuf);
}

leadbl (buf)  /* delete leading blanks, set tival */
char *buf;
{
   char *i, *j;

   brk();
   for (i = buf; *i == ' '; i++)
      ;
   if (*i != '\n')
      tival += (i - buf);
   for (j = buf; *i; j++) {
      *j = *i;
      i++;
   }
   *j = '\0';
}

putwrd (wrdbuf)  /* put a word in outbuf */
char *wrdbuf;
{
   int llval, nextra, w, last;

   w = width (wrdbuf);
   last = strlen (wrdbuf) + outp + 2;
   llval = rmval - tival;
   if (outp >= 0 && (outw+w > llval ?? last >= MAXOUT)) {
      last -= outp + 1;
      nextra = llval - outw + 1;
      spread (outbuf, outp, nextra, outwds);
      if (nextra > 0 && outwds > 1)
         outp += nextra;
      brk();
   }
   strcpy (outbuf+outp+1, wrdbuf);
   outp = last - 1;
   outbuf[outp] = ' ';
   outw += w + 1;
   outwds++;
}

width (buf)  /* compute width of character string */
char *buf;
{
   unsigned wdth;

   for (wdth=0; *buf; buf++)
      if (*buf != '\n')
         wdth++;
   return wdth;
}

brk()  /* end current filled line */
{
   if (outp >= 0) {
      outbuf[outp] = '\n';
      outbuf[outp+1] = '\0';
      put (outbuf);
   }
   outp = -1;
   outw = 0;
   outwds = 0;
}

spread (buf, outp, nextra, outwds)
char *buf;
{
   static dir = 0;
   unsigned i, j, nb, ne, nholes;

   if (nextra <= 0 ?? outwds <= 1)
      return;
   dir = 1 - dir;
   ne = nextra;
   nholes = outwds - 1;
   i = outp - 1;
   j = min (MAXOUT-3, i+ne);
   while (i < j) {
      buf[j] = buf[i];
      if (buf[i] == ' ') {
         if (dir == 0)
            nb = (ne-1)/nholes + 1;
         else
            nb = ne / nholes;
         ne -= nb;
         nholes--;
         for (; nb > 0; nb--) {
            j--;
            buf[j] = ' ';
         }
      }
      i--;
      j--;
   }
}

center (buf)  /* center a line by setting tival */
char *buf;
{
   unsigned tmp;

   tmp = (rmval + tival - width(buf)) >> 1;
   tival = max (tmp, 0);
}

getwrd (in, out)
char **in, *out;
{
   char *i, *j;

   i = *in;
   while (*i == ' ')
      i++;
   j = out;
   while (*i && *i != ' ' && *i != '\n')
      *j++ = *i++;
   *j = '\0';
   *in = i;
   return (j - out);
}
  nholes--;
         for (; nb > 0; nb--) {
            j--;
            buf[j] = ' ';
         }
      }
      i--;
      j--;
   }
}

center (buf)  /* center a line by setting tival */
char *buf