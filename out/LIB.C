/*
    Create and mantain library files.
    Library file names must end
    with  ".l"
*/

#include <stdio.h>

#define  MAXLIB  200
#define  MAXID    20
#define  MAXLINE  81

char ids[MAXLIB][MAXID], files[MAXLIB][MAXID];
char size;
char inbuf[MAXLINE];

main()
{
    size = 0;
    for (;;) {
        clrscrn();
        printf ("\n\n\n\n\n     Load library\n");
        printf ("     Save library\n");
        printf ("     Clear library\n");
        printf ("     Display library\n");
        printf ("     Add modules to library\n");
        printf ("     Remove modules from library\n");
        printf ("     Quit\n\n     Enter Choice: ");
        gets (inbuf);
        clrscrn();
        switch (*inbuf) {
            case 'l':
            case 'L':
                      load();
                      break;
            case 's':
            case 'S':
                      save();
                      break;
            case 'd':
            case 'D':
                      display();
                      break;
            case 'c':
            case 'C':
                      size = 0;
                      break;
            case 'a':
            case 'A':
                      add();
                      break;
            case 'r':
            case 'R':
                      remove();
                      break;
            case 'q':
            case 'Q':
                      putchar ('\n');
                      exit();
        }
    }
    putchar ('\n');
}


clrscrn()
{
    putchar ('?');
}


FILE openlib (mode)
char *mode;
{
    FILE f;

    printf ("\n\nlibrary file name: ");
    gets (inbuf);
    f = fopen (inbuf, mode);
    if (f == NULL ?? ferror()) {
        f = NULL;
        printf ("\ncan't open %s\n", inbuf);
        wait();
    }
    return f;
}


load()
{
    FILE f;
    unsigned i;

    if ((f = openlib ("r")) == NULL)
        return;
    size = getc(f);
    for (i = 0; i < size; i++) {
        getid (ids[i], f);
        getid (files[i], f);
    }
    fclose(f);
}


getid (s, f)
char *s;
FILE f;
{
    while (*s++ = getc(f))
       ;
}


save()
{
    FILE f;
    unsigned i;

    if ((f = openlib ("w")) == NULL)
        return;
    putc (size, f);
    for (i = 0; i < size; i++)
        fprintf (f, "%s%c%s%c", ids[i], 0, files[i], 0);
    fclose(f);
}


wait()
{
    printf ("\nhit RETURN to continue");
    while (getch() != '\n')
        ;
}


display()
{
    unsigned i, line;

    line = 1;
    for (i = 0; i < size; i++) {
        if (line == 1) {
            clrscrn();
            printf ("%-20s%-19s\n\n", "IDENTIFIER", "FILE");
            line = 3;
        }
        printf ("%-20s%-19s\n", ids[i], files[i]);
        line++;
        if (line == 24) {
            wait();
            line = 1;
        }
    }
    wait();
}


getch()
{
    char a, x, y;

    do
        sys (0xffe4, &a, &x, &y);
    while (a == 0);
    return a;
}


add()
{
    unsigned n;
    FILE f;

    printf ("\nhit RETURN when done\n\n");
    for (;;) {
        printf ("object file name: ");
        gets (inbuf);
        if (!isalpha(*inbuf))
            return;
        f = fopen (inbuf, "r");
        if (f == NULL ?? ferror())
            printf ("can't open %s\n", inbuf);
        else {
            n = getw(f);
            while (n--)
                getc(f);
            n = getw(f);
            while (n--)
                getw(f);
            n = getw(f);
            while (n-- && size < MAXLIB) {
                getid (ids[size], f);
                if (isalpha(*ids[size]) && isprint(ids[size][1]))
                    strcpy (files[size++], inbuf);
                getc(f);
                getw(f);
            }        
            fclose (f);
            if (size == MAXLIB) {
                printf ("\nlibrary is full\n");
                wait();
                return;
            }
        }
    }
}


remove()
{
    unsigned i, j;

    printf ("\nhit RETURN when done\n");
    for (;;) {
        printf ("\nfile to delete: ");
        gets (inbuf);
        if (!isalpha(*inbuf))
            return;
        for (i = 0; i < size && strcmp(files[i], inbuf) != 0; i++)
            ;
        if (i == size)
            printf ("file not found\n");
        else {
            while (i < size && strcmp (files[i], inbuf) == 0) {
                for (j = i; j+1 < size; j++) {
                    strcpy (files[j], files[j+1]);
                    strcpy (ids[j], ids[j+1]);
                }
                size--;
            }
        }
    }
}
 gets (inbuf);
        if (!isalpha(*inbuf))
            return;
  