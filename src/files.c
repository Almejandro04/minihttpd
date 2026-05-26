#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include "files.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define DIRECTORIO_WEB "www"

int construir_ruta_archivo(const char *ruta_http, char *ruta_final, int tamano_ruta_final) {
    
    if (ruta_http == NULL || ruta_final == NULL || tamano_ruta_final <=0) {

        return FILES_ERROR;

    }

    if(ruta_http[0] != '/'){

        return FILES_ERROR;

    }

    if(strstr(ruta_http, "..") != NULL){

        return FILES_PROHIBIDO;

    }

    const char *ruta_solicitada = ruta_http;

    if(strcmp(ruta_http, "/") == 0) {
        
        ruta_solicitada = "/index.html";

    }

    char raiz_real[PATH_MAX];
    char ruta_temporal[PATH_MAX];
    char archivo_real[PATH_MAX];

    if(realpath(DIRECTORIO_WEB, raiz_real) == NULL){

        return FILES_ERROR;

    }

    int resultado = snprintf(
        ruta_temporal,
        sizeof(ruta_temporal),
        "%s%s",
        DIRECTORIO_WEB,
        ruta_solicitada
    );

    if (resultado < 0 || resultado >= (int)sizeof(ruta_temporal)) {

        return FILES_ERROR;

    }

    if(realpath(ruta_temporal, archivo_real) == NULL){

        if(errno == ENOENT){

            return FILES_NO_ENCONTRADO;

        }

        return FILES_ERROR;

    }

    size_t longitud_raiz = strlen(raiz_real);

    if(strncmp(archivo_real, raiz_real, longitud_raiz) != 0 ||
       (archivo_real[longitud_raiz] != '/' && archivo_real[longitud_raiz] != '\0')){

        return FILES_PROHIBIDO;

    }

    resultado = snprintf(
        ruta_final,
        tamano_ruta_final,
        "%s",
        archivo_real
    );

    if (resultado < 0 || resultado >= tamano_ruta_final){

        return FILES_ERROR;

    }

    return FILES_OK;
    
}

int obtener_tamano_archivo(const char *ruta_archivo, long *tamano){

    struct stat informacion;

    if (ruta_archivo == NULL || tamano == NULL){

        return FILES_ERROR;

    }

    if(stat(ruta_archivo, &informacion) != 0){

        if(errno == ENOENT){

            return FILES_NO_ENCONTRADO;

        }

        return FILES_ERROR;

    }

    if(!S_ISREG(informacion.st_mode)){

        return FILES_PROHIBIDO;

    }

    *tamano = informacion.st_size;

    return FILES_OK;

}


int leer_archivo(const char *ruta_archivo, char *buffer, long tamano_buffer, long *bytes_leidos){

    if(ruta_archivo == NULL || buffer == NULL || bytes_leidos == NULL || tamano_buffer < 0){

        return FILES_ERROR;

    }

    FILE *archivo = fopen(ruta_archivo, "rb");

    if(archivo == NULL){

        return FILES_ERROR;

    }

    size_t leidos = fread(buffer, 1, tamano_buffer, archivo);

    if(ferror(archivo)){

        fclose(archivo);

        return FILES_ERROR;

    }

    fclose(archivo);

    *bytes_leidos = leidos;

    return FILES_OK;

}