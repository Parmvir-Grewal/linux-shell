#include <unistd.h>

struct listen_sock {
    struct sockaddr_in *addr;
    int sock_fd;
};

//Returns 1 if "=" exits otherwise 0
int check_pipe(char **tokens);

int builtin_runner(char** tokens);

int pipe_helper(char **tokens);

void check_process_done();

void run_builtin_background(char **tokens, int count);

int print_processes();

int run_bash_command(char **tokens);

int setup_server_socket(struct listen_sock *s, char* server_port);

int send_message(char** tokens);

int connect_client_to_Server(char** tokens);

int accept_connection(int listenfd);

ssize_t start_server(char **tokens);