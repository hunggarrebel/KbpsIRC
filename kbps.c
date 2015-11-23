#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>

#include "irclib/tools.h"
#include "irclib/parser.h"
#include "kbps/interpret.h"

/* 
 * Global variables.
 * conn - socket file descriptor.
 * sbuf - server(socket?) buffer.
 */

int conn;
char sbuf[512];

/*
 * Initialize the socket file descriptor.
 * host - address of the server to connect.
 * port - the port to use.
 */

void
initsocket(char *host, char *port)
{
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(host, port, &hints, &res);

	conn = socket(res->ai_family, res->ai_socktype,
		res->ai_protocol);

	connect(conn, res->ai_addr, res->ai_addrlen);
}

int
main(void)
{
	char *host 		= "irc.rizon.net";
	char *port 		= "6667";
	char *channel 	= "#silvestackers";

	char nick[32], pass[32];

	char buf[513];
	int sl;

	printf("Select a nickname for the bot: ");
	fgets(nick, sizeof nick, stdin);

	printf("Need to auth with NickServ? Pass: ");
	fgets(pass, sizeof pass, stdin);

	nick[strcspn(nick, "\n")] = '\0';
	pass[strcspn(pass, "\n")] = '\0';

	initsocket(host, port);

	raw("USER %s 0 * :KbpsIRC bot.\r\n", nick);
	raw("NICK %s", nick);

	while ((sl = read(conn, sbuf, 512)) > 0)
	{
		int i;
		for(i = 0; i < sl; i++)
			buf[i] = sbuf[i];

		buf[strcspn(buf, "\r\n")] = '\0';

		/* TODO: take into account split lines. */

		printf(">> %s", buf);

		interpret(parse(buf));
	}

	return 0;
}
