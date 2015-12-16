#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
   char c;
   for (;;) {
      c = rand()%128;
      printf("%c", c);
   }
   return 0;
}
