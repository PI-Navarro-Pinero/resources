#include "built_ins.h"

#define COLOR_PROMPT "\x1b[36;1m"
#define COLOR_RESET  "\x1b[0m"

int main(int argc, char* argv[])
{
  printf("|---|-------------------------------------------|---|\n");
  printf("|---|----------       myshell         ----------|---|\n");
  printf("|---|-------------------------------------------|---|\n\n");

  char path[1024];
  char *linea;
  char **args;
  int estado;

  // char *user = getenv("USER");
  // if(user == NULL) user = "usuario";
  //
  // char host[100];
  // gethostname(host,100);

  do
  {
    if(getcwd(path, sizeof(path)) != NULL)
      printf(COLOR_PROMPT "\n%s" COLOR_RESET "\n", path);
    else
      printf("\nError al obtener directorio actual.\n");

    printf("|> ");
    // printf("%s@%s:-", user, host);
    // showPath();

    linea = leer_linea();
    args = parsear(linea);
    estado = ejecutar(args);

    free(linea);
    free(args);
  } while(estado);

  return EXIT_SUCCESS;
}
