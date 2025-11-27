
struct direct {
          char type;         /* file type */
          char track;        /* track of link */
          char sector;       /* sector of link */
          char name[16];     /* file name */
          char sstrk;        /* relative file side sector track */
          char sssec;        /*    "      "   side sector sector */
          char recsize;      /*    "      "   record size */
          char unused[6];
          unsigned nblocks;  /* file size */
       };

struct direct *readdir();

ruct direct {
          char type;         /* file type */
          char track;        /* track of link */
          char sector;       /* sector of link */
          char name[16];     /* file name */
          char sstrk;        /* relative file si