/* my webserver daemon test program
 *  
 * original daemonize code is written by Aryaman Sharda
 * - see https://gigazine.net/news/20200907-unix-linux-daemon/
 * - see https://digitalbunker.dev/2020/09/03/understanding-daemons-unix/
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>

#define PIDFILE "/var/run/mywebserver.pid"

int run_server(const char *greeting)
{
	int rsock;
	struct sockaddr_in addr={0};
	int ret;

	rsock = socket(PF_INET, SOCK_STREAM, 0);
	if (rsock < 0) {
		syslog(LOG_INFO, "socket(AF_INET)\n");
		return -1;	
	}

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(8080);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(rsock, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0) {
		syslog(LOG_INFO, "bind(rsock) %s\n", strerror(errno));
		return -1;
	}

	syslog(LOG_INFO, "start server: %s", greeting);

	listen(rsock, 5);

	for (int iter=0; iter<100; iter++){
		char msg_in[128];
		char msg_header[128];
		char msg_body[128];

		struct sockaddr_in client={0};
		int len = sizeof(client);
		int wsock = accept(rsock, (struct sockaddr *)&client, &len);

		memset(msg_in, 0, sizeof(msg_in));
		recv(wsock, msg_in, sizeof(msg_in), 0);

		syslog(LOG_INFO, "got a request");
		fprintf(stderr, "Hi stderr\n");

		sprintf(msg_body,
			"<html><body><p>%s: %d/100</p></body></html>\r\n", greeting, iter);
		sprintf(msg_header,
			"HTTP/1.1 200 OK\r\n"
			"Content-Length: %ld\r\n"
			"Content-Type: text/html\r\n"
			"\r\n", strlen(msg_body));
		send(wsock, msg_header, strlen(msg_header), 0);
		send(wsock, msg_body, strlen(msg_body), 0);

		close(wsock);
	}

	close(rsock);

	return 0;
}

static void create_daemon()
{
	pid_t pid;

	/* fork: parent gets a child's; child get 0 */
	pid = fork();
	if (pid < 0) {
		/* case: ERROR */
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		/* case: PARENT */
		printf("fork1: got PID %d\n", pid);
		exit(EXIT_SUCCESS);
	}

	/* setsid: make child a session leader with a new empty terminal */
	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	/* signal: ignore SIGCHLD from parent, SIGHUP from terminal */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	/* fork again: to prevent getting a new terminal */
	pid = fork();
	if (pid < 0) {
		/* case: ERROR */
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		/* case: PARENT */
		printf("fork2: got PID %d\n", pid);
		exit(EXIT_SUCCESS);
	}

	/* directory */
	umask(0);
	chdir("/");

	/* close file descriptors */
	for (int fd=sysconf(_SC_OPEN_MAX); fd>=0; fd--) {
		close(fd);
	}
	
	/* /dev/null for STDIN,OUT,ERR */
	{
		int nullfd = open("/dev/null", O_RDWR);
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
	}	
}

void make_pidfile()
{
	FILE *fp;
	fp = fopen(PIDFILE, "w");
	fprintf(fp, "%d", (int)getpid());
	fclose(fp);
}

int main(int argc, char *argv[])
{
	int ret;
	const char *greeting;
	const char default_greeting[] = "hello from myserver";

#ifdef USE_CUSTOM_IMPL
	printf("custom daemonize routine\n");
	create_daemon();
	make_pidfile();
#endif

#ifdef USE_GLIBC_IMPL
	printf("glibc daemonize routine\n");
	if (daemon(0,0)) {
		exit(EXIT_FAILURE);
	}
	make_pidfile();
#endif

	greeting = (argc>1) ? argv[1] : default_greeting;

	openlog("mywebserver", LOG_PID, LOG_USER);
	ret = run_server(greeting);
	closelog();

	return ret;
}
