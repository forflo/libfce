#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include "fce.h"

#ifdef TEST
#include <stdio.h>
#endif

/* based on the example in "Unix Netzwerkprogrammierung" 
   	by Markus Zahn. Bulletproofly deamonizes the calling process.
 	Param: path = Specifies the new current working directory of the 
 		new deamon process 
 		flg = Flag. If set to a value != 0 the deamon closes all 
 			three standard io streams */
int deamonize(const char *path, int flg){
	pid_t pid;
	/* SIGHUP: Sent if the controlling terminal closes
	 	SIGINT: Sent by the controlling terminal if the user wants
	 		to interrupt the process (typically done by pressing C-c)
	 	SIGQUIT: Sent by the controlling terminal. Triggers termination
	 		and generation of a core dump
	 	SIGTSTP: C-z on the shell. Sent by the controlling terminal 
	 	SIGTTIN/SIGTTOU: Is sent to the process if it reads or writes 
	 		to/from the tty while in the background. */
	int i, sigs[] = {SIGHUP, SIGINT, SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOU};
	struct sigaction action;

	/* Step one. Ignore all terminal generated signals */
	action.sa_handler = SIG_IGN; /* Ignore the signal */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	for (i=0; i<sizeof(sigs) / sizeof(int); i++){
		if(sigaction(sigs[i], &action, NULL) < 0)
			return -1;
	}

	/* Step two. Fork into the background */
	switch(pid = fork()){
		case -1:
			return -1;
			break;
		case 0:
			break;
		default:
			exit(EXIT_SUCCESS);
			break;
	}

	/* Step three. Cut the association with the controlling terminal */
	/* Make the process leader of a new session and a new process group.
	 	After a successful call, the process won't own a controlling 
		terminal anymore */
	if(setsid() < 0)
		return -1;

	/* End the process group and session leadership. From now on, the
		process will not be able to gain a controlling terminal again */
	switch(pid = fork()){
		case -1:
			return -1;
			break;
		case 0:
			break;
		default:
			exit(EXIT_SUCCESS);
			break;
	}

	/* Step four. Close unnecessary file descriptors */
	if(flg){
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	/* Step five. Change the cwd */
	chdir(path);

	/* Step six. Reset the modmask */
	umask(0);

	return 0;
}

int deamonize_pid(const char *path, int flg, int pipe[2]){
	pid_t pid;
	FILE *temp;
	int i, sigs[] = {SIGHUP, SIGINT, SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOU};
	struct sigaction action;

	/* Step one. Ignore all terminal generated signals */
	action.sa_handler = SIG_IGN; /* Ignore the signal */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	for (i=0; i<sizeof(sigs) / sizeof(int); i++){
		if(sigaction(sigs[i], &action, NULL) < 0)
			return -1;
	}

	/* Step two. Fork into the background */
	switch(pid = fork()){
		case -1:
			return -1;
			break;
		case 0:
			break;
		default:
			exit(EXIT_SUCCESS);
			break;
	}

	/* Step three. Cut the association with the controlling terminal */
	/* Make the process leader of a new session and a new process group.
	 	After a successful call, the process won't own a controlling 
		terminal anymore */
	if(setsid() < 0)
		return -1;

	/* End the process group and session leadership. From now on, the
		process will not be able to gain a controlling terminal again */
	switch(pid = fork()){
		case -1:
			return -1;
			break;
		case 0:
			break;
		default:
			exit(EXIT_SUCCESS);
			break;
	}

	/* Step four. Close unnecessary file descriptors */
	if(flg){
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	/* Step five. Change the cwd */
	chdir(path);

	/* Step six. Reset the modmask */
	umask(0);

	/* Step seven. Send the current pid */
	close(pipe[0]);
	temp = fdopen(pipe[1], "w");
	fprintf(temp, "%d", getpid());
	fclose(temp);

	return 0;
}

/* Ignores all signals, forks into the background and changes the current
 	session and makes the calling process itself to the new session and
 	process group leader. The calling process won't own a conrtolling 
 	terminal after this procedure, but it is - unlike the other two
 	functions - possible to gain a new one
 	Param: path = The path that the backgroundly running process will get
 		as it's current working directory
 		flg = If set, the three standard io file descriptors are closed */
int back_fork(const char *path, int flg){
	pid_t pid;
	FILE *temp;
	int i, sigs[] = {SIGHUP, SIGINT, SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOU};
	struct sigaction action;

	/* Step one. Ignore all terminal generated signals */
	action.sa_handler = SIG_IGN; /* Ignore the signal */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	for (i=0; i<sizeof(sigs) / sizeof(int); i++){
		if(sigaction(sigs[i], &action, NULL) < 0)
			return -1;
	}

	/* Step two. Fork into the background */
	switch(pid = fork()){
		case -1:
			return -1;
			break;
		case 0:
			break;
		default:
			exit(EXIT_SUCCESS);
			break;
	}

	/* Step three. Cut the association with the controlling terminal */
	/* Make the process leader of a new session and a new process group.
	 	After a successful call, the process won't own a controlling 
		terminal anymore */
	if(setsid() < 0)
		return -1;

	/* Step four. Close unnecessary file descriptors */
	if(flg){
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	/* Step five. Change the cwd */
	chdir(path);

	/* Step six. Reset the modmask */
	umask(0);

	return 0;
}

/* Ignores all signals, forks into the background and changes the current
 	session and makes the calling process itself to the new session and
 	process group leader. The calling process won't own a conrtolling 
 	terminal after this procedure, but it is - unlike the other two
 	functions - possible to gain a new one
 	Param: path = The path that the backgroundly running process will get
 		as it's current working directory
 		pipe = The new pid is marshalled through it. The parent process 
 			has to read from the pipe to get the pid
 		flg = If set, the three standard io file descriptors are closed */
int back_fork_pid(const char *path, int flg, int pipe[2]){
	pid_t pid;
	FILE *temp;
	int i, sigs[] = {SIGHUP, SIGINT, SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOU};
	struct sigaction action;

	/* Step one. Ignore all terminal generated signals */
	action.sa_handler = SIG_IGN; /* Ignore the signal */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	for (i=0; i<sizeof(sigs) / sizeof(int); i++){
		if(sigaction(sigs[i], &action, NULL) < 0)
			return -1;
	}

	/* Step two. Fork into the background */
	switch(pid = fork()){
		case -1:
			return -1;
			break;
		case 0:
			break;
		default:
			exit(EXIT_SUCCESS);
			break;
	}

	/* Step three. Cut the association with the controlling terminal */
	/* Make the process leader of a new session and a new process group.
	 	After a successful call, the process won't own a controlling 
		terminal anymore */
	if(setsid() < 0)
		return -1;

	/* Step four. Close unnecessary file descriptors */
	if(flg){
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	/* Step five. Change the cwd */
	chdir(path);

	/* Step six. Reset the modmask */
	umask(0);

	/* Step seven. Send the current pid */
	close(pipe[0]);
	temp = fdopen(pipe[1], "w");
	fprintf(temp, "%d", getpid());
	fclose(temp);

	return 0;
}

#ifdef TEST
int main(void){
	pid_t pid;
	int pip[2];
	FILE *f;
	int i;
	if(pipe(pip) < 0)
		return EXIT_FAILURE;
	printf("Pipe -> read: %d write: %d\n", pip[0], pip[1]);

	switch(pid = fork()){
		case -1:
			printf("Fehler beim Forken!\n");
			break;
		case 0:
			if(deamonize_pid("/home/florian/Desktop/", 0, pip))
				printf("Fehler");
			sleep(40);
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
	}

	close(pip[1]);
	f = fdopen(pip[0], "r");

	fscanf(f, "%d", &i);
	fclose(f);
	printf("Die Pid ist: %d", i);
}
#endif
