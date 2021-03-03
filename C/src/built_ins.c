#include "built_ins.h"
#include "segundo_plano.h"
#include "IO_redirection.h"
#include "sockets.h"

/**
 * @brief
 * Declaración de las funciones propias de la consola.
 */
char *builtin_str[] = {
  "cd",
  "clr",
  "echo",
  "quit"
};

/**
 * @brief
 * Declaración de las funciones propias de la consola.
 * @param builtin_fun lista de funciones propias.
 */
int (*builtin_fun[])(char **) = {
  &cd_com,
  &clr_com,
  &echo_com,
  &quit_com
};

/**
 * @brief
 * Devuelve la cantidad de funciones propias.
 * @return funciones propias
 */
int builtins()
{
  return sizeof(builtin_str)/sizeof(char *);
}

/**
 * @brief
 * Función proncipal del programa. Muestra el prompt con el directorio
 * actual, el usuario y el nombre de la máquina, y toma los comandos
 * que se escriben.
 */
void loop(char linea[STR_LEN])
{
  char **args;
  int estado;

  // char *user = getenv("USER");
  // if(user == NULL) user = "usuario";
  //
  // char host[100];
  // gethostname(host,100);

  // do
  // {
  args = parsear(linea);
  estado = ejecutar(args);
  // free(linea);
  // free(args);
  // } while(estado);
}


/**
 * @brief
 * Se toma la linea y se la divide en tokens,
 * guardando cada puntero en el buffer. En caso que
 * se necesite reasignar los punteros, se lo hace.
 * @return comandos individuales
 */
char **parsear(char *linea)
{
  int bufsize = TOKEN_BUFSIZE, posicion = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if(!tokens)
  {
    fprintf(stderr, "Error de asignación");
    exit(EXIT_FAILURE);
  }

  token = strtok(linea,TOKEN_DELIM);

  while(token != NULL)
  {
    tokens[posicion] = token;
    posicion++;

    if(posicion >= bufsize)
    {
      bufsize += TOKEN_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));

      if(!tokens)
      {
        fprintf(stderr, "Error de asignación");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL,TOKEN_DELIM);
  }

  tokens[posicion] = NULL;
  return tokens;
}

/**
 * @brief
 * Toma la lista de argumentos y los ejecuta en un proceso hijo si no hay un
 * '&' al final del argumento. En caso de que se deba ejecutar en background
 * el padre espera que termine la ejecución de esos comandos y
 * finalmente vuelve a mostrar el prompt. Si no es en backroung, myshell
 * es quien ejecuta el proceso.
 * @param  args argumentos
 * @return      ejecución exitosa
 */
int run(char **args)
{
  char file_name[256];//, input_file[256], output_file[256];
  pid_t pid;
  int estado, io_redirection;

  int tiene_amp = 0;

  if(segundo_plano(args))
  {
    eliminar(args);
    tiene_amp = 1;
  }

  pid = fork();

  switch (pid)
  {
    case -1:
      perror("EROR_fork");
      break;

    case 0:
      io_redirection = check_redirection(args,file_name);

      switch (io_redirection)
      {
        case I_REDIRECTION:
          freopen(file_name, "r", stdin);
          if(check_redirection(args,file_name) == O_REDIRECTION);
          else break;

        case O_REDIRECTION:
          output_redirection(file_name);
          break;
      }

      execvp(args[0], args);
      perror("ERROR_hijo");
      break;

    default:
      if(tiene_amp) waitpid(pid, &estado, WNOHANG);
      else waitpid(pid, &estado, WUNTRACED);
      break;
  }

  return 1;
}

/**
 * @brief
 * Esta función ejecuta un built-in o un proceso.
 * Si el comando es un built-in, lo ejecuta, sino utiliza la función
 * 'run' para ejecutarlos.
 * Realiza la comprobación de redirección de salida o entrada.
 * @param  args       argumentos
 * @return built-in   comando built-in
 * @return            proceso
 */
int ejecutar(char **args)
{
  if(args[0] == NULL) return 1;

  for (int i = 0; i < builtins(); i++)
  {
    if(strcmp(args[0], builtin_str[i]) == 0)
    return(*builtin_fun[i])(args);
  }

  return run(args);
}

/**
 * @brief
 * Cambia el directorio actual al directorio especificado en el comando.
 * En caso de error muestra un mensaje.
 * @param  args argumentos
 * @return      cambio de dirección exitoso
 */
int cd_com(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Error: inserte un directorio\n");
  } else {
    if (chdir(args[1]) != 0) {
      fprintf(stderr, "Error: El directorio no existe.\n");
    }
  }
  return 1;
}

/**
 * @brief
 * Limpia la consola y posiciona el cursor al principio de la linea.
 * @param  args argumentos
 * @return      limpieza exitosa
 */
int clr_com(char **args)
{
  fprintf(stdout, "\33[2J");
  fprintf(stdout, "\33[1;1H");
  return 1;
}

/**
 * @brief
 * Muestra el contenido escrito luego de 'echo' seguido de una
 * nueva línea.
 * @param  args argumentos
 * @return      echo exitoso
 */
int echo_com(char **args)
{
  //printf("Este es el echo de myshellobama");
  char file_name[256];
  FILE *output;
  int i = 1;

  if(check_redirection(args,file_name) == O_REDIRECTION)
  {
    output = fopen(file_name,"w");
    while(args[i] != NULL)
    {
      fprintf(output, "%s ", args[i]);
      i++;
    }
    fprintf(output, "%s", "\n");
    fclose(output);
  } else
  {
    while(args[i] != NULL)
    {
      printf("%s ", args[i]);
      i++;
    }
    printf("\n");
  }

  return 1;
}

/**
 * @brief
 * Termina la ejecución de la shell devolviendo un 0.
 * @param  args argumentos
 * @return 0    terminar la ejecución de myshell
 */
int quit_com(char **args)
{
  return 0;
}
