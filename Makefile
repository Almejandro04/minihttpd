CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC = src/main.c src/server.c src/http.c src/files.c src/mime.c
OUT = minihttpd

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)

run: all
	./$(OUT)