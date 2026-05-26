#ifndef FILES_H
#define FILES_H

#define FILES_OK 0
#define FILES_ERROR -1
#define FILES_NO_ENCONTRADO -2
#define FILES_PROHIBIDO -3


int construir_ruta_archivo(const char *ruta_http, char *ruta_final, int tamaño_ruta_final);

int obtener_tamano_archivo(const char *ruta_archivo, long *tamaño);

int leer_archivo(const char *ruta_archivo, char *buffer, long tamaño_buffer, long *bytes_leidos);

#endif