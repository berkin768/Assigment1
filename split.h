#include "stdio.h"
#include "string.h"
#include "stdlib.h"

void parsing(char **parsedInput,char *string, char *parseType){//can be changed, in order of OUTPUT, INPUT, PARSING TYPE
    int i = 0;     //index of output
    char *temp = malloc(strlen(string)+1);
    strcpy(temp,string);
    char *token = strtok(temp, parseType);  //output words from input
    
    while(token != NULL){  
    parsedInput[i] = malloc(strlen(token)+1);//Ensure a memory for storing
    strcpy(parsedInput[i], token);   //indexing
    token = strtok(NULL, parseType);  //splitting
    i++; //next index
  }
}