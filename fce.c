#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "fce.h"

/* Couldn't be implemented because the time got short */
void fce_exec(struct fce_env *env, const char *code){
}

/* Couldn't be implemented because the time got short */
int fce_exec_rc(struct fce_env *env, const char *code){
	return 0;
}

/* Couldn't be implemented because the time got short */
char *fce_execs(struct fce_env *env, const char *code){
	return NULL;
}

/* Starts an interactive Interpreter specified by env, 
	puts it into the background and connects STDIN
	STDOUT and STDERR to the newly opened pseudo terminal 
 	Param: env = A valid pointer to an environment structure
 	Return: -1 on failure, 0 on success */
int fce_init(struct fce_env *env){
	FILE *temp;
	int pip[2];
	int slave;
	if(env == NULL)
		return -1;
	pid_t pid;

	if(pipe(pip))
		return -1;

	switch(pid = fork()){
		case -1:
			return -1;
			break;
		case 0:
			/* Make slave of pseudo terminal the new STDIN, STDOUT and
				STDERR */
			if((slave = open (env->ptsname, O_RDWR)) <0)
				return -1;

			back_fork_pid(env->working_dir, 0, pip);
			
			if(dup2(slave, STDIN_FILENO))
				exit(EXIT_FAILURE);
			if(dup2(slave, STDOUT_FILENO))
				exit(EXIT_FAILURE);
			if(dup2(slave, STDERR_FILENO))
				exit(EXIT_FAILURE);

			execlp(env->path, env->path, env->config_args, NULL);
			break;
		default:
			break;
	}

	close(pip[1]);
	if((temp = fdopen(pip[0], "r")) == NULL)
		return -1;
	fscanf(temp, "%d", &(env->pid));
	fclose(temp);
	
	return 0;
}

/* Opens a new pseudo terminal device and populates the members of the
 	given structure with the corresponding values present in the 
 	argumentlist or created while opening the pt 
 	Param: env = A valid pointer to a environment object
 		conf = Contains the configuration parameters
 		path = Represents the program path */
int fce_envinit(struct fce_env *env, const char *conf, const char *path, const char *wdir){
	char *temp;
	if(env == NULL || conf == NULL || path == NULL)
		return -1;	
	env->config_args = (char *) malloc(sizeof(char) * (strlen(conf) + 1));
	env->path = (char *) malloc(sizeof(char) * (strlen(path) + 1));
	env->working_dir = (char *) malloc(sizeof(char) * (strlen(wdir) +1));
	if(env->config_args == NULL || env->path == NULL || env->working_dir == NULL)
		return -1;
	strcpy(env->path, path);
	strcpy(env->config_args, conf);
	strcpy(env->working_dir, wdir);
	
	if((env->master = posix_openpt(O_RDWR | O_NOCTTY)) < 0)
		return -1;
	if(grantpt(env->master) || unlockpt(env->master))
		return -1;
	if((temp = ptsname(env->master)) == NULL)
		return -1;
	env->ptsname = (char *) malloc(sizeof(char) * 
							(strlen(temp) + 1));
	if(env->ptsname == NULL)
		return -1;
	strcpy(env->ptsname, temp);
	
	return 0;
}

/* Constructs a new fce_conf object. 
	Param: ec = A valid string that represents the end command
		the module should send
		cl = A stringlist containing commands for the interpreter
			the module should run, before any other command
	Return: A valid pointer to a populated fce_conf structure
		or NULL on failure. */
struct fce_conf *fce_getconf(const char *ec, const char **cl){
	int i = 0, j;
	struct fce_conf *conf = (struct fce_conf *) 
								malloc(sizeof(struct fce_conf));
	if(conf == NULL || ec == NULL || cl == NULL)
		return NULL;

	while(cl[i++] != NULL);

	char **temp = (char **) malloc(sizeof(char) * i);
	for(j=0; j<i; j++){
		temp[j] = (char *) malloc(sizeof(char) * (strlen(cl[j]) + 1)); 
		strcpy(temp[j], cl[j]);
	}
	temp[i] = NULL;

	char *temp2 = (char *) malloc(sizeof(char) * (strlen(ec) +1));
	strcpy(temp2, ec);

	conf->command_list = temp;
	conf->end_command = temp2;
	return conf;
}

/* Returns an freshly allocated struct fce_env object 
 	Param: void
 	Return: NULL on failure, arbitrary valid pointer on success */
struct fce_env *fce_getenv(){
	struct fce_env *res = (struct fce_env *) 
								malloc(sizeof(struct fce_env));
	if(res == NULL)
		return NULL;

	res->config_args = NULL;
	res->path = NULL;
	res->ptsname = NULL;
	res->working_dir = NULL;
	res->master = -1;
	res->slave = -1;
	res->pid = -1;
	return res;
}
