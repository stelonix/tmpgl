#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "helpers.h"

FILE* stdout_bk;
int pipefd[2];

void setup_stdout() {
	auto num = dup(fileno(stdout));

	pipe2(pipefd, O_NONBLOCK);
	// What used to be stdout will now go to the pipe.
	dup2(pipefd[1], fileno(stdout));
	stdout_bk = fdopen(num, "w");

	setvbuf(stdout_bk, NULL, _IONBF, 0);
	//fprintf(stdout_bk,"reacher\n");
}

void *poll_stdout(void *ptr) {
	struct pollfd fds[2];
	fds[0].fd = pipefd[0];
	fds[0].events = POLLIN|POLLPRI;
	fds[1].fd = pipefd[1];
	fds[1].events = POLLOUT|POLLPRI;
	auto actual_pipe = fdopen(pipefd[1], "w");
	while (1) {
		auto ret = poll(fds, 2, 10);
		if (ret > 0) {
			char buf[1024]; char trimmed[1024];
			while (read(pipefd[0], buf, 1024) > 0 ) {
				trim_out(trimmed, 1024, buf);
				//if (strlen(trimmed) == 0) break;
				fprintf(stdout_bk,"%s", buf);
				memset(buf, 0, 1024);
			}
		} else if (ret == -1) {
			fprintf(stdout_bk,"errno: %d\n", errno);
		}
		usleep(500000);
	}
}

void setup_threads() {
	pthread_t thread1;
	pthread_create( &thread1, NULL, &poll_stdout, (void*) NULL);
}