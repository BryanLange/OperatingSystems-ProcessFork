#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#define MAXLINE 4096

/*------------------------------------------------------------------- 
   About: This program determines the amount of time necessary to 
		  run a command by forking a child process and measuring the
		  time it takes to return to the parent process
   Compile: $ g++ time.cpp 
   Execute: $ ./a.out <name of command>  ( example:$ ./a.out whoami )
-------------------------------------------------------------------*/


int main(int argc, char **argv) {
	

	// check for proper number of command line arguments
	if(argc != 2) {
		std::cerr << "Invalid number of arguments, program terminated.\n";
		return 3;
	}

	int n, fd[2];
	pid_t pid;
	char line[MAXLINE];
	

	// check for pipe creation failure
	if(pipe(fd) < 0) {
		std::cerr << "Pipe failed\n";
		return 1;
	}

	// fork child process and check for fork creation failure
	pid = fork();
	if(pid < 0) {
		std::cerr << "Fork failed\n";
		return 2;
	}

	

	// PARENT PROCESSING:
	if(pid > 0) {

		close(fd[1]); // close write end of pipe

		std::cout << "\nName of command: " << argv[1] << std::endl;


		// read from the pipe and store into 'line'(char array)
		read(fd[0], line, MAXLINE);


		// split the character array on comma delimiter
		char* pch;
		pch = strtok(line, ","); 		// get first char string
		int startTime_sec = atoi(pch);		// cast string to int using atoi
		pch = strtok(NULL, ","); 		// get next char string
		int startTime_usec = atoi(pch);

		std::cout << "\nStarting Time(parent): " << startTime_sec << "." <<
				startTime_usec << std::endl;

		// block parent process until child is finished
		wait(NULL);


		// get ending time
		struct timeval endTime;
		gettimeofday(&endTime, NULL);
		

		// output ending time
		std::cout << "\nEnding time: " << endTime.tv_sec << "." <<
				endTime.tv_usec << std::endl;


		/* calculate and output elapsed time using starting time received 
		from pipe and ending time taken after child process finished */
		std::cout << "\nElaspsed time(microseconds): " <<
				 ((endTime.tv_sec - startTime_sec)*1000000 +
				endTime.tv_usec) - startTime_usec << "\n\n";
		
	} // end parent processing

	

	/* CHILD PROCESSING:
		This code runs when pid is equal to 0 (child process)
	*/
	else { 

		close(fd[0]); // close read end of pipe

		// get starting time for pipe
		struct timeval current;
		gettimeofday(&current, NULL);
		
		
		// convert timeval struct to string(with comma delimiter) and
		// convert string to character string
		std::string s = std::to_string(current.tv_sec) + "," +
				std::to_string(current.tv_usec);
		char cstr[s.size()+1];
		strcpy(cstr, s.c_str());
	
	
		// write character string to pipe
		write(fd[1], cstr, s.length());

	
		// get starting time
		struct timeval actualStart;
		gettimeofday(&actualStart, NULL);
	
	
		// output starting time
		std::cout << "\nStarting time(child): " << actualStart.tv_sec << "." <<
			 	actualStart.tv_usec << std::endl; 		


		/* output name of command and execute command.
		exec() creates a new image of the process, no code past the exec() will
		be run unless exec() fails */
		std::cout << "\nOutput of command:\n";
		execlp(argv[1], argv[1], NULL);

	} // end child processing

	return 0;

} // end main
