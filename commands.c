#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>     
#include <netdb.h>         
#include <netinet/in.h> 
#include <errno.h>
#include <assert.h>

#include "commands.h"
#include "io_helpers.h"
#include "builtins.h"
#include "variables.h"

// Returns 1 if "|" exits otherwise 0
int process_num = 1;
int pid_store[MAX_STR_LEN];
int pid_store_val[MAX_STR_LEN];
int sigval = 0;
void handler_server(int code) {   
    sigval = 1;
}  

int check_pipe(char **tokens)
{
    int i = 0;
    while (tokens[i] != NULL)
    {
        if (strchr(tokens[i], '|') != NULL)
        {
            return 1;
        }
        i++;
    }
    return 0;
}

int builtin_runner(char** tokens) {
        bn_ptr builtin_fn = check_builtin(tokens[0]);
        if (builtin_fn != NULL) {              
            ssize_t err = builtin_fn(tokens);
            if (err == - 1) {
                display_error("ERROR: Builtin failed: ", tokens[0]);
                return -1;
            }
        return 0;
    }
    display_error("ERROR: Unrecognized command: ", tokens[0]);
    return -1;
}

int pipe_helper(char **tokens) {
    char *token_arr1[MAX_STR_LEN] = {NULL};
    char *token_arr2[MAX_STR_LEN] = {NULL};
    int numberfoargs = 0;
    int index = 0;
    int status = 0;
    int indexarr2 = 0;
    int indexarr1 =0;
    pid_t pid = 0;
    int val = 0;
    while (tokens[index] != NULL) {
        if (strncmp(tokens[index], "|", 1) == 0) {
            numberfoargs++;
            status = 1;
        }
        if(status == 1) {
            if (strncmp(tokens[index], "|", 1) != 0) {
                token_arr2[indexarr2] = tokens[index];
                indexarr2++;
             }
        }
        else {
            token_arr1[indexarr1] = tokens[index];
            indexarr1++;  
        }
        index++;
    }
    

    if (strstr(token_arr1[0], "cd") != NULL) {
        return 0;
    }
    if (strchr(token_arr1[0], '=') != NULL) {
        return builtin_runner(token_arr2);
    }
    
    if (strstr(token_arr1[0], "echo") != NULL) {
        if (strstr(token_arr2[0], "echo") != NULL) {
           return builtin_runner(token_arr2);
        }
    }
    if (strstr(token_arr1[0], "echo") != NULL) {
        if (strstr(token_arr2[0], "cat") != NULL) {
           return builtin_runner(token_arr1);
        }
    }

    int fd[2];
    if ((pipe(fd)) == -1) {
        perror("pipe");
        exit(1);
    }
    int result = fork();

    if (result < 0) {
        perror("fork");
        exit(1);
    }

    if (result > 0) {
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        if ((close(fd[0])) == -1) {
            perror("close");
            exit(1);
        }
        
        bn_ptr builtin_fn = check_builtin(token_arr1[0]);
        if (builtin_fn != NULL) {              
            ssize_t err = builtin_fn(token_arr1);
            if (err == - 1) {
                display_error("ERROR: Builtin failed: ", token_arr1[0]);
            }
        }
        display_message("?");
        fflush(stdout);
        if ((close(fd[1])) == -1) {
            perror("close");
            exit(1);
        }
        
    }
    if (result == 0) {
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
       
        if ((close(fd[1])) == -1) {
            perror("close");
            exit(1);
        }
        bn_ptr builtin_fn = check_builtin(token_arr2[0]);
        if (builtin_fn != NULL) {              
            ssize_t err = builtin_fn(token_arr2);
            if (err == - 1) {
                display_error("ERROR: Builtin failed: ", token_arr2[0]);
                return -1;
            }
        }
        else {
            pid = fork();
            if(pid == 0) {
                char path2[64] = "/usr/bin/";
                strcat(path2, token_arr2[0]);
                execv(path2, token_arr2);
            }
            else{
                waitpid(pid, &status, 0);
                if ((close(fd[0])) == -1) {
                    perror("close");
                    exit(1);
                }
                val = 1;
            }
        }
        if(val == 0) {
            if ((close(fd[0])) == -1) {
                perror("close");
                exit(1);
            }
        }
    }
    
   
    return 0;
}

