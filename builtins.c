#include <string.h>
#include <dirent.h> 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include "other_helpers.h"
#include "commands.h"


int server_pid = 0;
// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on error ... but there are no errors on echo.
 */
ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;
    int token_count = 0;
    while (tokens[token_count] != NULL) {
        token_count++;
    }
    token_count--;
    if (strstr(tokens[token_count], "&") != NULL && token_count >= 2) {
        run_builtin_background(tokens, token_count);
        return 0;
    }
    if (tokens[index] != NULL) {
        // TODO:
        // Implement the echo command
        if (index <= 1) {
            if (strchr(tokens[index], '$') != NULL) {
                display_message(return_val(tokens[index]));
            }
            else {
                display_message(tokens[index]);
            }
            if (tokens[index+1] != NULL) {
                display_message(" ");
            }
        }
    }
    while (tokens[index] != NULL) {
        // TODO:
        // Implement the echo command
        if (index > 1) {
            if (strchr(tokens[index], '$') != NULL) {
                display_message(return_val(tokens[index]));
            }
            else {
                display_message(tokens[index]);
            }
            if (tokens[index+1] != NULL) {
                display_message(" ");
            }
        }
        index += 1;
    }
    display_message("\n");

    return 0;
}

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_ls(char **tokens) {
    struct dirent *directory;
    DIR *path;
    ssize_t index = 0;
    ssize_t token_count = 0;
    while (tokens[index] != NULL) {
        if (strchr(tokens[index], '|') != NULL) {
            break;
        }
        token_count++;
        index++;
    }

    if (token_count > 1) {
        if (strncmp(tokens[1], "$", 1) == 0) {
            tokens[1] = return_val(tokens[1]);
        }

    }

    if (token_count == 4) {
        if (strncmp(tokens[1], "--rec", 5) == 0 && strncmp(tokens[2], "--d", 3) == 0) {
            return ls_rec_helper(".", tokens[3], NULL);   
        }
        else if (strncmp(tokens[1], "--d", 3) == 0 && strncmp(tokens[3], "--rec", 5) == 0) {
            return ls_rec_helper(".", tokens[2], NULL);   
        }
    }
    if (token_count == 5) {
        if (strncmp(tokens[1], "--rec", 5) == 0 && strncmp(tokens[3], "--d", 3) == 0) {
            return ls_rec_helper(tokens[2], tokens[4], NULL);   
        }
        else if (strncmp(tokens[1], "--d", 3) == 0 && strncmp(tokens[3], "--rec", 5) == 0) {
            return ls_rec_helper(tokens[4], tokens[2], NULL);   
        }
    }
    if (token_count == 6) {
        if (strncmp(tokens[1], "--f", 3) == 0 && strncmp(tokens[3], "--rec", 5) == 0 && strncmp(tokens[4], "--d", 3) == 0) {
            return ls_rec_helper(".", tokens[5], tokens[2]);   
        }
        else if (strncmp(tokens[1], "--rec", 5) == 0 && strncmp(tokens[2], "--f", 3) == 0 && strncmp(tokens[4], "--d", 3) == 0) {
            return ls_rec_helper(".", tokens[5], tokens[3]);   
        }
        else if (strncmp(tokens[1], "--rec",5) == 0 && strncmp(tokens[2], "--d", 3) == 0 && strncmp(tokens[4], "--f", 3) == 0) {
            return ls_rec_helper(".", tokens[3], tokens[5]);   
        }
        else if (strncmp(tokens[1], "--d", 3) == 0 && strncmp(tokens[3], "--rec", 5) == 0 && strncmp(tokens[4], "--f", 3) == 0) {
            return ls_rec_helper(".", tokens[2], tokens[5]);   
        }
        else if (strncmp(tokens[1], "--d", 3) == 0 && strncmp(tokens[3], "--f", 3) == 0 && strncmp(tokens[5], "--rec", 5) == 0) {
            return ls_rec_helper(".", tokens[2], tokens[4]);   
        }
        else if (strncmp(tokens[1], "--f", 3) == 0 && strncmp(tokens[3], "--d", 3) == 0 && strncmp(tokens[5], "--rec", 5) == 0) {
            return ls_rec_helper(".", tokens[4], tokens[2]);   
        }
    }
    if (token_count == 7) {
        if (strncmp(tokens[1], "--f", 3) == 0 && strncmp(tokens[3], "--rec", 5) == 0 && strncmp(tokens[5], "--d", 3) == 0) {
            return ls_rec_helper(tokens[4], tokens[6], tokens[2]);   
        }
        else if (strncmp(tokens[1], "--rec", 5) == 0 && strncmp(tokens[3], "--f", 3) == 0 && strncmp(tokens[5], "--d", 3) == 0) {
            return ls_rec_helper(tokens[2], tokens[6], tokens[4]);   
        }
        else if (strncmp(tokens[1], "--rec",5) == 0 && strncmp(tokens[3], "--d", 3) == 0 && strncmp(tokens[5], "--f", 3) == 0) {
            return ls_rec_helper(tokens[2], tokens[4], tokens[6]);   
        }
        else if (strncmp(tokens[1], "--d", 3) == 0 && strncmp(tokens[3], "--rec", 5) == 0 && strncmp(tokens[5], "--f", 3) == 0) {
            return ls_rec_helper(tokens[4], tokens[2], tokens[6]);   
        }
        else if (strncmp(tokens[1], "--d", 3) == 0 && strncmp(tokens[3], "--f", 3) == 0 && strncmp(tokens[5], "--rec", 5) == 0) {
            return ls_rec_helper(tokens[6], tokens[2], tokens[4]);   
        }
        else if (strncmp(tokens[1], "--f", 3) == 0 && strncmp(tokens[3], "--d", 3) == 0 && strncmp(tokens[5], "--rec", 5) == 0) {
            return ls_rec_helper(tokens[6], tokens[4], tokens[2]);   
        }
    }
    if (token_count > 3) {
        if (strncmp(tokens[1], "--f", 3) == 0) {
            return ls_f_helper(tokens[3], tokens[2]);
            
        }
        else if (strncmp(tokens[2], "--f", 3) == 0) {
            return ls_f_helper(tokens[1], tokens[3]);
        }
    }
    if (token_count == 1) {
        path = opendir(".");
    }
    else {
        path = opendir(tokens[1]);
    }
    if (path == NULL) {
        display_error("ERROR: ", "Invalid path");
        return -1;
    }
    while ((directory = readdir(path)) != NULL) {
            display_message(directory->d_name);
            display_message("\n");
    }

    closedir(path);
    return 0;
}

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_cd(char **tokens) {
    ssize_t index = 0;
    ssize_t token_count = 0;
    while (tokens[index] != NULL) {
        token_count++;
        index++;
    }
    if (token_count > 2) {
         display_error("ERROR: ", "Invalid path");
        return -1;
    }
    char *st = tokens[1];
    int count = 0;
    if (strncmp(tokens[1], "$", 1) == 0) {
        tokens[1] = return_val(tokens[1]);
    }
    if (strcmp(tokens[1], "..") == 0) {
        count = -1;
        for(int i = 0; i <= strlen(st); i++) {
            if(strncmp(&st[i], ".", 1) == 0)  
            {
                count++;
            }
        }
        while(count !=0) {
            if (chdir("..") != 0) {
                display_error("ERROR: ", "Invalid path");
                return -1;
            }
            count--;
        }
        return 0;
    }
    
    if (chdir(tokens[1]) != 0) {
        display_error("ERROR: ", "Invalid path");
        return -1;
    }
    return 0;
}

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_cat(char **tokens) {
    FILE *file;
    char c;
    
    if (tokens[1] == NULL) {
        struct pollfd fds;
        fds.fd = 0;     
        fds.events = POLLIN; 
        int ret = poll(&fds, 1, 10);
        if (ret == 0) {
            display_error("ERROR: ", "No input source provided");
            return -1;
        }
        else {
            file = stdin;
            char c = fgetc(file);
            while(c != '?') {
                display_message(&c);
                c = fgetc(file);
            }
            return 0;
        }
    }
    file = fopen(tokens[1], "r");
    if (file == NULL) {
        display_error("ERROR: ", "Cannot open file");
        return -1;
    }
    int index = cat_count_helper(tokens[1]);
    c = fgetc(file);
    char charp[index];
    int i = 0;
    int indexcount = index;
    while (c != EOF) {
        charp[i] = c;
        i++;
        if (i == 64){
            display_message(charp);
            i = 0;
            indexcount -= 64;
            strcpy(charp, "");
        }
        c = fgetc(file);
    }
    char s[indexcount+1];
    strncpy(s, charp, indexcount);
    s[indexcount] = '\0';
    display_message(s);
    display_message("\n");
    //fclose(file);
    return 0;
}

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_wc(char **tokens) {
    FILE *file;
    int chcounter = 0;
    int wordcounter = 0;
    int linecounter = 0;
    char c;
    char str[64];
    int wordexits = 0;


    if (tokens[1] == NULL) {
        struct pollfd fds;
        fds.fd = 0;     
        fds.events = POLLIN; 
        int ret = poll(&fds, 1, 10); 
        if (ret == 0) {
            display_error("ERROR: ", "No input source provided");
            return -1;
        }
        else {
            file = stdin;
            int c = fgetc(file);
            while(c != '?') {
                chcounter++;
                if (c == '\n') {
                    linecounter++;
                }
                if (isspace(c) || c == '\n' || c == '\t' || c == '\r') {
                    wordcounter += wordexits;
                    wordexits = 0;
                } 
                else {
                    wordexits = 1;
                }
                c = fgetc(file);
            }
           
            wordcounter += wordexits;
            display_message("word count ");
            sprintf(str, "%d", wordcounter);
            display_message(str);
            display_message("\n");
            display_message("character count ");
            sprintf(str, "%d", chcounter);
            display_message(str);
            display_message("\n");
            display_message("newline count ");
            sprintf(str, "%d", linecounter);
            display_message(str);
            display_message("\n");
            return 0;
        }
    }
    else {
        file = fopen(tokens[1], "r");
        if (file == NULL) {
            display_error("ERROR: ", "Cannot open file");
            return -1;
        }

        c = fgetc(file);
        wordexits = 0;
        while (c != EOF) {
            chcounter++;
            if (c == '\n') {
                linecounter++;
            }
            if (isspace(c) || c == '\n' || c == '\t' || c == '\r') {
                wordcounter += wordexits;
                wordexits = 0;
            } 
            else {
                wordexits = 1;
            }
            c = fgetc(file);
        }
        wordcounter += wordexits;
        display_message("word count ");
        sprintf(str, "%d", wordcounter);
        display_message(str);
        display_message("\n");
        display_message("character count ");
        sprintf(str, "%d", chcounter);
        display_message(str);
        display_message("\n");
        display_message("newline count ");
        sprintf(str, "%d", linecounter);
        display_message(str);
        display_message("\n");
        fclose(file);
    }
    return 0;
}

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_kill(char **tokens) {
        pid_t killvalue;
        killvalue = kill(atoi(tokens[1]), 0);
        if (killvalue == -1) {
            display_error("ERROR: The process does not exist", "");
            return -1;
        }
        if(tokens[2] != NULL) {
            if (strchr(tokens[2], '$') != NULL) {
                    tokens[2] = return_val(tokens[2]);
            }
        }   
        if(tokens[2] == NULL) {
            killvalue = kill(atoi(tokens[1]), SIGTERM);
        }
        else {
            killvalue = kill(atoi(tokens[1]), atoi(tokens[2]));
        }
        if (killvalue == -1) {
            display_error("ERROR: Invalid signal specified", "");
            return -1;
        } 
    return 0;
}

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_ps(char **tokens) {
    return print_processes();
}

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_start_server(char **tokens) {
    pid_t pid = 0;
    if(tokens[1] == NULL) {
        display_error("ERROR: ", "No port provided");
        return -1;
    }
    pid = fork();
    if(pid == 0) {
        start_server(tokens);
        exit(0);
    }
    server_pid = pid;
    return 0;
}

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_close_server(char **tokens) {
    char str[64];
    sprintf(str, "%d", server_pid);
    char *token_arr[MAX_STR_LEN] = {"kill", str, NULL};
    bn_kill(token_arr);
    return 0;
}

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_send(char **tokens) {
    return send_message(tokens);
}

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_start_client(char **tokens) {
    if(tokens[1] == NULL) {
        display_error("ERROR: ", "No port provided");
        return -1;
    }
    else if(tokens[2] == NULL) {
        display_error("ERROR: ", "No hostname provided");
        return -1;
    }
    return connect_client_to_Server(tokens);
}