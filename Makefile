CC=gcc

TARGETS=kbps.c irclib/*.c kbps/*.c

.PHONY: kbps

kbps:
	$(CC) $(CFLAGS) -o bin/kbps $(TARGETS)
