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
			list_add(&sodyd->list, &alias);

			return 1;
		}

		else if(nr_tokens == 1)
		{
			struct entry *search;
			int i = 1;
			list_for_each_entry_reverse(search, &alias, list)
			{
				if(i % 2 == 1){
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


	struct entry *search;
	char new_token[10][100] = { 0, };
	int new_nr_token = 0;
	int flag = 0;
	for(int i = 0; i < nr_tokens; i++)
	{
		flag = 0;
		list_for_each_entry_reverse(search, &alias, list)
		{
			if(strcmp(tokens[i], search->string) == 0)
			{
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
		tokens[i] = strdup(new_token[i]);
	}
	nr_tokens = new_nr_token;

	int check_pipe = -1;
	for(int i = 0; i < nr_tokens; i++){
		if(strcmp(tokens[i], "|") == 0){
			check_pipe = i;
			break;
		}
	}

	pid_t pid, pid1, pid2;
	int fd[2];

	if(strcmp(tokens[0], "exit") == 0) return 0;

	else
	{
		if(check_pipe != -1)
		{
			char *tokens1[check_pipe + 1];
			memcpy(tokens1, tokens, sizeof(char*) * check_pipe);
			tokens1[check_pipe] = NULL;

			if(pipe(fd) == -1)
			{
				perror("pipe!");
				return -1;
			}

			pid1 = fork();
			
			if(pid1 == 0){
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);

				if(execvp(tokens1[0], tokens1) == -1)
				{
					fprintf(stderr, "Unable to execute %s\n", tokens1[0]);
					exit(1);
				}
			}
			else if(pid1 == -1)
			{
				perror("fork");
				return -1;
			}


			else{

				close(fd[1]);
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);
			}

			char* tokens2[nr_tokens - check_pipe];
			memcpy(tokens2, &tokens[check_pipe + 1], sizeof(char*) * (nr_tokens - check_pipe - 1));
			tokens2[nr_tokens - check_pipe - 1] = NULL;

			pid2 = fork();
			if(pid2 == 0)
			{

				if(execvp(tokens2[0], tokens2) == -1)
				{
					fprintf(stderr, "Unable to execute %s\n", tokens2[0]);
					exit(1);
				}
			}
				
			else if(pid2 == -1)
			{
				perror("fork");
				return -1;
			}

			if((pid1 > 0) || (pid2 > 0)){
				close(fd[0]);
				close(fd[1]);
				
				int status1, status2;
				waitpid(pid1, &status1, 0);
				waitpid(pid2, &status2, 0);
			}

		}

		else{
			pid = fork();

			if(pid == 0){
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
