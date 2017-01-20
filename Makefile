src = main.c http.c cJSON.c link.c log.c
CC=gcc

all: wifigod

wifigod: $(src)
	$(CC) $(src) -o $@ -lm

.PHONY: clean all

clean:
	rm -f wifigod
