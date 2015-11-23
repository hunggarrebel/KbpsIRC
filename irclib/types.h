#ifndef _TYPES_H
#define _TYPES_H

typedef struct
{
	char *nickname;
	char *username;
	char *hostmask;
} s_user;

typedef struct
{
	s_user agent;
	char *command;
	char **params;
	int paramcnt;
} s_line;

#endif /* _TYPES_H */
