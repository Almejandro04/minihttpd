#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "server.h"
#include "http.h"
#include "files.h"
#include "mime.h"

#define MAX_EVENTOS 64
#define TAMANO_BUFFER 4096


static int enviar_todo(int fd, const void *buffer, long total_bytes){

    const char *datos = (const char *)buffer;
    long enviados = 0;

    while(enviados < total_bytes){

        ssize_t resultado = send(fd, datos + enviados, total_bytes - enviados, 0);

        if(resultado <= 0){

            return -1;

        }

        enviados += resultado;

    }

    return 0;

}


int crear_socket_servidor(int puerto){

    int servidor_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor_fd == -1){

        return -1;

    }

    int opcion = 1;

    if(setsockopt(servidor_fd, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion)) == -1){

        close(servidor_fd);
        return -1;

    }

    struct sockaddr_in direccion;
    
    memset(&direccion, 0, sizeof(direccion));

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);

    if(bind(servidor_fd, (struct sockaddr *)&direccion, sizeof(direccion)) == -1){

        close(servidor_fd);
        return -1;

    }
    
    if(listen(servidor_fd, 10) == -1){

        close(servidor_fd);
        return -1;

    }

    return servidor_fd;

}


int aceptar_clientes(int servidor_fd){

    if (servidor_fd < 0){
        
        return -1;

    }

    struct sockaddr_in direccion_cliente;
    socklen_t tamano_direccion = sizeof(direccion_cliente);

    int cliente_fd = accept(
        servidor_fd,
        (struct sockaddr *)&direccion_cliente,
        &tamano_direccion
    );
    
    if(cliente_fd == -1){

        return -1;

    }

    return cliente_fd;

}


