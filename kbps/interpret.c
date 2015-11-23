#include <stdlib.h>
#include <string.h>

#include "../irclib/tools.h"
#include "../irclib/types.h"
#include "interpret.h"

/* proto */

static void flushline(s_line);
static void flushuser(s_user);

/* def */

static void
flushline(s_line line)
{
	int i;

	flushuser(line.agent);
	free(line.command);
	
	for (i = 0; i < line.paramcnt; i++)
		free(*(line.params + i));
	
	free(line.params);
}

static void
flushuser(s_user user)
{
	free(user.nickname);
	free(user.username);
	free(user.hostmask);
}

void
interpret(s_line line)
{
	if (!strcmp(line.command, "PING"))
		pong(line.params[0]);

	flushline(line);
}
