/* include libraries. /lib/tlpi_hdr.h is a library that
 * is made/used in Linux programming interface*/


#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "/lib/tlpi_hdr.h"
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>

/* Define buffers, these are directly from Linux Programming interface*/

#ifndef BUF_SIZE /*Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif


int main(int argc, char *argv[])
{

	int outputFd, openFlags; /* Initialize outputfile and flags*/
	int opt, afnd; /* Command-line options. afnd is used to check if -a flag is found (see Linux programming interface appendix B */
	mode_t filePerms; /* File permissions */
	ssize_t numRead;
	char buf[BUF_SIZE+1]; /*So that we have null byte */

	afnd=0;

	/* Check arguments*/
	if(argc == 1 || strcmp(argv[1], "--help") == 0)
	{
		printf("Program needs at least 1 argument\n");
		exit(EXIT_FAILURE);

	}

	/* Command-line arguments cases, this implementation only uses -a to check if we append or not*/
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

	/* From command-line arguments we deduce appending or not. 
	 * I use the approach in Appendix B for this, I could probably just set these in the switch-case*/
	if(afnd!=0)
	{
		openFlags = O_CREAT | O_WRONLY | O_APPEND;
	}
	else
	{
		openFlags = O_CREAT | O_WRONLY | O_TRUNC; 
	}
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;


	/* Create outputfile
	 * This was a headache
	 * With the -a flag in calling the program, I just kept creating a file named "-a" and kept appending to that
	 * This may or may not be the optimal approach, using argv[optind] to determine all other command-line arguments*/
	outputFd = open(argv[optind], openFlags, filePerms);
	if(outputFd == -1)
	{
		printf("%s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Reading using while
	 * Earlier implementations read in 1024 bytes and wrote just that
	 * but calling e.g. 
	 * $ man man | .mytee outputfile.txt 
	 * only wrote the first ~ 490 characters
	 * This works*/
	while((numRead = read(STDIN_FILENO, buf, BUF_SIZE))>0)
	{
		if(write(outputFd,buf, numRead) != numRead)
		{
			printf("Could not write whole buffer\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Let's add the terminating null byte since we make space for it*/
	buf[numRead] = '\0';

	/* Check for errors*/
	if(numRead == -1)
	{
		printf("%s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}



	/* Close the output file */
	if(close(outputFd) == -1)
	{
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	exit(EXIT_SUCCESS);


}

