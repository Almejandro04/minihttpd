#include <stdio.h>
#include <unistd.h>
#include "server.h"

int main(){

    int puerto = 8080;

    int servidor_fd = crear_socket_servidor(puerto);

    if (servidor_fd == -1){

        printf("Error al crear el servidor\n");
        return 1;

    }

    printf("MiniHTTPd escuchando en el puerto %d\n", puerto);

    if(ejecutar_servidor_epoll(servidor_fd) == -1){

        printf("Error ejecutando el servidor con epoll\n");
        close(servidor_fd);
        return 1;

    }

    close(servidor_fd);

    return 0;

}