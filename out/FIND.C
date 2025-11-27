#include <stdio.h>

#define  MAXLINE  256
#define  MAXPAT   256
#define  ERR       -3
#define  YES        1
#define  NO         0
#define  CLOSURE  '*'
#define  CLOSIZE    4
#define  COUNT      1
#define  START      3
#define  PREVCL     2
#define  OK         1

#define  EOS     '\0'
#define  CHR      'a'
#define  BOL      '%'
#define  EOL      '$'
#define  ANY      '?'
#define  CCL      '['
#define  NCCL     'n'
#define  CCLEND   ']'
#define  NOT      '!'
#define  ESCAPE   '@'
#define  DASH     '-'

char line[MAXLINE];
char *iret, *jret;


main (argc, argv)
unsigned argc;
char **argv;
{
   static char pat[MAXPAT];
   char *s;
   FILE fin;
   int except, number, prname;

   except = NO;
   number = NO;
   prname = NO;
   while (--argc && **++argv == '-')
      for (s = *argv + 1; *s; s++)
         switch (*s) {
            case 'x':
                 except = YES;
                 break;
            case 'n':
                 number = YES;
                 break;
            case 'f':
                 prname = YES;
                 break;
            default:
                 printf ("find: illegal option %c\n", *s);
                 break;
         }

   if (argc == 0)
      error ("usage: find [-x] [-n] [-f] pattern");

   if (getpat (*argv, pat) == ERR)
      error ("illegal pattern");

   if (argc == 1)
      find (stdin, pat, except, number, prname, "stdin");
   else
      while (--argc) {
         if ((fin = fopen (*++argv, "r")) == NULL ?? ferror()) {
            printf ("can't open %s\n", *argv);
            exit();
         }
         find (fin, pat, except, number, prname, *argv);
         fclose (fin);
      }
}

find (fin, pat, except, number, prname, name)
FILE fin;
char *pat, *name;
{
   int lineno = 0;

   while (fgets(line, MAXLINE, fin) != NULL) {
      lineno++;
      if (match(line, pat) != except) {
         if (prname)
            printf ("%s: ", name);
         if (number)
            printf ("%d: ", lineno);
         fputs (line, stdout);
         }
      }
}

error(s)
char *s;
{
   puts(s);
   exit();
}

match (lin, pat)
char *lin, *pat;
{
   for (; *lin; lin++)
      if (amatch (lin, pat))
         return (YES);
   return (NO);
}

amatch (lin, pat)
char *lin, *pat;
{
   char *i, *j, *offset, *stack;

   stack = pat - 1;
   offset = lin;
   for (j=pat; *j; j += patsiz(j))
      if (*j == CLOSURE) {
         stack = j;
         j += CLOSIZE;
         for (i=offset; *i; i=iret)
            if (omatch (i, j) == NO)
               break;
         *(stack + COUNT) = i - offset;
         *(stack + START) = offset - lin;
         offset = i;
         }
      else if (omatch (offset, j) == NO) {
         for (; stack >= pat; stack = pat + *(stack+PREVCL) - 1)
            if (*(stack + COUNT) > 0)
               break;
         if (stack < pat)
            return (0);
         (*(stack + COUNT))--;
         j = stack + CLOSIZE;
         offset = lin + *(stack+START) + *(stack+COUNT);
         }
      else
         offset = iret;
   return (offset - lin + 1);
}

patsiz(n)
char *n;
{
   switch (*n) {
      case CHR:
         return 2;
      case BOL:
      case EOL:
      case ANY:
         return 1;
      case CCL:
      case NCCL:
         return *(n+1) + 2;
      case CLOSURE:
         return CLOSIZE;
      default:
         error ("in patsiz: can't happen");
   }
}

omatch (i, j)
char *i, *j;
{
   int bump;

   iret = i;
   if (*i == '\0')
      return NO;
   bump = -1;
   switch (*j) {
      case CHR:
         if (*i == *(j+1))
            bump = 1;
         break;
      case BOL:
         if (i == line)
            bump = 0;
         break;
      case ANY:
         if (*i != '\n')
            bump = 1;
         break;
      case EOL:
         if (*i == '\n')
            bump = 0;
         break;
      case CCL:
         if (locate(*i, j+1))
            bump = 1;
         break;
      case NCCL:
         if (*i != '\n' && locate(*i,j+1) == NO)
            bump = 1;
         break;
      default:
         error ("in omatch: can't happen");
   }
   if (bump >= 0) {
      iret = i + bump;
      return YES;
   }
   return NO;
}

