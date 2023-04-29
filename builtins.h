#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include <unistd.h>

int server_pid;
/* Type for builtin handling functions
 * Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
typedef ssize_t (*bn_ptr)(char **);
ssize_t bn_echo(char **tokens);

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_ls(char **tokens);

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_cd(char **tokens);

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_cat(char **tokens);

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_wc(char **tokens);

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_kill(char **tokens);

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_ps(char **tokens);

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_start_server(char **tokens);

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_close_server(char **tokens);

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_send(char **tokens);

/* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
ssize_t bn_start_client(char **tokens);

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd);


/* BUILTINS and BUILTINS_FN are parallel arrays of length BUILTINS_COUNT
 */
static const char * const BUILTINS[] = {"echo", "ls", "cd", "cat", "wc", "kill", "ps", "start-server", "close-server", "send", "start-client"};
static const bn_ptr BUILTINS_FN[] = {bn_echo, bn_ls, bn_cd, bn_cat, bn_wc, bn_kill, bn_ps, bn_start_server, bn_close_server, bn_send, bn_start_client, NULL};    // Extra null element for 'non-builtin'
static const size_t BUILTINS_COUNT = sizeof(BUILTINS) / sizeof(char *);

#endif