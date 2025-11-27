#include <stdio.h>

#define  NAMESIZE  20
#define  ERR        1
#define  MAXLINE   81
#define  MARGIN1    3
#define  MARGIN2    3
#define  PAGELEN   66
#define  BOTTOM    63

main (argc, argv)
unsigned argc;
char **argv;
{
   FILE fin;
 
   if (argc == 1)
      fprint ("stdin", stdin);
   else
      while (--argc) {
         if ((fin = fopen(*++argv, "r")) == NULL ?? ferror()) {
            printf ("cant't open %s\n", *argv);
            exit();
         }
         fprint (*argv, fin);
         fclose (fin);
      }
}

fprint (name, fin)
char *name;
FILE fin;
{
   char line[MAXLINE];
   unsigned pageno, lineno;

   pageno = 0;
   lineno = 0;
   while (fgets (line, MAXLINE, fin) != NULL) {
      if (lineno == 0) {
         skip (MARGIN1);
         pageno++;
         head (name, pageno);
         skip (MARGIN2);
         lineno = MARGIN1 + MARGIN2 + 1;
      }
      fputs (line, stdout);
      lineno++;
      if (lineno >= BOTTOM) {
         skip (PAGELEN - lineno);
         lineno = 0;
      }
   }
   if (lineno > 0)
      skip (PAGELEN - lineno);
}

skip (n)
{
   while (n--)
      putchar ('\n');
}

head (name, pageno)
char *name;
{
   printf ("%-30sPage %d\n", name, pageno);
}
 NULL) {
      if (lineno == 0) {
         skip (MARGIN1);
         pagen