#include <iostream>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <string.h>
#include <fstream>
using namespace std;
 
int main(int argc, char * argv[]) {
	fstream s("script.py", ios::out);
	s << " #! /usr/bin/python3" << endl;
	s << "print 1";
	for (int i=1; i<argc; i++) {
		s << "*" << argv[i];
	}
	s << "" << endl;
	execlp("./script.py", "./script.py", NULL);
}