int atender_cliente(int cliente_fd){

    if(cliente_fd < 0){

        return 1;

    }

    char buffer[TAMANO_BUFFER];
    char respuesta[TAMANO_BUFFER];

    int bytes_recibidos = recv(cliente_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_recibidos <= 0 ){
        
        return 1;

    }

    buffer[bytes_recibidos] = '\0';

    SolicitudHTTP solicitud;

    if (parsing_solicitud_http(buffer, &solicitud) != 0){
        
        respuesta_error(HTTP_BAD_REQUEST, respuesta, sizeof(respuesta));
        enviar_todo(cliente_fd, respuesta, strlen(respuesta));
        return 1;

    }

    int codigo = validar_solicitud_http(&solicitud);

    if (codigo != HTTP_OK){
        
        respuesta_error(codigo, respuesta, sizeof(respuesta));
        enviar_todo(cliente_fd, respuesta, strlen(respuesta));
        return 1;

    }

    char ruta_archivo[512];
    long tamano_archivo;
    const char *tipo_mime;

    int resultado_ruta = construir_ruta_archivo(solicitud.ruta, ruta_archivo, sizeof(ruta_archivo));

    if (resultado_ruta != FILES_OK){

        if(resultado_ruta == FILES_PROHIBIDO){

            respuesta_error(HTTP_FORBIDDEN, respuesta, sizeof(respuesta));

        } else if(resultado_ruta == FILES_NO_ENCONTRADO){

            respuesta_error(HTTP_NOT_FOUND, respuesta, sizeof(respuesta));

        } else {

            respuesta_error(HTTP_INTERNAL_SERVER_ERROR, respuesta, sizeof(respuesta));

        }

        enviar_todo(cliente_fd, respuesta, strlen(respuesta));
        return 1;

    }

    int resultado_tamano = obtener_tamano_archivo(ruta_archivo, &tamano_archivo);

    if (resultado_tamano != FILES_OK){

        if(resultado_tamano == FILES_PROHIBIDO){

            respuesta_error(HTTP_FORBIDDEN, respuesta, sizeof(respuesta));

        } else if(resultado_tamano == FILES_NO_ENCONTRADO){

            respuesta_error(HTTP_NOT_FOUND, respuesta, sizeof(respuesta));

        } else {

            respuesta_error(HTTP_INTERNAL_SERVER_ERROR, respuesta, sizeof(respuesta));

        }

        enviar_todo(cliente_fd, respuesta, strlen(respuesta));
        return 1;

    }

    tipo_mime = obtener_tipo_MIME(ruta_archivo);

    if (tipo_mime == NULL){

        respuesta_error(HTTP_INTERNAL_SERVER_ERROR, respuesta, sizeof(respuesta));
        enviar_todo(cliente_fd, respuesta, strlen(respuesta));
        return 1;

    }

    char *contenido_archivo = NULL;
    long bytes_leidos = 0;

    if(tamano_archivo > 0){

        contenido_archivo = malloc(tamano_archivo);

        if (contenido_archivo == NULL){

            respuesta_error(HTTP_INTERNAL_SERVER_ERROR, respuesta, sizeof(respuesta));
            enviar_todo(cliente_fd, respuesta, strlen(respuesta));
            return 1;

        }

        if (leer_archivo(ruta_archivo, contenido_archivo, tamano_archivo, &bytes_leidos) != FILES_OK){

            free(contenido_archivo);
            respuesta_error(HTTP_INTERNAL_SERVER_ERROR, respuesta, sizeof(respuesta));
            enviar_todo(cliente_fd, respuesta, strlen(respuesta));
            return 1;

        }

    }

    const char *conexion_respuesta = "keep-alive";
    int cerrar_conexion = 0;

    if(strcasecmp(solicitud.conexion, "close") == 0){

        conexion_respuesta = "close";
        cerrar_conexion = 1;

    }

    if (construir_encabezado_http(
        HTTP_OK,
        tipo_mime,
        bytes_leidos,
        conexion_respuesta,
        respuesta,
        sizeof(respuesta)
    ) != 0){

        free(contenido_archivo);
        respuesta_error(HTTP_INTERNAL_SERVER_ERROR, respuesta, sizeof(respuesta));
        enviar_todo(cliente_fd, respuesta, strlen(respuesta));
        return 1;

    }

    if(enviar_todo(cliente_fd, respuesta, strlen(respuesta)) != 0){

        free(contenido_archivo);
        return 1;

    }

    if(bytes_leidos > 0){

        if(enviar_todo(cliente_fd, contenido_archivo, bytes_leidos) != 0){

            free(contenido_archivo);
            return 1;

        }

    }

    free(contenido_archivo);

    return cerrar_conexion;

}


int ejecutar_servidor_epoll(int servidor_fd){

    int epoll_fd = epoll_create1(0);

    if(epoll_fd == -1){

        return -1;

    }

    struct epoll_event evento;
    struct epoll_event eventos[MAX_EVENTOS];

    evento.events = EPOLLIN;
    evento.data.fd = servidor_fd;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servidor_fd, &evento) == -1){

        close(epoll_fd);
        return -1;

    }

    while(1){

        int cantidad_eventos = epoll_wait(epoll_fd, eventos, MAX_EVENTOS, -1);

        if(cantidad_eventos == -1){

            continue;

        }

        for(int i = 0; i < cantidad_eventos; i++){

            int fd_actual = eventos[i].data.fd;

            if(fd_actual == servidor_fd){

                int cliente_fd = aceptar_clientes(servidor_fd);

                if(cliente_fd == -1){

                    continue;

                }

                struct epoll_event evento_cliente;

                evento_cliente.events = EPOLLIN;
                evento_cliente.data.fd = cliente_fd;

                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cliente_fd, &evento_cliente) == -1){

                    close(cliente_fd);
                    continue;

                }

                printf("Cliente conectado\n");

            } else {

                int cerrar = atender_cliente(fd_actual);

                if(cerrar){

                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_actual, NULL);
                    close(fd_actual);

                }

            }

        }

    }

    close(epoll_fd);

    return 0;

}