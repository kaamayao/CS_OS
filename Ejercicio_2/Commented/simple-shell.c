//defines printf() - prints string to standard output, fprintf() - prints string to file
//The library defines three variable types, several macros, and various functions for performing input and output.
#include <stdio.h>
//defines fork() -create new process and chdir() -change working directory
//header file that provides access to the POSIX operating system API. 
#include <unistd.h>
//defines strtok() - break string into tokens and strcat() -concatenate string 
//The header defines one variable type, one macro, and various functions for manipulating arrays of characters.
#include <string.h>
//defines exit() - terminates calling process, exits program, malloc() - allocate a block of memory dynamically, realloc() - resize a block of memory previously allocated , free() - deallocates memory. EXIT_SUCCESS=0 and EXIT_FAILURE=8 VARIABLES which tells the kernel the status of the program.
//general purpose standard library of C programming language which includes functions involving memory allocation, process control, conversions and others
#include <stdlib.h>
//defines waitpid() - suspends execution of the calling process until a child specified by pid argument has changed state.
//The header shall define the following symbolic constants for use with waitpid():
#include <sys/wait.h>

//defines bool as an abbreviation of int
typedef int bool;

// Defines macro true as 1
#define true 1
// Defines macro false as 0
#define false 0
#define LSH_RL_BUFSIZE 1024 /* buffer size for reading user input */
#define LSH_TOK_BUFSIZE 64 /* buffer size for splitting the arguments */
#define LSH_HIST_SIZE 10 /* buffer size for storing history of commands */
#define LSH_TOK_DELIM " \t\r\n\a" /* delimiters for parsing the arguments */

/* global variable to check parent and child process concurrency */
bool conc = false;
/* global variable to point to the last command executed */
int cur_pos = -1;
/* global variable storing the history of commands executed */
char *history[LSH_HIST_SIZE];
//current buf size of the command storage
int cur_bufsize = LSH_TOK_BUFSIZE;

/* Function declarations for built-in shell commands */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/* List of built-in commands, followed by their corresponding functions */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

//size of the array that stores all the built in functions
int lsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

//BUILT-IN FUNCTION
//This function changes the current console's directory 
int lsh_cd(char **args)
{
    //if no arguments are present It displays an console error 
    if(args[1] == NULL){
        //fprintf prints the erorr inside the file descriptor stderr 
        //which defaults to user screen in the terminal
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
        return 1;
    }

    if(chdir(args[1]) != 0){
       //prints an error by displaying "lsh: <descriptive error>"
       perror("lsh");
    }

    return 1;
}

//BUILT-IN FUNCTION
//This function displays all the console's capabilities. 
//Gets triggered if the user types out "help" as a command
//returns 1 if successful
int lsh_help(char **args)
{
    int i;

    //Prints all the info about the console
    printf("Aman Dalmia's LSH\n");
    printf("Type program names and arguments, and press enter.\n");
    printf("Append \"&\" after the arguments for concurrency between parent-child process.\n");
    printf("The following are built in:\n");


    //iterates and prints all the commands of the builtin_str array which stores all the built-in commands 
    //builtin_str=[cd, help, exit] 
    for(i = 0; i < lsh_num_builtins(); i++){
        printf(" %s\n", builtin_str[i]);
    }

    //prints info about how to access info about other commands
    printf("Use the man command for information on other programs.\n");
    return 1;
}

//BUILT-IN FUNCTION
int lsh_exit(char **args)
{
    return 0;
}

