#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>

int conn;
char sbuf[512];

/* This function works exactly like printf
 * except it sends whatever you pass to the server as well */
void raw(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(sbuf, 512, fmt, ap);
    va_end(ap);
    printf("<< %s", sbuf);
    write(conn, sbuf, strlen(sbuf));
}

int main() {

    char nick[25];
    char pass[25];
    char *channel = "#silverstackers";
    char *host = "irc.rizon.net";
    char *port = "6667";

    /* All the following are pointers into locations in the data recieved from the server
	 * user: 	The user that sent the message.
	 * 		 	So if I said something in a channel or PM, "fscked" would come up here.
	 * command: Always "NOTICE" or "PRIVMSG".
	 * 			Optimize by just checking if command[0] is 'N' or 'P'.
	 * where: 	This will either be a channel or a username.
	 * target: 	So that you don't have to worry about the difference between PMs
	 * 		   	and in-channel messages, this will always be what you should insert
	 * 		   	between your PRIVMSG or NOTICE and your message - it'll be a username
	 * 		   	for a PM, and a channel name for a channel response, depending on whether
	 * 		   	the originating message came from a channel or a PM.
	 * message: The actual message you recieved. This will be \r\n terminated.
	 */
    char *user, *command, *where, *message, *sep, *target;

    /* various counters... 
     * a:	sanatises nick's input
	 * i:	loop while i < sl i.e. while there is a command to read
	 * o:	keeps track of length of command from server (max length 512)
	 * sl:	output of read() goes here
	 * l:	takes the length of the command from o in order to seperate user/targer/where/msg
	 * j:	sorts through the command while it is < l
     */
    int a, i, j, l, sl, o = -1, start, wordcount;
    char buf[513];
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host, port, &hints, &res);
    conn = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(conn, res->ai_addr, res->ai_addrlen);
    
    /* enter nick and password */
    printf("Please select a nick: ");
    fgets(nick, sizeof(nick), stdin);
    /* these next 4 lines remove the garbage from the end of the nick array
     * this is needed or sending nick the the server for identifcation will fail
       because the nick will contain invalid characters */
    a = 0;
    while(nick[a] != '\n')
        a++;
    nick[a] = '\0';
    printf("Please enter the nick password: ");
    fgets(pass, sizeof(pass), stdin);

    /* Demonstration of raw() sends USER and NICK to the server */
    raw("USER %s 0 0 :%s\r\n", nick, nick);
    raw("NICK %s\r\n", nick);

	
/* repeat until read() says 0 bytes were read */
	while ((sl = read(conn, sbuf, 512))) {
        for (i = 0; i < sl; i++) {
            o++;
			/* copy sbuf into buf */
            buf[o] = sbuf[i];
			/* when you reach '\r\n' copy is complete */
            if ((i > 0 && sbuf[i] == '\n' && sbuf[i - 1] == '\r') || o == 512) {
                /* add end of string character to finish the command in buf */
				buf[o + 1] = '\0';
                l = o;
                o = -1;

                printf(">> %s", buf);

				/* If the line starts with PING, set the second byte
				 * in the line to an 'O' and send what we just recieved
				 * straight back to the server */
                if (!strncmp(buf, "PING", 4)) {
                    buf[1] = 'O';
                    raw(buf);
				/* If the line from the server starts with a ':'
				 * it's assumed to be a possible PRIVMSG or NOTICE and parsed as such */
                } else if (buf[0] == ':') {
                    wordcount = 0;
                    user = command = where = message = NULL;
                    /* loop through the command seperating
					 * user
					 * command
					 * where
					 */
					for (j = 1; j < l; j++) {
						/* increment wordcount when a space is reached */
                        if (buf[j] == ' ') {
                            buf[j] = '\0';
                            wordcount++;
                            /* take the user, command and location from buf */
							switch(wordcount) {
                                case 1: user = buf + 1; break;
                                case 2: command = buf + start; break;
                                case 3: where = buf + start; break;
                            }
                            if (j == l - 1) continue;
                            start = j + 1;
                        } else if (buf[j] == ':' && wordcount == 3) {
                            /* finaly, take the message from buf */
							if (j < l - 1) message = buf + j + 1;
                            break;
                        }
                    }

					/* Were there less than 3 words in the response?
					 * We don't have a user/command/location/message combination then -
					 * skip this line */
                    if (wordcount < 2) continue;

					/* Is the command "001"?. If it's 001, ignore everything else in
					 * the line and reply with JOIN <#channel> */
                    if (!strncmp(command, "001", 3) && channel != NULL) {
    					raw("PRIVMSG NickServ :IDENTIFY %s\r\n", pass);
						raw("JOIN %s\r\n", channel);
                    } else if (!strncmp(command, "PRIVMSG", 7) || !strncmp(command, "NOTICE", 6)) {
                        if (where == NULL || message == NULL) continue;
                        if ((sep = strchr(user, '!')) != NULL) user[sep - user] = '\0';

						/* if where is a channel set target to that channel otherwise
						 * set target to user */
						if (where[0] == '#' || where[0] == '&' || where[0] == '+' || where[0] == '!') target = where; else target = user;

						/* reply to the command received */
						printf("[from: %s] [reply-with: %s] [where: %s] [reply-to: %s] %s", user, command, where, target, message);
                    } else if (!strncmp(command, "INVITE", 6)) {
                        if (message == NULL) continue;
                        raw("JOIN %s\r\n", message); /* Join channel the bot was invited to. */
                    }
                }

            }
        }

    }

    return 0;

}