locate (c, offset)
char *offset;
{
   char *i;

   for (i=offset+*offset; i>offset; i--)
      if (*i == c)
         return YES;
   return NO;
}

getpat (arg, pat)
char *arg, *pat;
{
   return (makpat(arg, EOS, pat));
}

makpat (from, delim, pat)
char *from, *pat;
{
   char *i, *j, *lastj, *lj, *addset();
   int lastcl;

   j = pat;
   lastj = pat;
   lastcl = 0;
   for (i=from; *i; i++) {
      lj = j;
      if (*i == ANY)
         j = addset (ANY, j);
      else if (*i == BOL && i == from)
         j = addset (BOL, j);
      else if (*i == EOL && *(i+1)==delim)
         j = addset (EOL, j);
      else if (*i == CCL) {
         int tmp;
         tmp = getccl (i, j);
         i = iret;
         j = jret;
         if (tmp == ERR)
            break;
         }
      else if (*i == CLOSURE && i > from) {
         lj = lastj;
         if (*lj == BOL ?? *lj == EOL ?? *lj == CLOSURE)
            break;
         lastcl = stclos (pat, j, lastj, lastcl);
         j = jret;
         lastj = iret;
         }
      else {
         j = addset (CHR, j);
         j = addset (esc(i), j);
         i = iret;
         }
      lastj = lj;
      }
   if (*i != delim)
      return ERR;
   addset (EOS, j);
   return OK;
}

getccl (i, j)
char *i, *j;
{
   char *addset(), *filset();
   char *jstart;

   if (*++i == NOT) {
      j = addset (NCCL, j);
      i++;
      }
   else
      j = addset (CCL, j);
   jstart = j;
   j = addset (0, j);
   j = filset (CCLEND, i, j);
   i = iret;
   *jstart = j - jstart - 1;
   jret = j;
   if (*i == CCLEND)
      return OK;
   return ERR;
}

stclos (pat, j, lastj, lastcl)
char *pat, *j, *lastj;
{
   char *jp, *addset();

   for (jp=j-1; jp >= lastj; jp--)
      addset (*jp, jp+CLOSIZE);
   j += CLOSIZE;
   jp = lastj;
   lastj = addset (CLOSURE, lastj);
   lastj = addset (0, lastj);
   lastj = addset (lastcl, lastj);
   lastj = addset (0, lastj);
   jret = j;
   iret = lastj;
   return (jp - pat + 1);
}

char *filset (delim, i, j)
char *i, *j;
{
   static char digits[] = "0123456789";
   static char lowalf[] = "abcdefghijklmnopqrstuvwxyz";
   static char upalf[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char *addset();
   unsigned index();

   for (; *i != delim && *i != EOS; i++)
      if (*i == ESCAPE) {
         j = addset (esc(i), j);
         i = iret;
         }
      else if (*i != DASH)
         j = addset (*i, j);
      else if (index (digits, *(j-1)) > 0) {
         dodash (digits, i, j);
         i = iret;
         j = jret;
         }
      else if (index (lowalf, *(j-1)) > 0) {
         dodash (lowalf, i, j);
         i = iret;
         j = jret;
         }
      else if (index (upalf, *(j-1)) > 0) {
         dodash (upalf, i, j);
         i = iret;
         j = jret;
         }
      else
         j = addset (DASH, j);
      iret = i;
      return j;
}

esc(i)
char *i;
{
   iret = i;
   if (*i != ESCAPE)
      return *i;
   if (*(i+1) == EOS)
      return ESCAPE;
   iret = ++i;
   if (*i == 'n')
      return '\n';
   return *i;
}

dodash (valid, i, j)
char *valid, *i, *j;
{
   unsigned k, limit, index();

   i++;
   j--;
   limit = index (valid, esc(i));
   for (k = index(valid, *j); k <= limit; k++)
      j = addset (valid[k-1], j);
   jret = j;
}

char *addset (c, j)
char *j;
{
   *j++ = c;
   return j;
}

unsigned index (str, c)
char *str;
{
   char *i;

   for (i = str; *i; i++)
      if (*i == c)
         return (i - str + 1);
   return 0;
}

eturn ESCAPE;
   iret = ++i;
   if (*i == 'n')
      return '\n';
   retu