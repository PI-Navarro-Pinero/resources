/**
 * @file sockets.c
 * @brief
 * implementación de las funciones de 'sockets.h'.
 *
 * @author Tomás Santiago Piñero
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "sockets.h"
#include "built_ins.h"

/**
 * @brief
 * Función encargada de checkear el valor de operaciones que devuelvan -1 cuando
 * ocurre un error.
 *
 * @param err Número.
 */
void check_error(int err)
{
  if(err == -1)
  {
    perror("error");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief
 * Escribe en el socket deseado un mensaje.
 * @param  sockfd Socket en el que se desea escribir el mensaje.
 * @param  cmd    Comando enviado por el usuario.
 * @return        Si el write fue exitoso.
 */
ssize_t send_cmd(int sockfd, void *cmd)
{
  char buf[STR_LEN];
  strcpy(buf,(char*) cmd);
  ssize_t s;

  s = send(sockfd, buf, strlen(buf), 0);

  return s;
}

/**
 * @brief
 * Lee lo que se encuentra en el socket.
 * @param  sockfd Socket del que se desea leer el mensaje.
 * @param  cmd    Comando enviado por el usuario.
 * @return        Si el read fue exitoso.
 */
ssize_t recv_cmd(int sockfd, void *cmd)
{
  char buf[STR_LEN];
  ssize_t r;
  memset(buf, '\0', STR_LEN);

  r = recv(sockfd, buf, STR_LEN-1, 0);
  buf[strlen(buf)-1] = '\0';
  strcpy((char *) cmd, buf);

  return r;
}

/**
 * @brief
 * Función encargada de crear el socket del servidor en el puerto pedido.
 * @param  port Puerto al que se desea conectar.
 * @return      File descriptor del socket creado.
 */
int create_svsocket(char *ip, uint16_t port)
{
  int sockfd;
  struct sockaddr_in sv_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd == -1)
    {
      perror("create socket");
      exit(EXIT_FAILURE);
    }

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

  memset((char *) &sv_addr, 0, sizeof(sv_addr));

  sv_addr.sin_family = AF_INET;
  sv_addr.sin_addr.s_addr = inet_addr(ip);
  sv_addr.sin_port = htons(port);

  int b = bind(sockfd, (struct sockaddr *) &sv_addr, sizeof(sv_addr));

  if(b == -1)
    {
      perror("bind");
      exit(EXIT_FAILURE);
    }
  return sockfd;
}

/**
 * @brief
 * Función encargada de crear el socket y escuchar por posibles conexiones
 * al mismo.
 *
 * @param  sockfd File descriptor del socket.
 * @param  newfd  File descriptor del cliente.
 * @return        File descriptor cliente-servidor.
 */
int start_listening(int sockfd)//, int newfd)
{
  sockfd = create_svsocket(IP_ADDR, PUERTO);

  struct sockaddr_in cl_addr;
  uint cl_len;
  char cl_ip[STR_LEN];

  printf("Esuchando en puerto %d...\n", PUERTO);

  listen(sockfd, 1);
  cl_len = sizeof(cl_addr);

  int newfd = accept(sockfd, (struct sockaddr *) &cl_addr, &cl_len);
  check_error(newfd);

  inet_ntop(AF_INET, &(cl_addr.sin_addr), cl_ip, INET_ADDRSTRLEN);
  printf("Conexión aceptada a %s\n", cl_ip);

  close(sockfd);
  return newfd;
}
