#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
  printf("Ingrese un valor: ");
  fflush(stdout);

  char buffer[256];
  fgets(buffer, 256, stdin);

  buffer[strlen(buffer)-1] = '\0';

  printf("Usted ingrsó: '%s'\n", buffer);

  return EXIT_SUCCESS;
}
