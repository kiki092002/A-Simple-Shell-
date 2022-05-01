/* Name: Tuan Phan & Nga Hoang
   File : myshell.cpp
   Description: a simple shell is built by apply fork(), execvp(), wait(),dup2(), pipe() system calls.
*/
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
using namespace std;

void vectorSplit(vector<string>& vec, string s, char c);
void doCommand(vector<string> vec);
int ForkCommand(vector<string> vec);
void splitPipe(vector<string> vec, vector<vector<string> >& commands);
void doPipe(vector<vector<string> > commands);
void Execute(vector<string> vec);

int main(){

    while(1){
        cout << "myshell$";
        string s;
        getline(cin, s);

        if(cin.eof()){
            exit(0);
        }
        vector<string> vec;
        vectorSplit(vec, s, ' ');
	      Execute(vec);
    }


}
// split string by space into vector.                                           
void vectorSplit(vector<string>& vec, string s, char c){
    int string_size = s.length();
    string ele = ""; // temp string                                             
    for(int i = 0; i < string_size; i++){
        if(s[i] == c){
	    if (ele != ""){ // if ele is not black space 
         vec.push_back(ele); // push ele to vec                         
            }
            ele = "";
        } else {
            ele += s[i]; // push scan element to ele.                           
        }
        if(i == (int) s.length() - 1){
            if (ele != ""){
                 vec.push_back(ele); // push ele to vector                      
            }
        }
  }

}
// split pipeline and store it in 2d vector                                     
void splitPipe(vector<string> vec, vector<vector<string> >& commands){
    int cnt = 0;
    vector<string> com;
    commands.push_back(com);
    for(size_t i = 0; i < vec.size(); i++){
        if(vec[i] == "|"){
            commands.push_back(com);
            cnt++;
            continue;
        }
        commands[cnt].push_back(vec[i]);
    }
                                  
}
void doPipe(vector<vector<string>> commands){
  pid_t pid;
  int fd1[2];
  int fd2[2];
  for (int i = 0; i < (int) commands.size(); i++){
    if(i % 2==0){
      pipe(fd2);
    } else {
      pipe(fd1);
    }
    switch(pid = fork()){
    case -1:
      exit(1);
    case 0:
      if (i ==0){ // first born
        dup2(fd2[1],1);
        close(fd2[1]);
        close(fd2[0]);
      } else if(i == (int)(commands.size()-1)){// last born
        if(i%2 == 0){
          dup2(fd1[0], 0);
          close(fd1[0]);
          close(fd1[1]);
        } else{
          dup2(fd2[0], 0);
          close(fd2[0]);
          close(fd2[1]);
        }
      } else{ // middle born
        if(i%2==0){
          dup2(fd1[0], 0);
          dup2(fd2[1], 1);
        } else{
          dup2(fd2[0], 0);
          dup2(fd1[1], 1);
        }
      }
      doCommand(commands[i]);

    default:
      if (i == 0){ // first
        close(fd2[1]);
      } else if(i == (int) (commands.size()-1)){ // last
        if(i%2==0){
          close(fd2[1]);
        } else {
          close(fd1[1]);
        }
      } else { // middle
        if(i%2==0){
          close(fd1[0]);
          close(fd2[1]);
        } else {
          close(fd2[0]);
          close(fd1[1]);
        }
      }
      int returnvalue, status;
      returnvalue = 0;
      waitpid(pid, &status, returnvalue);
      cout << "process " << pid << " exits with " << WEXITSTATUS(status) << endl;
    }
  }
}

        
void doCommand(vector<string> vec){
    char *commands[20];
    size_t count = 0;
    for(size_t i = 0; i <  vec.size(); i++){
        commands[i] = (char *)vec[i].c_str();
        count++;
    }
    char *new_commands[20];
    for(size_t i = 0; i < count; i++){
        new_commands[i] = commands[i];
    }
    new_commands[count] = (char*) NULL;
    int status = execvp(new_commands[0], new_commands);
    perror(new_commands[0]);
    exit(status);
}

int ForkCommand(vector<string> vec){
    int status;
    if (!vec.size()){
        cout <<  "Invalid。" << "\n";
        return 0;
    }
    if(vec.size() == 1 && vec[0] == "exit"){
         exit(1);
    }
    int child = fork() ;
    if (child  < 0) {
        cout << "Failed fork\n";
        exit(1);
    }
    if (child  == 0) {
        doCommand(vec);
    }
    if ( waitpid(child, &status, 0) == -1) {
        perror(" wait for child");
        exit(1);
    }
    cout << "process "<< child << " exists with status " << WEXITSTATUS(status) << ".\n";
    return status;
}
void Execute(vector<string> vec){
   vector<string> commands;
    for(size_t i = 0; i < vec.size(); i++){
        if(vec[i] == "|"){
            vector<vector<string>> commands;
            splitPipe(vec, commands);
            doPipe(commands);                     
            return;
        }
        commands.push_back(vec[i]);
    }
    ForkCommand(commands);

}




