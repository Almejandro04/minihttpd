# MiniHTTPd

MiniHTTPd es un servidor HTTP/1.1 basico desarrollado en lenguaje C para Linux. El proyecto permite servir archivos estaticos desde la carpeta `www/`, procesar solicitudes con el metodo `GET`, responder con codigos de estado HTTP, manejar tipos MIME basicos, utilizar `epoll` para atender multiples clientes y aplicar validaciones de seguridad para evitar accesos fuera del directorio publico.

El objetivo principal del proyecto es comprender como funciona internamente un servidor web sencillo, integrando sockets TCP, analisis de solicitudes HTTP, acceso al sistema de archivos, generacion de respuestas HTTP y un modelo de concurrencia basado en eventos.

## Estructura del proyecto

```text
minihttpd/
├── Makefile
├── README.md
├── include/
│   ├── files.h
│   ├── http.h
│   ├── mime.h
│   └── server.h
├── src/
│   ├── files.c
│   ├── http.c
│   ├── main.c
│   ├── mime.c
│   └── server.c
└── www/
    ├── index.html
    ├── style.css
    ├── app.js
    ├── image.png
    └── foto.jpg
```

## Compilacion

Para compilar el proyecto se debe ejecutar:

```bash
make
```

Esto genera el ejecutable:

```text
minihttpd
```

## Ejecucion

Para ejecutar el servidor se puede usar:

```bash
./minihttpd
```

Tambien se puede ejecutar con:

```bash
make run
```

Por defecto, el servidor escucha en el puerto `8080`.

Una vez iniciado, se puede acceder desde un navegador mediante:

```text
http://localhost:8080/
```

## Pruebas

Para probar la pagina principal:

```bash
curl -i http://localhost:8080/
```

Para probar el archivo principal de forma explicita:

```bash
curl -i http://localhost:8080/index.html
```

Para probar el archivo CSS:

```bash
curl -i http://localhost:8080/style.css
```

Para probar el archivo JavaScript:

```bash
curl -i http://localhost:8080/app.js
```

Para probar un archivo inexistente:

```bash
curl -i http://localhost:8080/noexiste.html
```

El servidor debe responder con:

```text
404 Not Found
```

Para probar un metodo no permitido:

```bash
curl -i -X POST http://localhost:8080/
```

El servidor debe responder con:

```text
405 Method Not Allowed
```

Para probar proteccion contra Directory Traversal:

```bash
curl -i --path-as-is http://localhost:8080/../../etc/passwd
```

El servidor debe responder con:

```text
403 Forbidden
```

Para probar conexion persistente basica:

```bash
curl -i -H "Connection: keep-alive" http://localhost:8080/
```

Para probar cierre de conexion:

```bash
curl -i -H "Connection: close" http://localhost:8080/
```

## Funcionalidades implementadas

El servidor implementa solicitudes HTTP con el metodo `GET`, analisis basico de encabezados, respuestas con codigos de estado, tipos MIME, lectura de archivos estaticos, validacion de rutas, proteccion contra Directory Traversal, uso de `epoll` y soporte basico para conexiones persistentes.

Los codigos de estado implementados son:

```text
200 OK
400 Bad Request
403 Forbidden
404 Not Found
405 Method Not Allowed
500 Internal Server Error
```

Los tipos MIME implementados son:

```text
.html -> text/html
.css  -> text/css
.js   -> application/javascript
.png  -> image/png
.jpg  -> image/jpeg
```

## Seguridad

El servidor valida que las rutas solicitadas pertenezcan al directorio publico `www/`. Para evitar Directory Traversal, se revisan rutas sospechosas y se resuelven rutas reales antes de permitir el acceso a un archivo.

Tambien se evita el uso de funciones inseguras como `strcpy` o `sprintf`, priorizando funciones con control de tamano como `snprintf`.

## Limpieza

Para eliminar el ejecutable generado:

```bash
make clean
```

## Observaciones

MiniHTTPd es un servidor academico y educativo. No busca reemplazar servidores reales como Apache o Nginx, sino demostrar de forma practica como un servidor HTTP basico puede aceptar conexiones TCP, interpretar solicitudes HTTP, servir archivos estaticos y responder con codigos de estado adecuados.