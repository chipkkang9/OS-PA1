/**********************************************************************
 * Copyright (c) 2020-2023
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "types.h"
#include "list_head.h"
#include "parser.h"
#include <ctype.h>

/***********************************************************************
 * run_command()
 *
 * DESCRIPTION
 *   Implement the specified shell features here using the parsed
 *   command tokens.
 *
 * RETURN VALUE
 *   Return 1 on successful command execution
 *   Return 0 when user inputs "exit"
 *   Return <0 on error
 */

struct list_head alias;
LIST_HEAD(alias);

struct entry{
	struct list_head list;
	char* string;
};

int flag = 0;

int run_command(int nr_tokens, char *tokens[])
{
	pid_t pid;
	//int flag = 0;
	//extern struct list_head alias;

	if(strcmp(tokens[0], "cd") == 0)
	{
		if(nr_tokens == 1)
		{
			chdir(getenv("HOME"));
		}
		else if((nr_tokens == 2) && (strcmp(tokens[1], "~") == 0))
		{
			chdir(getenv("HOME"));
		}
		else
		{
			chdir(tokens[1]);
		}
		return 1;
	}

	else if(strcmp(tokens[0], "alias") == 0)
	{
		if(nr_tokens >= 3)
		{	
			struct entry *ekscnr = malloc(sizeof(struct entry));
			ekscnr->string = strdup(tokens[1]);
					 	
			list_add(&ekscnr->list, &alias);
			char* input = malloc(sizeof(char) * 100);
			for(int i = 2; i <= nr_tokens-1; i++)
			{
				strcat(input, tokens[i]);
				if(i != nr_tokens-1)
					strcat(input, " ");
			}
			
			struct entry *sodyd = malloc(sizeof(struct entry));
			sodyd->string = strdup(input);
			//sodyd->string = strtok(sodyd->string, " ");
			list_add(&sodyd->list, &alias);

			return 1;
		}

		else if(nr_tokens == 1)
		{
			struct entry *search;
			int i = 1;
			list_for_each_entry_reverse(search, &alias, list)
			{
				if( i % 2 == 1 ){
					fprintf(stderr, "%s:", search->string);
				}
				else
				{
					fprintf(stderr, " %s\n", search->string);
				}	
				i++;
			}	
		}

		return 1;
	}

	pid = fork();

	if(strcmp(tokens[0], "exit") == 0) return 0;

	else
	{
		if(pid == 0)
		{
			struct entry *search;
			//int num = 0;
			//char *tmp[100] = { NULL };
			char new_token[10][100] = { 0, };
			//char* token;
			int new_nr_token = 0;
			//char* delimiter = " \t\r\n\f\v";
			int flag = 0;	
			for(int i = 0; i < nr_tokens; i++) {
				flag = 0;
 				 list_for_each_entry_reverse(search, &alias, list) {
        				if(strcmp(tokens[i], search->string) == 0) {
            					struct entry *prev;
            					prev = list_prev_entry(search, list);

            					char* token = strdup(prev->string);
            					int j = 0;
            					for(size_t i = 0; i < strlen(token); i++) {
                					if(token[i] != ' ') {
                    						new_token[new_nr_token][j] = token[i];
                    						j++;
                					} 
							else if(j > 0){
                    						new_token[new_nr_token][j] = '\0';
                    						new_nr_token++;
                   						j = 0;
                					}
						}
						flag = 1;
						if(j > 0){
							new_token[new_nr_token][j] = '\0';
							new_nr_token++;
						}
						break;
        				}
    				}
			
			 	 if(!flag){
					strcpy(new_token[new_nr_token], tokens[i]);
					new_nr_token++;
				}
			}
			for(int i = 0; i < new_nr_token; i++){
				tokens[i] = new_token[i];
			}

							

			if(execvp(tokens[0], tokens) == -1)
			{
				fprintf(stderr, "Unable to execute %s\n", tokens[0]);
				exit(1);
			}
		}
		else if(pid == -1)
		{
			perror("fork");
			return -1;
		}
		else{
			int status;
			waitpid(pid, &status, 0);
		}
		
	}

	return 1;
}

/***********************************************************************
 * initialize()
 *
 * DESCRIPTION
 *   Call-back function for your own initialization code. It is OK to
 *   leave blank if you don't need any initialization.
 *
 * RETURN VALUE
 *   Return 0 on successful initialization.
 *   Return other value on error, which leads the program to exit.
 */
int initialize(int argc, char * const argv[])
{
	return 0;
}


/***********************************************************************
 * finalize()
 *
 * DESCRIPTION
 *   Callback function for finalizing your code. Like @initialize(),
 *   you may leave this function blank.
 */
void finalize(int argc, char * const argv[])
{
}
