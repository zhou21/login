src = main.c http.c cJSON.c link.c log.c
CC=gcc
way=_GET_

all: wifigod

wifigod: $(src)
	$(CC) $(src) -o $@ -lm -D way

.PHONY: clean all

clean:
	rm -f wifigod
