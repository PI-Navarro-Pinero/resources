#include "built_ins.h"
#include "sockets.h"

// #define TRUE  1
// #define FALSE 0

int check_batchfile(char *argv[]);

int main(int argc, char* argv[])
{
  printf("|---|-------------------------------------------|---|\n");
  printf("|---|---------- SISTEMAS OPERTAIVOS I ----------|---|\n");
  printf("|---|---------- Trabajo Práctico N° 2 ----------|---|\n");
  printf("|---|----------       myshell         ----------|---|\n");
  printf("|---|---------- Tomás Santiago Piñero ----------|---|\n");
  printf("|---|-------------------------------------------|---|\n\n");

  // if(check_batchfile(argv))
  // {
  //   FILE *batchfile = fopen(argv[1], "r");
  //   char linea[100];
  //
  //   while(fgets(linea,100,batchfile))
  //   {
  //     char *user = getenv("USER");
  //     if(user == NULL) user = "usuario";
  //
  //     char host[100];
  //     gethostname(host,100);
  //
  //     printf("%s@%s:-", user, host);
  //     showPath();
  //
  //     char **args = parsear(linea);
  //     ejecutar(args);
  //
  //     free(args);
  //   }
  // } else loop();

  int sockfd = -1;
  int newfd = -1;
  char buffer[STR_LEN];

  newfd = start_listening(sockfd, newfd);

  loop_listen(sockfd, newfd, buffer);

  return EXIT_SUCCESS;
}

/**
 * Antes de ejecutar myshell, comprueba si se pasó como argumento un
 * batchfile.
 * @param  argv argumentos
 * @return      True: utiliza un batchfile. False: no utiliza batchfile.
 */
// int check_batchfile(char *argv[])
// {
//   if(argv[1] != NULL) return TRUE;
//   else return FALSE;
// }