void check_process_done() {
    int i = 1;
    int status;
    char str[64];
    pid_t killvalue;
    size_t n = sizeof(pid_store)/sizeof(pid_store[0]);
    while(i != process_num && i != n) {
        killvalue = waitpid(pid_store[i-1], &status, WNOHANG);
        if (killvalue > 0) {
            process_num--;
            sprintf(str, "[%d]+  Done \t\t\t sleep %d\n", process_num, pid_store_val[i-1]);
            display_message(str);
        }
        i++;
    }      
}
void run_builtin_background(char **tokens, int count) {
    pid_t pid = 0;
    char str[64];
    tokens[count] = NULL;
    pid = fork();
        if(pid == 0) {
            bn_ptr builtin_fn = check_builtin(tokens[0]);
            if (builtin_fn != NULL) {              
                ssize_t err = builtin_fn(tokens);
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", tokens[0]);
            }
            exit(0);
            }
        }
        else {
                pid_store[process_num-1] = pid;
                sprintf(str, "[%d] %d\n", process_num, pid);
                display_message(str);
                process_num++;
        }

}

int print_processes() {
    int i = 0;
    char str[64];
    size_t n = sizeof(pid_store)/sizeof(pid_store[0]);
    while(i != process_num-1 && i != n) {
        sprintf(str, "sleep %d\n", pid_store[i]);
        display_message(str);
        i++;
    }
    return 0;
}

int run_bash_command(char **tokens) {
    int status = 0;
    pid_t pid = 0;
    int background = 0;
    char str[64];
    char path[64] = "/bin/";
    char path2[64] = "/usr/bin/";
    if(tokens[2] != NULL) {
        if(strstr(tokens[2], "&") != NULL) {
                tokens[2] = NULL;
                background = 1;
        }
    }
    pid = fork();
    if(pid == 0) {
        strcat(path, tokens[0]);
        execv(path, tokens);
        strcat(path2, tokens[0]);
        execv(path2, tokens);
        exit(1);
    }
    else {
        if(background) {
            pid_store[process_num-1] = pid;
            pid_store_val[process_num-1] = atoi(tokens[1]);
            sprintf(str, "[%d] %d\n", process_num, pid);
            display_message(str);
            process_num++;
            return 0;
        }
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit = WEXITSTATUS(status); 
            if(exit == 1) {
                return -1;
            }
        }
    }
    return 0;
}

int setup_server_socket(struct listen_sock *s, char* server_port) {
    if(!(s->addr = malloc(sizeof(struct sockaddr_in)))) {
        display_error("malloc", "");
        return 1;
    }
    // Allow sockets across machines.
    s->addr->sin_family = AF_INET;
    // The port the process will listen on.
    s->addr->sin_port = htons(strtol(server_port, NULL, 10));
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(s->addr->sin_zero), 0, 8);
    // Listen on all network interfaces.
    s->addr->sin_addr.s_addr = INADDR_ANY;

    s->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s->sock_fd < 0) {
        display_error("server socket", "");
        return 1;
    }
    int on = 1;
    int status = setsockopt(s->sock_fd, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    if (status < 0) {
        display_error("setsockopt", "");
        return 1;
    }
    if (bind(s->sock_fd, (struct sockaddr *)s->addr, sizeof(*(s->addr))) < 0) {
        display_error("ERROR: ", "PORT IN USE");
        display_error("ERROR: Builtin failed: ", "start-server");
        close(s->sock_fd);
        return 1;
    }
    if (listen(s->sock_fd, 5) < 0) {
        display_error("server: listen", "");
        close(s->sock_fd);
        return 1;
    }
    return 0;
}

