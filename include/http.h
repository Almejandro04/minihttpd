#ifndef HTTP_H
#define HTTP_H

#define MAX_METODO 8
#define MAX_RUTA 256
#define MAX_VERSION 16
#define MAX_CONEXION 32

#define HTTP_OK 200
#define HTTP_BAD_REQUEST 400
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404

#define HTTP_METHOD_NOT_ALLOWED 405

#define HTTP_INTERNAL_SERVER_ERROR 500




typedef struct {

    char metodo[MAX_METODO];
    char ruta[MAX_RUTA];
    char version[MAX_VERSION];
    char conexion[MAX_CONEXION];

} SolicitudHTTP;  

int parsing_solicitud_http(const char *buffer, SolicitudHTTP *solicitud);

int validar_solicitud_http(const SolicitudHTTP *solicitud);

const char *obtener_mensaje_estado(int codigo_estado);

int construir_encabezado_http(
    int codigo_estado,
    const char *tipo_contenido,
    long tamaño_contenido,
    const char *conexion,
    char *buffer,
    int tamaño_buffer
);

int respuesta_error(
    int codigo_estado,
    char *buffer,
    int tamaño_buffer

);


#endif 
