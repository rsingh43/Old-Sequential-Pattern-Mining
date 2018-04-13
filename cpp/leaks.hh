#ifndef _LEAKS_HH_
#define _LEAKS_HH_

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

void call_leaks(const char *prog) {
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
	}
	else if (pid == 0) {
		execlp("leaks", "leaks", prog, (char*)0);
		perror("execlp");
		exit(EXIT_FAILURE);
	}
	else {
		int status;
		if(waitpid(pid, &status, 0) == -1) {
			perror("waitpid");
		}
		else {
			if (WIFEXITED(status)) {
				//printf("child exited with code %d\n", WEXITSTATUS(status));
				std::cerr << "child exited normally with code " << WEXITSTATUS(status) << std::endl;
			}
			else if (WIFSIGNALED(status)) {
				std::cerr << "child exited because of an uncaught signal " << WTERMSIG(status) << "(" <<  strsignal(WTERMSIG(status)) << ")" << std::endl;
			}
			else if (WIFSTOPPED(status)) {
				std::cerr << "child process stopped by signal " << WSTOPSIG(status) << "(" <<  strsignal(WSTOPSIG(status)) << ")" << std::endl;
			}
		}
	}
}

#endif
