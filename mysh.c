#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>  

#include "builtins.h"
#include "io_helpers.h"

#include "commands.h"
#include "variables.h"

void handler(int code) {   
    display_message("\n");
}  

int main(int argc, char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};
    struct sigaction sa;  
    sa.sa_handler = handler;  
    sa.sa_flags = 0;  
    sigemptyset(&sa.sa_mask);  
    sigaction(SIGINT, &sa, NULL);   

    while (1) {
        // Prompt and input tokenization
       
        // TODO Step 2:
        // Display the prompt via the display_message function.
        display_message(prompt);

        int ret = get_input(input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr);

        // Clean exit
        if (ret != -1 && (token_count == 0 || (strcmp("exit", token_arr[0]) == 0))) {
            if (ret != 1) {
                free_mem();
                if(server_pid != 0) {
                    char str[65];
                    sprintf(str, "%d", server_pid);
                    char *token_arr2[MAX_STR_LEN] = {"kill", str, NULL};
                    bn_kill(token_arr2);
                }
                break;
            }
        }
        check_process_done();
        // Command execution
        if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);
            if (builtin_fn != NULL) {
                if (check_pipe(token_arr) != 0) {
                    ssize_t err = pipe_helper(token_arr);
                    if (err == - 1) {
                     //   display_error("ERROR: Builtin failed: ", token_arr[0]);
                    }
                }
                else {
                    ssize_t err = builtin_fn(token_arr);
                    if (err == - 1) {
                        display_error("ERROR: Builtin failed: ", token_arr[0]);
                    }
                }
                } 
            else {
                if (check_pipe(token_arr) != 0) {
                    ssize_t err = pipe_helper(token_arr);
                    if (err == - 1) {
                      //  display_error("ERROR: Builtin failed: ", token_arr[0]);
                    }
                }
                else if (token_count == 1) {
                    int assgined_exist = check_assign(token_arr[0]);
                    if (assgined_exist != 0) {
                        set_vals(token_arr[0]);
                    }
                    else if (run_bash_command(token_arr) !=0) {
                        display_error("ERROR: Unrecognized command: ", token_arr[0]);
                    }
                }
                else if (run_bash_command(token_arr) !=0) {
                   display_error("ERROR: Unrecognized command: ", token_arr[0]);
                }
            }
        }

    }

    return 0;
}