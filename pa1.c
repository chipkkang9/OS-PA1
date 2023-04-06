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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
int run_command(int nr_tokens, char *tokens[])
{
	pid_t pid;
	//int ch;

	if(strcmp(tokens[0], "exit") == 0) return 0;

	else
	{
		pid = fork();
		if(pid == 0) // child process
		{	
			if((execvp(tokens[0], tokens) == -1) && (strcmp(tokens[0], "cd") != 0))
			{
				fprintf(stderr, "Unable to execute %s\n", tokens[0]);
				return 1;
			}
			else
			{
				if(strcmp(tokens[0], "cd") == 0)
				{
					chdir(tokens[1]);
				}
				else{
					execlp(tokens[0], tokens[0], NULL);
				}
			}		
		}
		else if(pid > 1) // parent process
		{
			wait(NULL);
		}
		else // fork error
		{
			printf("error!");
			return 1;
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
