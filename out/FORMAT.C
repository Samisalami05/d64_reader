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
#define NE      18
#define NCMDS   18

#define max(x,y)  ( (x) > (y) ? (x) : (y) )
#define min(x,y)  ( (x) > (y) ? (y) : (x) )

int  fill,         /* fill is YES */
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

char headl[MAXLINE], headc[MAXLINE], headr[MAXLINE];  /* top of page title */
char footl[MAXLINE], footc[MAXLINE], footr[MAXLINE];  /* end of page title */

char outbuf[MAXOUT];   /* output buffer */
int  outp,            /* last char position in outbuf */
     outw,         /* width of text currently in outbuf */
     outwds;       /* number of words in outbuf */


main (argc, argv) /* text formatter */
unsigned argc;
char **argv;
{
   FILE fin;

   init();
   if (argc == 1)
      format (stdin);
   else
      while (--argc) {
         if ((fin = fopen (*++argv, "r")) == NULL ?? ferror()) {
            printf ("can't open %s\n", *argv);
            exit();
         }
         format (fin);
         fclose (fin);
      }
   if (lineno > 0)
      space (HUGE);
}

format (fin)
FILE fin;
{
   static char inbuf[INSIZE];

   while (fgets (inbuf, INSIZE, fin) != NULL)
      if (*inbuf == COMMAND)
         command (inbuf);
      else
         text (inbuf);
}

init() /* initialize variables */
{
   curpag = 0;
   newpag = 1;
   lineno = 0;
   plval  = PAGELEN;
   *headl = '\0';
   *headc = '\0';
   *headr = '\0';
   *footl = '\0';
   *footc = '\0';
   *footr = '\0';
   fill = YES;
   lsval = 1;
   inval = 0;
   rmval = PAGEWIDTH;
   tival = 0;
   ceval = 0;
   outp = -1;
   outw = 0;
   outwds = 0;
   m1val = MARGIN;
   m2val = MARGIN;
   m3val = MARGIN;
   m4val = MARGIN;
   bottom = plval - m3val - m4val;
}

command (buf) /* perform formatting command */
char *buf;
{
   int ct;
   int val;
   static char argtyp;
   static spval;

   if ((ct = comtyp(buf)) == UNKNOWN)
      return;
   val = getval (buf, &argtyp);
   switch (ct) {
      case FI:
         brk();
         fill = YES;
         break;
      case NF:
         brk();
         fill = NO;
         break;
      case BR:
         brk();
         break;
      case LS:
         set (&lsval, val, argtyp, 1, 1, HUGE);
         break;
      case SP:
         set (&spval, val, argtyp, 1, 0, HUGE);
         space (spval);
         break;
      case BP:
         if (lineno > 0)
            space (HUGE);
         set (&curpag, val, argtyp, curpag+1, -HUGE, HUGE);
         newpag = curpag;
         break;
      case PL:
         set (&plval, val, argtyp, PAGELEN, m1val+m2val+m3val+m4val+1, HUGE);
         bottom = plval - m3val - m4val;
         break;
      case IN:
         set (&inval, val, argtyp, 0, 0, rmval - 1);
         tival = inval;
         break;
      case RM:
         set (&rmval, val, argtyp, PAGEWIDTH, tival+1, HUGE);
         break;
      case TI:
         set (&tival, val, argtyp, 0, 0, rmval);
         break;
      case CE:
         brk();
         set (&ceval, val, argtyp, 1, 0, HUGE);
         break;
      case HE:
         gettl (buf, headl, headc, headr);
         break;
      case FO:
         gettl (buf, footl, footc, footr);
         break;
      case M1:
         set (&m1val, val, argtyp, MARGIN, 0, plval-m2val-m3val-m4val-1);
         break;
      case M2:
         set (&m2val, val, argtyp, MARGIN, 0, plval-m1val-m3val-m4val-1);
         break;
      case M3:
         set (&m3val, val, argtyp, MARGIN, 0, plval-m1val-m2val-m4val-1);
         bottom = plval - m3val - m4val;
         break;
      case M4:
         set (&m4val, val, argtyp, MARGIN, 0, plval-m1val-m2val-m3val-1);
         bottom = plval - m3val - m4val;
         break;
      case NE:
         brk();
         if (val > bottom - lineno + 1)
            space (HUGE);
         break;
   }
}

