/**
 * @file sockets.h
 * @brief
 * Header para el uso de los sockets.
 *
 * @author Tomás Santiago Piñero
 */

#include <stdint.h>

#define IP_ADDR "127.0.0.1" /**< Dirección IP del servidor */
#define PUERTO  7777        /**< Puerto para conectar con Java */
#define STR_LEN 1024        /**< Largo de los strings */

/**
 * @brief
 * Escribe en el socket deseado un mensaje.
 * @param  sockfd Socket en el que se desea escribir el mensaje.
 * @param  cmd    Comando enviado por el usuario.
 * @return        Si el write fue exitoso.
 */
ssize_t send_cmd(int sockfd, void *cmd);

/**
 * @brief
 * Lee lo que se encuentra en el socket.
 * @param  sockfd Socket del que se desea leer el mensaje.
 * @param  cmd    Comando enviado por el usuario.
 * @return        Si el read fue exitoso.
 */
ssize_t recv_cmd(int sockfd, void *cmd);

/**
 * @brief
 * Función encargada de crear el socket en el puerto pedido.
 * @param  port Puerto al que se desea conectar.
 s* @return      File descriptor del socket creado.
 */
int create_svsocket(char *ip, uint16_t port);

/**
 * @brief
 * Función encargada de crear el socket y escuchar por posibles conexiones
 * al mismo.
 *
 * @param  sockfd File descriptor del socket.
 * @param  newfd  File descriptor del cliente.
 * @return        File descriptor cliente-servidor.
 */
int start_listening(int sockfd);//, int newfd);

/**
 * @brief
 * Función encargada de checkear el valor de operaciones que devuelvan -1 cuando
 * ocurre un error.
 *
 * @param err Número.
 */
void check_error(int err);
