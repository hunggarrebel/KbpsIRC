#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "tools.h"

/*
 * This function formats a string and sends it
 * to the server through the socket, showing it
 * on stdout in the process.
 */

void
raw(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(sbuf, 512, fmt, ap);
	va_end(ap);

	strcpy(&sbuf[509], "\r\n");

	printf("<< %s", sbuf);
	write(conn, sbuf, strlen(sbuf));
}

/* PONG :target\r\n  */

void
pong(char *trg)
{
	raw("PONG :%s\r\n", trg);
}

/* PRIVMSG target :message\r\n */

void
privmsg(char *trg, char *msg, ...)
{
	char fmt_msg[512];
	va_list ap;

	va_start(ap, msg);
	vsnprintf(fmt_msg, 512, msg, ap);
	va_end(ap);

	raw("PRIVMSG %s :%s\r\n", trg, fmt_msg);
}

/* NOTICE target :message\r\n */

void
notice(char *trg, char *msg, ...)
{
	char fmt_msg[512];
	va_list ap;

	va_start(ap, msg);
	vsnprintf(fmt_msg, 512, msg, ap);
	va_end(ap);

	raw("NOTICE %s :%s\r\n", trg, fmt_msg);
}

/* PART target :message\r\n */

void
part(char *trg, char *msg, ...)
{
	char fmt_msg[512];
	va_list ap;

	va_start(ap, msg);
	vsnprintf(fmt_msg, 512, msg, ap);
	va_end(ap);

	raw("PART %s :%s\r\n", trg, fmt_msg);
}

/* QUIT :message\r\n */

void
quit(char *msg, ...)
{
	char fmt_msg[512];
	va_list ap;

	va_start(ap, msg);
	vsnprintf(fmt_msg, 512, msg, ap);
	va_end(ap);

	raw("QUIT :%s\r\n", fmt_msg);
}

/* JOIN :target\r\n */

void
join(char *trg, ...)
{
	char fmt_trg[512];
	va_list ap;

	va_start(ap, trg);
	vsnprintf(fmt_trg, 512, trg, ap);
	va_end(ap);

	raw("JOIN :%s\r\n", fmt_trg);
}
