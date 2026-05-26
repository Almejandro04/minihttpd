#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "http.h"

int parsing_solicitud_http(const char *buffer, SolicitudHTTP *solicitud){

    if (buffer == NULL || solicitud == NULL){

        return -1;

    }

    memset(solicitud, 0, sizeof(SolicitudHTTP));

    int resultado = sscanf(
        buffer,
        "%7s %255s %15s",
        (*solicitud).metodo,
        (*solicitud).ruta,
        (*solicitud).version
    );

    if (resultado != 3){

        return -1;

    }

    snprintf((*solicitud).conexion, sizeof((*solicitud).conexion), "keep-alive");

    const char *linea = strstr(buffer, "\r\n");

    while (linea != NULL){

        linea += 2;

        if (linea[0] == '\r' && linea[1] == '\n'){

            break;

        }

        if (strncasecmp(linea, "Connection:", 11) == 0){

            const char *valor = linea + 11;

            while (*valor == ' ' || *valor == '\t'){

                valor++;

            }

            sscanf(valor, "%31s", (*solicitud).conexion);

        }

        linea = strstr(linea, "\r\n");

    }

    return 0;

}

int validar_solicitud_http(const SolicitudHTTP *solicitud) {

    if (solicitud == NULL || strcmp((*solicitud).version, "HTTP/1.1") != 0 || (*solicitud).ruta[0] != '/'){

        return HTTP_BAD_REQUEST;

    }

    if(strcmp((*solicitud).metodo, "GET") != 0){

        return HTTP_METHOD_NOT_ALLOWED;

    }

    return HTTP_OK;

}

const char *obtener_mensaje_estado(int codigo_estado){

    if(codigo_estado == HTTP_OK){

        return "OK";

    }

    if(codigo_estado == HTTP_BAD_REQUEST){

        return "Bad Request";

    }

    if(codigo_estado == HTTP_FORBIDDEN){

        return "Forbidden";

    }

    if(codigo_estado == HTTP_NOT_FOUND){

        return "Not Found";

    }

    if(codigo_estado == HTTP_METHOD_NOT_ALLOWED){

        return "Method Not Allowed";

    }

    if(codigo_estado == HTTP_INTERNAL_SERVER_ERROR){

        return "Internal Server Error";

    }
    
    return "Internal Server Error";

}

int construir_encabezado_http(
    int codigo_estado,
    const char *tipo_contenido,
    long tamano_contenido,
    const char *conexion,
    char *buffer,
    int tamano_buffer
) {

    if(tipo_contenido == NULL || conexion == NULL || buffer == NULL || tamano_buffer <= 0){

        return -1;

    }

    const char *mensaje = obtener_mensaje_estado(codigo_estado);

    int resultado = snprintf(
        buffer,
        tamano_buffer,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: %s\r\n"
        "\r\n",
        codigo_estado,
        mensaje,
        tipo_contenido,
        tamano_contenido,
        conexion
    );

    if(resultado < 0 || resultado >= tamano_buffer){

        return -1;

    }

    return 0;

}

int respuesta_error(
    int codigo_estado,
    char *buffer,
    int tamano_buffer 
){

    if(buffer == NULL || tamano_buffer <= 0){

        return -1;

    }

    const char *mensaje = obtener_mensaje_estado(codigo_estado);

    char cuerpo[512];

    int resultado_cuerpo = snprintf(
        cuerpo,
        sizeof(cuerpo),
        "<html>"
        "<head><title>%d %s</title></head>"
        "<body><h1>%d %s</h1></body>"
        "</html>",
        codigo_estado,
        mensaje,
        codigo_estado,
        mensaje
    );

    if(resultado_cuerpo < 0 || resultado_cuerpo >= (int)sizeof(cuerpo)){

        return -1;

    }

    int resultado = snprintf(
        buffer,
        tamano_buffer,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        codigo_estado,
        mensaje,
        (long)strlen(cuerpo),
        cuerpo
    );

    if(resultado < 0 || resultado >= tamano_buffer){

        return -1;

    }

    
    return 0;

}