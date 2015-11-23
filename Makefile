prefix=/usr/local

CC=gcc

override CFLAGS+=-Iinclude

TARGETS=kbps.c irclib/*.c kbps/*.c

.PHONY: kbps

kbps:
	$(CC) $(CFLAGS) -o bin/kbps $(TARGETS)
