#ifndef BUILT_INS_H
#define BUILT_INS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define STR_LEN       1024
#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM " \t\r\n\a"

char **parsear(char *linea);
int  run(char **args, int socket);
int  ejecutar(char **args, int socket);

int cd_com(char **args);
int clr_com(char **args);
int echo_com(char **args);

#endif
