# Universidad Nacional De Córdoba
## Facultad de Ciencias Exactas, Físicas y Naturales
### Sistemas Operativos I
#### Trabajo Práctico N° 2: myshell
------
#### Piñero, Tomás Santiago - Año 2019
------

#### Objetivos
Con el desarrollo del siguiente Trabajo Práctico (TP), se busca:

 - Utilizar mecanismos de creación de procesos.
 - Utilizar mecanismos de concurrencia e IPC.
 - Diseñar un intérprete de línea de comandos al estilo Bourne shell.

#### Funcionalidades

1. **Comandos internos:** *myshell* soporta 4 comandos internos:

  1.  **cd** *'directorio'*

      Cambia el directorio actual a *'directorio'*, en caso de que exista. Sino, muestra el error.

  2. **clr**

      Limpia la pantalla de la *shell*.

  3. **echo** *'comentario'*

      Muestra *'comentario'* en la pantalla seguido por una línea nueva.

  4. **quit**

      Termina la ejecución de *myshell* cerrando el programa.

2. **Invocación de programas:** Todas los comandos que no sean interpretados como internos son interpretados como la invocación de un programa, que será ejecutado por *myshell*.

3. **Ejecución en segundo plano:** Si se ingresa el símbolo '**&**' (ampersand) al final de la línea de comando, *myshell* ejecutará el comando mediante un proceso hijo, permitiendo la ejecución en segundo plano.

4. **Redirección de Entrada y/o Salida:** *myshell* soporta tanto redirección de entrada como de salida, las cuales se indican en la línea de comandos por medio de los operadores '**<**' y '**>**' respectivamente.\
El comando interno *echo* soporta la redirección de salida, por ejemplo, el siguiente comando:

  ```shell
  echo hola mundo! > output
  ```
imprimirá "hola mundo!" en un archivo llamado *output*.

#### Desarrollo

Se dividió el proyecto en la siguiente estructura:

- **./inc** -- carpeta en la que se encuentra las cabeceras desarrolladas:

  1. *built_ins.h* -- contiene las funciones de loop, parseo y ejecución de comandos.
  2. *IO_redirection.h* -- contiene las funciones que corresponden a la redirección de E/S.
  3. *segundo_plano.h* -- contiene las funciones utilizadas para la ejecución en segundo plano.


- **./obj** -- carpeta en la que se guardan los .o creados por el comiplador.
- **./src** -- carpeta en la que se encuentran las implementaciones de funciones de las cabeceras ubicadas en ./inc
- *batchfile* -- archivo con la lista de comandos a ejecutar automáticamente.
- *myshell.c* -- archivo principal. Se encarga de la ejecución del programa y la verificación del *batchfile*.
- *input* -- archivo utilizado para la redirección de entrada.
- *output* -- archivo utilizado para la redirección de salida.


#### Compilación y ejecución del proyecto

1. Ir al directorio donde se encuentra el proyecto, por ejemplo:

```zsh
 $ cd Documentos/myshell
```

2. Compilarlo con el siguiente comando:

```zsh
  $ make
```

3. Ejecución: *myshell* tiene dos modos de ejecución:

  1. Ejecución manual:

   ```zsh
     $ ./myshell
   ```
    
   Esto mostrará un *prompt* con la ubicación del directorio actual junto con el nombre de usuario y máquina, esperando la introducción de comandos por parte del usuario.

  2. Ejecución automática a través de un **_batchfile_**:

  ```zsh
    $ ./myshell batchfile
  ```
    
   Ejecuta los comandos del arhivo *batchfile* y una vez que éste se termina, *myshell* termina su ejecución.

4. Si se desea eliminar los archivos generados por el *makefile*, simplemente ingresar en consola:

```zsh
  $ make clean
```

Código fuente basado en [lsh de Stephen Brennan](https://brennan.io/2015/01/16/write-a-shell-in-c/)
