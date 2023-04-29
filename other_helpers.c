#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
#include "io_helpers.h"
#include "other_helpers.h"

int currdepth = 0;

size_t ls_f_helper(char *path, char *specifier)
{
  struct dirent *directory;
  DIR *parent;
  parent = opendir(path);
  if (parent == NULL)
  {
    display_error("ERROR: ", "Invalid path");
    return -1;
  }
  while ((directory = readdir(parent)) != NULL)
  {
    if (strstr(directory->d_name, specifier) != NULL)
    {
      display_message(directory->d_name);
      display_message("\n");
    }
  }
  closedir(parent);
  return 0;
}


size_t ls_rec_helper(char *path, char *depth, char *specifier)
{
  currdepth++;
  int depthentered = atoi(depth);
  struct dirent *directory = NULL;
  DIR *parent;
  parent = opendir(path);
  if (parent == NULL)
  {
    display_error("ERROR: ", "Invalid path");
    return -1;
  }
  if (depthentered == 1)
  {
    if (specifier != NULL)
    {
      ls_f_helper(path, specifier);
    }
    else
    {
      char *tokens[] = {"ls", path, NULL};
      bn_ls(tokens);
    }
  }
  else
  {
    while ((directory = readdir(parent)) != NULL)
    {
      if (specifier != NULL)
      {
        if (strstr(directory->d_name, specifier) != NULL)
        {
          display_message(directory->d_name);
          display_message("\n");
        }
      }
      else
      {
        display_message(directory->d_name);
        display_message("\n");
      }

      if (strcmp(directory->d_name, ".") == 0 || strcmp(directory->d_name, "..") == 0 || strchr(directory->d_name, '.') != NULL)
      {
        continue;
      }
      else
      {
        char new_path[64] = "";
        strcat(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, directory->d_name);
        DIR *dir = opendir(new_path);
        if (dir)
        {
          closedir(dir);
          if (depthentered == currdepth)
          {
            break;
          }
          else
          {
            char str[64];
            sprintf(str, "%d", depthentered);
            ls_rec_helper(new_path, str, specifier);
          }
        }
      }
    }
  }
  currdepth = 0;
  closedir(parent);
  return 0;
}

int cat_count_helper(char *path) {
    FILE *file;
    char c;    
    file = fopen(path, "r");
    c = fgetc(file);
    int i = 0;
    while (c != EOF) {
        i++;
        c = fgetc(file);
    }
    fclose(file);
    return i;
}