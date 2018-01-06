
CC = gcc
EXE = idget

LIBS = -L../lib -llogLog -levent -levent_core -levhtp -levent_openssl  -levent_pthreads -ljson -lssl -lcrypto -ldl -lpthread
INC = -I../include/ -I../include/evhtp

OBJ = fileread.o main.o 

CFLAGS = -Wall $(INC)

all: $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIBS) $(CFLAGS)

%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean :
	rm -f *.o $(EXE)



