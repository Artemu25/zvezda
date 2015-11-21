#include <iostream>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <string.h>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {
    int file = open("script.py",  O_TRUNC|O_CREAT|O_WRONLY, 0755);
    write(file, "#!/usr/bin/python\nprint ", 24);
    write(file, argv[1], strlen(argv[1]));
    for (int i = 2; i < argc; i++) {
        write(file, "*", 1);
        write(file, argv[i], strlen(argv[i]));
    }
    execlp("./script.py", "./script.py", NULL);
    close(file);
}