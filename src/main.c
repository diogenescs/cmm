#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{

  
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <source.cmm>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  exit(EXIT_SUCCESS);
}
