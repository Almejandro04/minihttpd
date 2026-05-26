#ifndef SERVER_H
#define SERVER_H

int crear_socket_servidor(int puerto);

int aceptar_clientes(int servidor_fd);

int atender_cliente(int cliente_fd);

int ejecutar_servidor_epoll(int servidor_fd);

#endif