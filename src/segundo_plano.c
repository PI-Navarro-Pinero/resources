#include "segundo_plano.h"

int segundo_plano(char **args)
{
  int sp = 0;
  int i = 0;

  while(args[i] != NULL) i++;

  if(strcmp(args[i-1], "&") == 0) sp = 1;

  return sp;
}

void eliminar(char **args)
{
  int i = 0;

  while(args[i] != NULL) i++;

  args[i-1] = NULL;
}
