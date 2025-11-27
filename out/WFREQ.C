#include <stdio.h>

#define  MAXWORD  20
#define  LETTER  'a'


struct tnode {
          char *word;
          int count;
          struct tnode *left;
          struct tnode *right;
       };


main()
{
   struct tnode *root, *tree();
   static char word[MAXWORD];
   int t;

   root = NULL;
   while ((t=getword(word, MAXWORD)) != EOF)
      if (t == LETTER)
         root = tree(root, word);
   treeprint(root);
}


struct tnode *tree(p, w)
struct tnode *p;
char *w;
{
   char *strsave(), *malloc();
   int  cond;

   if (p == NULL) {
      p = (struct tnode *)
          malloc (sizeof (struct tnode));
      if (p == NULL)
         full();
      p->word = strsave(w);
      p->count = 1;
      p->left = NULL;
      p->right = NULL;
   } else if ((cond=strcmp(w, p->word)) == 0)
      p->count++;
   else if (cond < 0)
      p->left = tree(p->left, w);
   else
      p->right = tree(p->right, w);
   return(p);
}


treeprint (p)
struct tnode *p;
{
   if (p != NULL) {
      treeprint(p->left);
      printf ("%4d %s\n", p->count, p->word);
      treeprint(p->right);
   }
}


getword(w, lim)
char *w;
{
   int c;

   if (!isalpha((c=getchar(), *w++=c))) {
      *w = '\0';
      return (c);
   } 
   while (--lim)
      if (!isalnum(*w++=getchar()))
         break;
   *(w-1) = '\0';
   return (LETTER);
}


char *strsave(s)
char *s;
{
   char *p, *malloc();

   if ((p=malloc(strlen(s)+1)) == NULL)
      full();
   strcpy (p, s);
   return(p);
}


full()
{
   printf ("input too big\n");
   exit();
}
     printf ("%4