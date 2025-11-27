#include <stdio.h>

#define LINES  500
#define MAXLEN  81
#define TRUE     1
#define FALSE    0


main (argc, argv)

unsigned argc;
char **argv;

{
   static char *lineptr?LINES?;
   int nlines;
   int strcomp(), numcmp();
   int numeric = FALSE;
   char *s;

   s = *++argv;
   if (argc>1 && *s++ == '-' && *s == 'n')
      numeric = TRUE;
   if ((nlines = readlines(lineptr, LINES)) >= 0) {
      if (numeric)
         qsort(lineptr, nlines, sizeof(char *), numcmp);
      else
         qsort(lineptr, nlines, sizeof(char *), strcomp);
      writelines (lineptr, nlines);
   } else
      printf("input too big to sort\n");
}

readlines(lineptr, maxlines)
char *lineptr??;
unsigned maxlines;

   unsigned len, nlines;
   char *p, *malloc(), line?MAXLEN?;

   nlines = 0;
   while ((len = getline(line, MAXLEN))) {
      if (nlines >= maxlines)
         return (-1);
      if ((p = malloc(len)) == NULL)
         return (-1);
      strcpy(p, line);
      lineptr?nlines++? = p;
   }
   return (nlines);
}

getline (s, lim)
char *s;
unsigned lim;
{
   if (fgets(s, lim, stdin) == NULL)
      return (0);
   return (strlen(s)+1);
}

strcomp (s1, s2)
char **s1, **s2;
{
   return (strcmp(*s1, *s2));
}

numcmp (s1, s2)
char **s1, **s2;
{
   double atof(), v1, v2;

   v1 = atof(*s1);
   v2 = atof(*s2);
   if (v1 < v2)
      return(-1);
   if (v1 > v2)
      return(1);
   return(0);
}

writelines (lineptr, nlines)
char *lineptr??;
unsigned nlines;
{
   while (nlines--)
      printf ("%s", *lineptr++);
}
ines);
}

getline (
