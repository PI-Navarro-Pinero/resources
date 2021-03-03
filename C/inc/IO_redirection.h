#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define O_REDIRECTION   0
#define I_REDIRECTION   1
#define NO_REDIRECTION  2

void input_redirection(char file_name[]);
void output_redirection(char file_name[]);
void borrar_simbolo(char **args, int index);
int  check_redirection(char **args, char file_name[]);
