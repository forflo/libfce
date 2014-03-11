struct fce_env {
	char *config_args;
	char *path;
	char *ptsname;
	char *working_dir;
	int master;
	int slave;
	pid_t pid;
};

/* Strings in command_list will be sent to the bash on startup.
	end_command will be appended at the end of the code that is given
	to the functions fce_exec and variants. This command is usually 
	like "echo <number>" and provides a more or less bulletproof end
	marker */
struct fce_conf {
	char **command_list;
	char *end_command;
};

char *fce_exec(struct fce_env *env, const char *code);

char *fce_exec_rc(struct fce_env *env, const char *code);

int fce_init(struct fce_env *env);

int fce_envinit(struct fce_env *env, const char *conf, const char *path, const char *wdir);

struct fce_env *fce_getenv();

int deamonize(const char *path, int flg);

int deamonize_pid(const char *path, int flg, int pipe[2]);

int back_fork(const char *path, int flg);

int back_fork_pid(const char *path, int flg, int pipe[2]);
