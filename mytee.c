#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "/lib/tlpi_hdr.h"
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>

#ifndef BUF_SIZE /*Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif


int main(int argc, char *argv[])
{
	int outputFd, openFlags;
	int opt, afnd;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE+1]; /*So that we have null byte */

	afnd=0;

	if(argc == 1 || strcmp(argv[1], "--help") == 0)
	{
		printf("Program needs at least 1 argument\n");
		exit(EXIT_FAILURE);

	}

	while((opt = getopt(argc, argv, ":a")) != -1)
	{
		switch(opt)
		{
			case 'a': 
				afnd++;
				break;
			case ':':
				afnd=0;
				break;
			default:
				afnd=0;
		}
	}

	if(afnd!=0)
	{
		openFlags = O_CREAT | O_WRONLY | O_APPEND;
	}
	else
	{
		openFlags = O_CREAT | O_WRONLY | O_TRUNC; 
	}
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	outputFd = open(argv[optind], openFlags, filePerms);
	if(outputFd == -1)
	{
		printf("%s\n",strerror(errno));
	}

	numRead = read(STDIN_FILENO, buf, BUF_SIZE);
	if(numRead == -1)
	{
		printf("%s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}

	buf[numRead] = '\0';
	if(write(outputFd, buf, numRead) == -1)
	{
		printf("%s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(close(outputFd) == -1)
	{
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	exit(EXIT_SUCCESS);


}

