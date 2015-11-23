#ifndef _TOOLS_H
#define _TOOLS_H

/*
 * Declare external variables conn and sbuf.
 *
 * conn - file descriptor for socket created
 *   in kbps.c.
 * sbuf - array used as the outbound buffer
 *   for the socket/server we have.
 */

extern int 	conn;
extern char	sbuf[512];

void raw(char *, ...);
void pong(char *);
void privmsg(char *, char *, ...);
void notice(char *, char *, ...);
void part(char *, char *, ...);
void quit(char *, ...);
void join(char *, ...);

#endif /* _TOOLS_H */
