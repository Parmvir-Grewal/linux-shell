#include <unistd.h>

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
size_t ls_f_helper(char* path, char* specifier);

 /* Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
size_t ls_rec_helper(char* path, char* depth, char* specifier);

int cat_count_helper(char *path);