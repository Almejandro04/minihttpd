#include <stdio.h>
#include <string.h>
#include "files.h"

#define DIRECTORIO_WEB "www"

int constuir_ruta_archivo(const char *ruta_http, char *ruta_final, int tamaño_ruta_final) {
    
    if (ruta_http == NULL || ruta_final == NULL || tamaño_ruta_final <=0) {

        return -1;

    }

    if(strcmp(ruta_http, "/") == 0) {
        
        int resultado = snprintf(ruta_final, tamaño_ruta_final, "%s/%s", DIRECTORIO_WEB, "index.html");

        if (resultado < 0 || resultado >= tamaño_ruta_final) {

            return -1;

        }

        return 0;

    }

    int resultado = snprintf(ruta_final, tamaño_ruta_final, "%s%s", DIRECTORIO_WEB, ruta_http);

    if (resultado < 0 || resultado >= tamaño_ruta_final){

        return -1;

    }

    return 0;
    
}