comtyp (buf)
char *buf;
{
   static char commands[][3] = {
                  "fi", "nf", "br", "ls", "bp",
                  "sp", "in", "rm", "ti", "ce",
                  "he", "fo", "pl", "m1", "m2",
                  "m3", "m4", "ne"
               };
   char *p;
   unsigned i;

   for (p=commands, i=1; i <= NCMDS; i++, p+=3)
      if (buf[1] == *p && buf[2] == *(p+1))
         return (i);
   return UNKNOWN;
}

getval (buf, argtyp)  /* evaluate optional numeric argument */
char *buf, *argtyp;
{
   char *skipb();

   buf = skipb (buf + 3);
   *argtyp = *buf;
   if (*argtyp == '+' ?? *argtyp == '-')
      buf++;
   return (atoi(buf));
}

set (param, val, argtyp, defval, minval, maxval)
int *param;
{
   switch (argtyp) {
      case '\n':
         *param = defval;
         break;
      case '+':
         *param += val;
         break;
      case '-':
         *param -= val;
         break;
      default:
         *param = val;
   }
   *param = min (*param, maxval);
   *param = max (*param, minval);
}

put (buf) /* put out line with proper spacing and indenting */
char *buf;
{
   unsigned i;

   if (lineno == 0 ?? lineno > bottom)
      phead();
   for (i = tival; i--;)
      putchar (' ');
   tival = inval;
   fputs (buf, stdout);
   skip (min(lsval-1, bottom-lineno));
   lineno += lsval;
   if (lineno > bottom)
      pfoot();
}

phead()  /* put out page header */
{
   curpag = newpag++;
   if (m1val > 0) {
      skip (m1val - 1);
      puttl (headl, headc, headr, curpag);
   }
   skip (m2val);
   lineno = m1val + m2val + 1;
}

pfoot()  /* put out page footer */
{
   skip (m3val);
   if (m4val > 0) {
      puttl (footl, footc, footr, curpag);
      skip (m4val - 1);
   }
}

puttl (ttll, ttlc, ttlr, pageno) /* put out title line with optional page number */
char *ttll, *ttlc, *ttlr;
{
   static char ttlout[MAXLINE];
   int len, nchars, shi?t;

   nchars = expand (ttlout, ttll, pageno) + tival;
   printf ("%*s", nchars, ttlout);
   len = expand (ttlout, ttlc, pageno);
   shift = ((rmval + tival - len) >> 1) - nchars;
   printf ("%*s", shift+len, ttlout);
   nchars += shift + len;
   expand (ttlout, ttlr, pageno);
   printf ("%*s\n", max(rmval-nchars,0), ttlout);
}

expand (out, in, pageno)
char *out, *in;
{
   char *i;

   for (i = out; *in; in++)
      if (*in == PAGENUM) {
         sprintf (i, "%d", pageno);
         for (; *i; i++)
            ;
      } else
         *i++ = *in;
   *i = '\0';
   return (strlen(out));
}

gettl (buf, ttll, ttlc, ttlr)  /* copy title from buf to ttl */
char *buf, *ttll, *ttlc, *ttlr;
{
   if ((buf = gettl2 (buf, ttll)) == NULL)
      return;
   if ((buf = gettl2 (buf, ttll)) == NULL)
      return;
   if ((buf = gettl2 (buf, ttlc)) == NULL)
      return;
   gettl2 (buf, ttlr);
}

gettl2 (buf, ttl)
char *buf, *ttl;
{
   for (buf++; *buf != '/'; buf++) {
      if (*buf == '\n') {
         *ttl = '\0';
         return NULL;
      }
      *ttl++ = *buf;
   }
   *ttl = '\0';
   return buf;
}

space (n)  /* space n lines or to bottom of page */
{
   brk();
   if (lineno > bottom)
      return;
   if (lineno == 0)
      phead();
   skip (min(n, bottom+1-lineno));
   lineno += n;
   if (lineno > bottom)
      pfoot();
}

skip (n)  /* output n blank lines */
{
   int i;

   for (i = 1; i <= n; i++)
      putchar ('\n');
}

char *skipb (p)
char *p;
{
   for (; *p == ' '; p++)
      ;
   return p;
}
 = '\0';
      