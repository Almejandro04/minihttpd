#include <string.h>
#include "mime.h"

const char *obtener_tipo_MIME(const char *ruta){
    
    const char *extension = strrchr(ruta, '.');

    if (extension != NULL) {

        if (strcmp(extension, ".html") == 0)
        {
            return "text/html";
        }
        if (strcmp(extension, ".css") == 0)
        {
            return "text/css";
        }
        if (strcmp(extension, ".js") == 0)
        {
            return "application/javascript";
        }
        if (strcmp(extension, ".png") == 0)
        {
            return "image/png";
        }
        if (strcmp(extension, ".jpg") == 0)
        {
            return "image/jpeg";
        }

    }

    return NULL;


}