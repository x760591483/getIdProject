#!/bin/bash
gcc main.c fileread.c -O getid -L ../lib/ -llogLog -levent -levent_core  -levhtp -levent_openssl -levent_pthreads -ljson -lssl -lcrypto -ldl -lpthread -I ../include/ -I ../include/evhtp/
