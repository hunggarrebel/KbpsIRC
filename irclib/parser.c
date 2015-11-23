#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "tools.h"

/* Prototypes for static functions. */
static int parseword(s_line *, char *, int);

static int
parseword(s_line *ret, char *word, int wcount)
{
	if (wcount == 1)
		ret->agent = parseuser(word);
	else if (wcount == 2)
		ret->command = strdup(word);
	else
	{
		if ((ret->params = realloc(ret->params,
			(ret->paramcnt + 1) * sizeof(char *))) == NULL)
		{
			perror("[ERROR] (Re)Allocating memory");
			exit(EXIT_FAILURE);
		}

		/*
		 * If we find a separator character(':'),
		 * we tell parse to stop looking for words
		 * and store the parameter as a complete
		 * string, rather than splitting it into
		 * words.
		 */

		if (word[0] == ':')
		{
			word[strlen(word)] = ' ';
			ret->params[ret->paramcnt++] = strdup(word + 1);
			return 1;
		}
		else
		{
			ret->params[ret->paramcnt++] = strdup(word);
			return 0;
		}
	}
}

s_user
parseuser(char *agent)
{
	s_user ret;
	char *nick, *user, *host;

	if (strchr(agent, '@') == NULL || strchr(agent, '!') == NULL)
	{
		fprintf(stderr, "[ERROR] Cannot parse \"%s\" for a user "
			"because of missing characters '!' and/or '@'.\n",
			agent);

		ret.nickname = ret.username = ret.hostmask = NULL;

		return ret;
	}

	host = strchr(agent, '@') + 1;
	user = strchr(agent, '!') + 1;
	nick = agent;

	*(host - 1) = *(user - 1) = '\0';

	ret.nickname = strdup(nick);
	ret.username = strdup(user);
	ret.hostmask = strdup(host);

	return ret;
}

s_line
parse(char *buf)
{
	s_line ret;
	int wcount, wsplit;
	int i;

	ret.command = ret.params = NULL;
	ret.paramcnt = 0;

	if (!strncmp(buf, "PING", 4))
	{
		if ((ret.params = malloc(sizeof(char *))) == NULL)
		{
			perror("[ERROR] Allocating memory");
			exit(EXIT_FAILURE);
		}

		/*
		 * Explanation for the 6 offset:
		 *   0 1 2 3 4 5 6 7 ...
		 *   P I N G :   s t uff...
		 */

		ret.command 	= strdup("PING");
		ret.params[0] 	= strdup(buf + 6);
		ret.paramcnt 	= 1;

		return ret;
	}

	/*
	 * An invalid message becomes a null s_line
	 * struct.
	 */

	if (buf[0] != ':')
		return ret;
	else
		buf += 1;

	wcount = 0;
	wsplit = 1;

	for (i = 0; buf[i] != '\0'; i++)
	{
		if (buf[i] == ' ')
			buf[i] = '\0';
		else
			continue;

		if (parseword(&ret, buf, wcount++))
			break;

		buf += strlen(buf) + 1;
	}

	/* 
	 * Parse the last word.
	 *
	 * In case parseword() finds a separator(':') character in the
	 * given string, it will set its final character to ' '. Since
	 * this is the last word in our server line and there won't be
	 * any other null characters to stop it from being read by the
	 * interpreter, we set it back to '\0' regardless of parseword()'s
	 * behaviour. 
	 */

	i = strlen(buf);
	parseword(&ret, buf, wcount++);
	ret.params[ret.paramcnt-1][i] = '\0';

	return ret;
}