int send_message(char** tokens) {
    char buf[MAX_STR_LEN];
    int index = 0;
    while(tokens[index] != NULL) {
        if (strchr(tokens[index], '$') != NULL) {
            strcpy(tokens[index],return_val(tokens[index]));
        }
        index++;
    }
    int socket_fd = socket(AF_INET , SOCK_STREAM , 0);

    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(strtol(tokens[1], NULL, 10));

    if (inet_pton(AF_INET, tokens[2], &peer.sin_addr) < 0) {
        display_error("inet_pton", "");
        exit(1);
    }
    if (connect(socket_fd , (struct sockaddr *)&peer, sizeof(peer)) == -1){
        display_error("connect failed. Error", "");
        exit(1);
    }
        int i = 3;
        int msg_len = 0;
        strcat(buf, "\n");
        msg_len++;
        while(tokens[i] != NULL) {
            strcat(buf, tokens[i]);
            if(tokens[i+1] != NULL) {
                strcat(buf, " ");
            }
            else {
                strcat(buf, "\n");
            }
            msg_len++;
            msg_len += strlen(tokens[i]);
            i++;
        }
        buf[msg_len+1] = '\n';
        write(socket_fd, buf, msg_len+1); 
    return 0;
}

int connect_client_to_Server(char** tokens) {
    struct sigaction sa;  
    sa.sa_handler = handler_server;  
    sa.sa_flags = 0;  
    sigemptyset(&sa.sa_mask);  
    sigaction(SIGINT, &sa, NULL);   
    char buf[MAX_STR_LEN];
    int socket_fd = socket(AF_INET , SOCK_STREAM , 0);

    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(strtol(tokens[1], NULL, 10));

    if (inet_pton(AF_INET, tokens[2], &peer.sin_addr) < 0) {
        display_error("inet_pton", "");
        exit(1);
    }
    if (connect(socket_fd , (struct sockaddr *)&peer, sizeof(peer)) == -1){
        display_error("connect failed. Error", "");
        exit(1);
    }
    if (sigval) {
        return 0;
    }
    fd_set fdset;
    FD_ZERO(&fdset);
    int bytes_read = 0;
    while (!sigval) {
        FD_SET(socket_fd, &fdset);
        FD_SET(STDIN_FILENO, &fdset);
        if(select(socket_fd+1, &fdset, NULL, NULL, NULL) < 0) {  
        }
        if(FD_ISSET(STDIN_FILENO, &fdset)) {
            bytes_read = read(STDIN_FILENO, &buf, MAX_STR_LEN);
            if (bytes_read == 0) {
                return 0;
            }
            write(socket_fd, &buf, bytes_read);
        }
    }
    return 0;
}

int accept_connection(int listenfd) {
    struct sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET;
    int client_socket = accept(listenfd, (struct sockaddr *)&peer, &peer_len);
    if (client_socket < 0) {
        display_error("accept", "");
        return -1;
    } else {
        return client_socket;
    }
}

ssize_t start_server(char **tokens) {
    int index = 0;
    while(tokens[index] != NULL) {
        if (strchr(tokens[index], '$') != NULL) {
            strcpy(tokens[index],return_val(tokens[index]));
        }
        index++;
    }
    struct listen_sock s;
    int i = setup_server_socket(&s, tokens[1]);
    if (i == 1) {
        return i;
    }
    while(1) {
        int fd = accept_connection(s.sock_fd);
        if (fd < 0) {
            continue;
        }
        char buf[MAX_STR_LEN];
        char str[MAX_STR_LEN];
        int index2;
        while ((index2 = read(fd, buf, sizeof(buf) - 1)) > 0) {
            buf[index2] = '\0';
            sprintf(str,"%s", buf);
            display_message(str);
        }
        close(fd);
    }
    exit(0);
}
