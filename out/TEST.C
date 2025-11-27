 /* test */
#define NAME "Spinnaker Software,"
#define ADDRESS "One Kendall Square,"
#define CITY "Cambridge, MA  02139"
#define COUNTRY "USA"
#define TELE "(617) 494-1200"
main()
{
  starbar();  
  printf("%s\n", NAME);
  printf("%s\n", ADDRESS);
  printf("%s\n", CITY);
  printf("%s\n", COUNTRY);
  printf("%s\n", TELE);
  starbar();
}
/* starbar() function */
#include <stdio.h>
#define LIMIT 39
starbar()
{
  int count;

  for ( count = 1; count <= LIMIT; count++)
      putchar('*');
  putchar('\n');
}
