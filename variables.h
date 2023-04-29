#include <unistd.h>

//Returns 1 if "=" exits otherwise 0
int check_assign(const char *cmd);

//Prints value of varibale if it exists
char* return_val(char *token);

//Assigns variable name and variable value then returns sturct
struct a_vars *assign_var_new(char *var_name, char *value);

//Changes variables value if it exists adn returns 1 or returns 0
int change_if_exists(struct a_vars *anode, char *name, char *value);

//Handles splitting the assignment variables and creating linked list 
void set_vals(char *token);

//Free variables
void free_mem();