#include <string.h>
#include "variables.h"
#include "builtins.h"
#include "io_helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Creation of struct
struct a_vars {
    char *var_name;
    char *value;
    struct a_vars *next;
    };
struct a_vars *head = NULL;
struct a_vars *nextnode = NULL;

//Returns 1 if "=" exits otherwise 0
int check_assign(const char *cmd) {
    if (strchr(cmd, '=') != NULL) {
        return 1;
    }
    return 0;
}

//Prints value of varibale if it exists
char* return_val(char *token) {
    struct a_vars *temp;
    temp=head;
    token++;
    int found=0;
    while(temp != NULL) {
        if (strcmp(token,temp->var_name) == 0) {
            found=1;
            return temp->value; 
        }
        temp = temp->next;
    }
    if (found==0) {
        token--;
        return token;
    }
    return token;
}

//Assigns variable name and variable value then returns sturct
struct a_vars *assign_var_new(char *var_name, char *value) {
    struct a_vars *new_var = malloc(sizeof(struct a_vars));
	new_var->var_name = malloc(sizeof(char)*64);
	new_var->value = malloc(sizeof(char)*64);
	
    if (new_var != NULL) {
        strncpy(new_var->var_name, var_name, 64);	
		strncpy(new_var->value, value, 64);
        new_var->next = NULL;
    }
    return new_var;
}

//Changes variables value if it exists adn returns 1 or returns 0
int change_if_exists(struct a_vars *head, char *name, char *value) {
    while (head != NULL) {
        if(strcmp(name,head->var_name) == 0) {
            strncpy(head->value, value, 64);
            return 1;
        }
        head = head->next;
    }
    return 0;
}

//Free variables
void free_mem() {
    struct a_vars *temp;
    while (head != NULL) {
        temp = head->next;
        free(head->var_name);
        free(head->value);
        free(head);
        head = temp;
    }
}

//Handles splitting the assignment variables and creating linked list 
void set_vals(char *token) {
    char stoken[64];
    char copy[64];
    strncpy(stoken, token, sizeof stoken);
    strncpy(copy, token, sizeof copy);
    stoken[sizeof stoken - 1] = '\0';
    copy[sizeof copy - 1] = '\0';
    char *varname = strtok(copy, "=");
    char *val = strstr(stoken, "=");
    val++;
    if (change_if_exists(head, varname, val) == 0) { 
        if (head == NULL) {
            head = assign_var_new(varname, val);
            if (head != NULL) {
                nextnode = head;
            }
    
        }
        else {
            nextnode->next = assign_var_new(varname, val);
            if (nextnode->next != NULL) {
                nextnode = nextnode->next;
            }
        }
    }
}