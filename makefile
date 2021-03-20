# Compilador con sus flags
CC=gcc
CFLAGS= -g -Wall -pedantic -I$(IDIR)

# Directorios a utilizar
IDIR =./inc
ODIR =./obj
SRC  =./src

# Objetos
_OBJ = built_ins.o segundo_plano.o IO_redirection.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Compila las dependencias
$(ODIR)/%.o: $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compila el programa principal
myshell: myshell.c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Elimina los objetos creados
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ myshell
