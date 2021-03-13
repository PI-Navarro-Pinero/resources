#include "IO_redirection.h"

/**
 * @brief
 * Cambia la dirección de entrada de la consola al archivo pasado
 * como argumento.
 * @param file_name archivo del cual se van a leer los comandos.
 */
void input_redirection(char file_name[])
{
  int in, flags, permisos;

  in = 0;
  flags = O_RDONLY;
  permisos = S_IWUSR|S_IRUSR;

  close(STDIN_FILENO);

  in = open(file_name,flags,permisos);

  if (in < 0)
  {
    perror("Entrada");
    exit(EXIT_FAILURE);
  }

  int copy = dup2(in,STDIN_FILENO);

  if (copy < 0)
  {
    perror("dup2 entrada");
    exit(EXIT_FAILURE);
  }

  close(in);
}

/**
 * @brief
 * Cambia la dirección de salida de la consola al archivo pasado
 * como argumento.
 * Flags:
 *  - O_WRONLY: solo para escritura.
 *  - O_CREAT: crea el archivo si éste no existe.
 *  - O_TRUNC: abre el archivo existente para lectura/escritura.
 * Permisos:
 *  - S_IWUSR: permiso de lectura.
 *  - S_IRUSR: permiso de escritura.
 *
 * @param file_name archivo al cual se le va a escribir el output.
 */
void output_redirection(char file_name[])
{
  int out, flags, permisos;

  flags = O_WRONLY|O_CREAT|O_TRUNC;
  permisos = S_IWUSR|S_IRUSR;

  out = open(file_name, flags, permisos);

  if (out < 0)
  {
    perror("Salida");
    exit(EXIT_FAILURE);
  }

  close(STDOUT_FILENO);

  int copy = dup2(out,STDOUT_FILENO);

  if (copy < 0)
  {
    perror("dup2 salida");
    exit(EXIT_FAILURE);
  }

  close(out);
}

/**
 * @brief
 * Realiza la verificación sobre el direccionamiento. En caso de encontrar
 * un símnolo de entrada (<) y/o salida (>), guarda el nombre del archivo al
 * cual se debe redirigir. Si es un archivo de entrada, se separan los
 * comandos en tokens para ejecutarlos.
 * @param args      argumentos
 * @param file_name archivo de redirección
 */
int check_redirection(char **args, char file_name[])
{
  int i = 0;

  while (args[i] != NULL)
  {
    if(!strcmp(args[i],"<"))
    {
      strcpy(file_name,args[i+1]);
      borrar_simbolo(args,i);
      return I_REDIRECTION;
    }
    else if(!strcmp(args[i],">"))
    {
      strcpy(file_name,args[i+1]);
      args[i] = NULL;
      args[i+1] = NULL;
      return O_REDIRECTION;
    }
    i++;
  }
  return NO_REDIRECTION;
}

/**
 * @brief
 * Elimina el símbolo que se encuentra en la posición deseada de la cadena, En
 * este caso elimina los símbolos de entrada (<) y/o salida (>).
 * @param args     argumentos
 * @param posicion posición en la que se encuentra el símbolo a eliminar.
 */
void borrar_simbolo(char **args, int posicion)
{
  while(args[posicion] != NULL)
  {
    args[posicion] = args[posicion+1];
    posicion++;
  }
}

/**
 * @brief
 * Redirecciona STDIN; STDOUT y STDERR al socket que se le pase como
 * argumento.
 * @param   socket Socket al que se desea redirigir las E/S.
 */
int socket_reditection(int socket)
{
  int stdin_state, stdout_state, stderr_state;

  close(0);
  close(1);
  close(2);

  stdin_state  = dup(socket);
  stdout_state = dup(socket);
  stderr_state = dup(socket);

  if(stdin_state != 0 || stdout_state != 1 || stderr_state != 2)
  {
    perror("ERROR_socket_dup");
    return -1;
  }

  return 0;
}
