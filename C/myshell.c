#include "built_ins.h"
#include "sockets.h"

int main(int argc, char* argv[])
{
  printf("|---|-------------------------------------------|---|\n");
  printf("|---|----------       myshell         ----------|---|\n");
  printf("|---|-------------------------------------------|---|\n\n");

  int rw;
  int sockfd = -1;
  int newfd  = -1;

  char buffer[STR_LEN];
  char **args;

  newfd = start_listening(sockfd);//, newfd);

  //Recibe los comandos del cliente, en caso de recibir un 'quit',
  //cierra la conexion y espera una conexion nueva
  while(1)
  {
    rw = recv_cmd(newfd, buffer);
    check_error((int) rw);

    if(!strcmp(buffer,"quit"))
      {
        send_cmd(newfd,"TERMINADO. ESCUCHANDO DE NUEVO\n");
        newfd = start_listening(sockfd);
        check_error(newfd);
      }
    else
    {
      args = parsear(buffer);
      ejecutar(args);
      send_cmd(newfd,"COMANDO EJECUTADO.\n");
    }
  }

  return EXIT_SUCCESS;
}