/* Launch a program */
int lsh_launch(char **args)
{
    //pid_t is a unsigned integer capable of storing process ids
    pid_t pid, wpid;
    //stores the status value that the waitpid is going to set, this is used to check if the process is terminated 
    //normally or by a signal
    int status;
    
    //Creates a child process and store It's Id
    pid = fork();

    //Since the child process Id is always set to 0 we can use that to check if the process being executed is the child or parent
    //if there is no other process running concurrently It returns 1 and does nothing
    //If there is a concurrent child process It 

    if(pid == 0){ /* child process */
        //When execvp() is executed, the program file given by the first argument will be loaded into the caller's 
        //address space and over-write the program there. Then, the second argument 
        //will be provided to the program and starts the execution
        //returns -1 if an error occurred during execution and the program will then print an error like so: "lsh <error>"
        if(execvp(args[0], args) == -1) perror("lsh");
        //terminates the child process
        exit(EXIT_SUCCESS);
    } else if(pid > 0 && !conc){ /* parent process */
        //waits till the process finishes executing and is signaled to finish
        do{
            //suspends exectuion of the calling process until the process has changed status
            wpid =  waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }else{ /* error forking */
        perror("lsh");
    }

    //since all the processes ended, the parent sets the concurrency to false
    conc = false;
    return 1;
}

/* Parse input to get the arguments */
char **lsh_split_line(char *line){
    //sets the current buffer size to the default buffer size
    cur_bufsize = LSH_TOK_BUFSIZE;
    //sets the buffer position to 0
    int position = 0;
    //Allocates the memory needed to store tokens of 64 characters
    char **tokens = malloc(cur_bufsize * sizeof(char*));
    //pointer to the current token
    char *token;

    //if the tokens memory is not able to be allocated exit with a failure status
    if(!tokens){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    //breaks the line into tokens using the delimitators assigned (such as \n or \t)
    //and gets the first token
    token = strtok(line, LSH_TOK_DELIM);
    
    // walk through all the other tokens
    while(token != NULL){
        //store the token in the position on the token array
        tokens[position] = token;
        //increase positions 
        position++;

        //if the position is greater than the buffer size allocated, increase the size using realloc
        if(position >= cur_bufsize){
            cur_bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, cur_bufsize * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        //strtok stores a variable of the last time It was called, the NULL param tells the function 
        //to continue tokenizing the first string
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    //If the last token is a & set the process as concurrent with the partent one
    if(position > 0 && strcmp(tokens[position - 1], "&") == 0) {
        conc = true;
        //set the last token & as NULL in the array
        tokens[position - 1] = NULL;
    }
    //set the last position as NULL
    tokens[position] = NULL;
    return tokens;
}

/* History of commands */
int lsh_history(char **args)
{
    //if there are no comamnds store in the history array return failure and exit
    if(cur_pos == -1 || history[cur_pos] == NULL){
        fprintf(stderr, "No commands in history\n");
        exit(EXIT_FAILURE);
    }

    //if the command is set to history list all the commands currently in the history array
    if(strcmp(args[0], "history") == 0){
        int last_pos = 0, position = cur_pos, count = 0;

        //if the current position is not at the last part of the array then the last position is set to current + 1
        if(cur_pos != LSH_HIST_SIZE && history[cur_pos + 1] != NULL){
            last_pos = cur_pos + 1;
        }

        //set the count of all the commands being stored
        count = (cur_pos - last_pos + LSH_HIST_SIZE) % LSH_HIST_SIZE + 1;

        //go throught all the commands stored and print them
        while(count > 0){
            char *command = history[position];
            printf("%d %s\n", count, command);
            position = position - 1;
            position = (position + LSH_HIST_SIZE) % LSH_HIST_SIZE;
            count --;
        }
    }else{
        //used to store the tokens of the line that 
        char **cmd_args;
        //use to store the command that's going to be executed, either from the most current history or from the user input
        char *command;
        //if the user inputs !! then It executes the most current command in the array history 
        //if the user inputs !<number> then executes the history command corresponding to the <number> index
        //iif the user inputs ! without arguments It exits the shell and gives an error
        if(strcmp(args[0], "!!") == 0){
            //allocates the memory given the most current command
            command = malloc(sizeof(history[cur_pos]));
            //sets the command to the most current one
            strcat(command, history[cur_pos]);
            //gets the tokens from the current command
            cmd_args = lsh_split_line(command);
            //if the current command corresponds to a built in function the it executes that one, otherwise It executes the command by lsh_launch
            int i;
            for (i = 0; i < lsh_num_builtins(); i++){
                if(strcmp(cmd_args[0], builtin_str[i]) == 0){
                    return (*builtin_func[i])(cmd_args);
                }
            }
            return lsh_launch(cmd_args);
        }else if(args[0][0] == '!'){
            //if the command ! has no arguments then exit shell with an error
            if(args[0][1] == '\0'){
                fprintf(stderr, "Expected arguments for \"!\"\n");
                exit(EXIT_FAILURE);
            }
            /* position of the command to execute */
            int offset = args[0][1] - '0';
            //set the position of the next command in a cyclic way
            int next_pos = (cur_pos + 1) % LSH_HIST_SIZE;
            //sets the offset of the command that's going to be executed, this offset is 
            //used to find the command corresponding to the !<number> attribute
            if(next_pos != 0 && history[cur_pos + 1] != NULL){
                offset = (cur_pos + offset) % LSH_HIST_SIZE;
            }else{
                offset--;
            }
            //if the index of the command is not found in the history array then exit the shell with an error
            if(history[offset] == NULL){
                fprintf(stderr, "No such command in history\n");
                exit(EXIT_FAILURE);
            }
            //allocates and copies the command from the corresponding index in the history array
            command = malloc(sizeof(history[cur_pos]));
            strcat(command, history[offset]);
            //tokenizes that command
            cmd_args = lsh_split_line(command);
            //if the current command corresponds to a built in function the it executes that one, otherwise It executes the command by lsh_launch
            int i;
            for (i = 0; i < lsh_num_builtins(); i++){
                if(strcmp(cmd_args[0], builtin_str[i]) == 0){
                    return (*builtin_func[i])(cmd_args);
                }
            }
            return lsh_launch(cmd_args);
        }else{
            perror("lsh");
        }
    }
}

/* Execute the parsed arguments */
int lsh_execute(char *line){
    int i;

    //get the tokens from the input line
    char **args = lsh_split_line(line);

    //if the command is empty is empty do nothing
    //if the command is !! or history execute the command from the history array
    if(args[0] == NULL){ /* empty command was entered, do nothing */
        return 1;
    }else if(strcmp(args[0], "history") == 0 ||
             strcmp(args[0], "!!") == 0 || args[0][0] == '!'){
        return lsh_history(args);
    }

    //Cicles through all the avaialble positions in the history array and points the most recent one
    cur_pos = (cur_pos + 1) % LSH_HIST_SIZE;
    //allocates the space in the history array to be the size of at least 64 characters
    history[cur_pos] = malloc(cur_bufsize * sizeof(char));
    //creates a pointer to the args array(the tokens)
    char **temp_args = args;

    //goes through command and It's params and stores them in the history current position
    while(*temp_args != NULL){
        strcat(history[cur_pos], *temp_args);
        strcat(history[cur_pos], " ");
        temp_args++;
    }

    //prints that a command has been inserted in the history array and the name of the command
    if(cur_pos > 0)
    printf("Inserted %s\n", history[cur_pos-1]);

    //goes through all the stored built in functions and if It matches It executes the function associate with It
    for (i = 0; i < lsh_num_builtins(); i++){
        if(strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }

    //launches the command with the arguments either concurrently or not
    return lsh_launch(args);
}

/* Read input from stdin */
char *lsh_read_line(void)
{
    //cur_bufsize stands for the terminal character of the line
    cur_bufsize = LSH_RL_BUFSIZE;
    //allocates a buffer of 64 characters to store the commands
    char *buffer = malloc(sizeof(char) * cur_bufsize);

    //if the is impossible to allocate, exit the program and print an error
    if(!buffer){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    //the current storing position of the buffer 
    int position = 0;
    //used to store the value per character of the user's input
    int c;

    while(1){
        /* Reads the next character of the user's input*/
        c = getchar();

        //if the character is either End of file or an enter store '\0' at the end of the buffer aand return It
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }

        //store at the current position in the buffer the character c
        buffer[position] = c;
        //increase position of the buffer to store the next character
        position++;

        /* If buffer exceeded, reallocate buffer */
        if(position >= cur_bufsize){
            cur_bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, cur_bufsize);
            if(!buffer){
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/* Loop for getting input and executing it */
void lsh_loop(void)
{
    //current line where the commands are getting stored
    char *line;
    //status of the command execution status
    int status;

    //Executes 
    do {
        //start of shell new line
        printf(">");
        //reads the commands and returns a char* variable
        line = lsh_read_line();
        //executes lines and returns status of command execution, returns 1 if done. 
        //returns 0 if the exit command is executed.
        status = lsh_execute(line);
        //Frees the memory from the line variable so It can store a new line of command
        free(line);
    } while(status);
}

int main(void)
{
    lsh_loop();

    return EXIT_SUCCESS;
}

