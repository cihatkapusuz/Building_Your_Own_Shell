#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <iostream>       
#include <deque>          
#include <list>           
#include <queue>          
#include <wait.h>
using namespace std;
int main(){

	cout << "Welcome to my gorgeous shell (Adaşıma selamlar)" << endl;
    pid_t pid; //process id
	//pid_t pid2;
    string input; // the string which stores the user's input
    string catfile; //printed or stored filenames for printfile
	string search_element; // will strore searched element for grep command
    string hist; // each history element
	string user; // the string will store the user
    queue<string> cubi; // the queue that will store the history content
	int i=0; // total number of commands will be computed in this integer i
	int pipefd[2]; // defines read and write ends of the pipe
	pipe(pipefd); // creation of pipe
	pid = fork(); // creating child process to get user

	if (pid < 0) {

		fprintf(stderr, "Fork failed");
		return 1;

	}
	if (pid == 0){ //child
		
		close(pipefd[0]);    // close reading end in the child
		dup2(pipefd[1], 1);  // send stdout to the pipe
		close(pipefd[1]);    // this descriptor is no longer needed

		execlp("whoami","whoami",NULL);

	}
	else { // parent
		
		wait(NULL); // parent will wait for the child to complete
		char buffer[100000]; // assumed the user will not exceed 100000 characters
		close(pipefd[1]);  // close the write end of the pipe in the parent
		if (read(pipefd[0], buffer, sizeof(buffer)) != 0){ // reads the child's output into buffer

			user+=buffer; // char* to string conversion

		}

		user=user.substr(0,user.find("\n")); // gets rid of "\n" of default whoami command
		close(pipefd[0]); // close reading end in the parent

	}

	while(1){
		cout << user <<" >>> " ; // the user + " >>> " output before user enters the input
		getline(cin, input); // gets the input from user
		i=i+1; // number of command that used to print history
		if(cubi.size()<15){  // adding into queue if there are not 15 elements already

			cubi.push(input);

		}
		else{  // already 15 element in the queue discards the oldest element and puts the new one into queue to fit size of 15

			cubi.pop();
			cubi.push(input);

		}
		if(input=="exit"){  // when user inputs "exit" command, program returns 0 so it terminates

			return 0;

		}
		pid = fork(); // creating a child process
		if (pid < 0){  

			fprintf(stderr, "Fork Failed"); 
			return 1;

		}
		else if (pid == 0) { // child

			if(input=="listdir"){ // if the input is only "listdir" calls "ls" command itself

				execlp("/bin/ls","ls",NULL);

			}
		    else if(input=="listdir -a"){ // if the input is only "listdir -a" calls "ls -a" command itself

		        execlp("/bin/ls","ls","-a",NULL);

		    }
		    else if(input=="currentpath"){ // if the input is only "currentpath" calls "pwd" command itself

		        execlp("/bin/pwd","pwd",NULL);

		    }
		    else if(input.substr(0,9)=="printfile"){ // checks the input contains "printfile" in it or not

		        int needed = input.find(" "); // finds the index of first space in the input
		        int needed2 = input.find(">"); // finds the index of ">" int the input
		        if(needed2==-1) { // this means no ">" exists in the input

					catfile=input.substr(needed + 1); // extracts the filename from the input
		            execlp("/bin/cat","cat", catfile.c_str(),NULL); // calls "cat" command itself with given file

		        }
		        else{ // this means in the input there exist ">"

					pipe(pipefd); // create pipe to send output of the first part
					pid = fork();
					string catie; // the string that I will consume each line of the output of child to write 
					if (pid < 0) {

						fprintf(stderr, "Fork failed");
						return 1;

					}
					if (pid == 0){

						close(pipefd[0]);    // close reading end in the child
						dup2(pipefd[1], 1);  // send stdout to the pipe
						close(pipefd[1]);    // this descriptor is no longer needed

						catfile=input.substr(needed + 1,(needed2-needed-2)); // extracts the filename from the input
						execlp("/bin/cat","cat",catfile.c_str(),NULL);

					}
					else
					{
						
						wait(NULL); // parent will wait for the child to complete
						close(pipefd[1]);  // close the write end of the pipe in the parent
						dup2(pipefd[0], 0); // receive stdin form the pipe
						close(pipefd[0]); // close the reading end in the parent
						ofstream myfile;
						myfile.open(input.substr(needed2 + 2).c_str()); // open file that will be written with the childs' output
						if (myfile.is_open()) {

							while(getline(cin,catie)){ // gets each line till the EOF

								myfile << catie << endl;

							}

						}	

						myfile.close(); // last line
						exit(0); // exit from the child when there are no exec() call
					}

		        }

		    }
			else if(input=="footprint") { // checks the input is "footprint" or not

		        for (int j = 0; j < cubi.size(); j++) {

		            hist = cubi.front();
		            cubi.pop();
		            cubi.push(hist);
		            cout << i-cubi.size()+j+1 << " "<< hist << endl; // prints each content of the history with corresponding number.
					
		        }

				exit(0);

		    }
			else if(input.find("|")!=-1){ // checks whether input contains "|" or not

			pipe(pipefd); // creates a pipe to get output from the child to parent
			pid = fork(); // creation of child process
			if (pid < 0) {

				fprintf(stderr, "Fork failed");
				return 1;

			}
			if (pid == 0){

				close(pipefd[0]);    // close reading end in the child
				dup2(pipefd[1], 1);  // send stdout to the pipe
				close(pipefd[1]);    // this descriptor is no longer needed

				if(input.substr(0,10)=="listdir -a"){ // checks the input contains -a condition or not
					execlp("/bin/ls","ls","-a",NULL); // execution of child with -a condition
				}
				else{ // execution of child without -a condition
					execlp("/bin/ls","ls",NULL);
				}
			}
			else{
				// parent
				wait(NULL);

				close(pipefd[1]);  // close the write end of the pipe in the parent
				dup2(pipefd[0], 0); // receive stdin form the pipe
				close(pipefd[0]); // close the read end of the pipe in the parent

				search_element=input.substr(input.find("\"")+1,input.substr(input.find("\"")+1).find_last_of("\"")); // extracts the search element form the input
				execl("/bin/grep", "grep", search_element.c_str(), NULL);

				}
			}
			
			else{

				cout << "INVALID INPUT" << endl; // if the user enters an invalid input notifies the user that s/he entered an invalid input
				exit(0); // exit from the child when there are no exec() call

			}

		}
		else{ //parent
		  
			wait(NULL); // parent will wait for the child to complete
			
			}
		}
	return 0;
}
