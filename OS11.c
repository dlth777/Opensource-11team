/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Thursday,  8 January 2015

  @brief        LSH (Libstephen SHell)

*******************************************************************************/


#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <utime.h>

#define SIZE 50


/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int mypwd(char **args);
int myuname(char **args);
int myls(char **args);
int mybc(char **args);
int mymkdir(char *args[]);
int myrm(char *args[]);
int mytouch(char *args[]);
int mymv(char *args[]);
int mycp(char *args[]);


/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "pwd",
  "uname",
  "bc",
  "mkdir",
  "rm",
  "touch",
  "mv",
  "cp"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  mypwd,
  myuname,
  mybc,
  &mymkdir,
  &myrm,
  &mytouch,
  &mymv,
  &mycp
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int mypwd(char **args)
{
        char buffer[100];
        printf("testpwd\n");
        if(getcwd(buffer,sizeof(buffer)))//Get current working directory and store it in buffer.
        {
                printf("%s\n",buffer);
        }
        else
        {
                perror("getcwd");//Display the error occurred with getcwd.
        }

        return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

int myuname(char **args)
{
        struct utsname buf;
        printf("testuname\n");
        if(!uname(&buf))//Get name and information about current kernel.

                printf("%s\n",buf.sysname);//Display the system name.
        else
                perror("uname");//Display the error occurred while retrieving information about current kernel.

        return 1;
}
int top=-1;
char s[SIZE],sufix[SIZE];

int prior(char ch) //This function returns the priority of the operator.
{
        if(ch=='('||ch=='#')
                return 1;
        if(ch=='+'||ch=='-')
                return 2;
        if(ch=='*'||ch=='/')
                return 3;
        if(ch=='$'||ch=='^')
                return 4;
}

char pop() //Pops an element from the stack pointed to by top.
{
        char element;

        if(top==-1)
        {
                printf("\nInvalid\n");
        }
        element=s[top];
        top--;
        return element;
}

void push(char element) //Pushes an element onto the stack.
{
        if(top==(SIZE-1))
        {
                printf("\nInvalid\n");
                return;
        }
        top++;
        s[top]=element;
}

void postfixEvaluation() //Evaluates the postfix expression.
{
        int i=0,result,op1,op2;
        while(sufix[i]!='\0') //Loop over the expression until the NULL Character(\0) is found.
        {
                if(isdigit(sufix[i])) //Check if the current symbol is a digit.
                        push(sufix[i]-'0'); //Push the digit(after conversion from character to integer) onto the stack.
                else //If the current symbol is an operator.
                {
                        op1=pop(); //Pop two elements from the stack.
                        op2=pop();
                        if(op2 != 35)
                        {
                                switch(sufix[i]) //Check which operation is to be performed.
                                {
                                case '*' : result=op2*op1; //If the current symbol is *, then multiply op1 and op2.
                                           push(result);
                                           break;
                                case '+' : result=op2+op1; //If the current symbol is +, then add op1 and op2.
                                           push(result);
                                           break;
                                case '/' : result=op2/op1; //If the current symbol is /, then divide op2 by op1.
                                           push(result);
                                           break;
                                case '-' : result=op2-op1; //If the current symbol is -, then subtract op1 from op2.
                                           push(result);
                                           break;
                                case '%' : result=op2%op1; //If the current symbol is %, then find the remainder of op2/op1.
                                           push(result);
                                           break;
                                default : printf("Invalid Operator\n");
                                }
                        }
                        else
                        {
                                printf("Invalid Expression\n");
                                return ;
                        }
                }
                i++;
        }
                printf("%d\n",result);
}

int mybc(char **args)
{
        char infix[SIZE];
        int i,j=0;
        push('#');
        gets(infix); //Scan the infix expression

        while(strcmp(infix,"quit")) //Scan for infix expressions and convert them to postfix expressions until the user wants to quit.
        {
                //Convert the infix expression to postfix.
                for(i=0;infix[i]!='\0';i++)
                {
                        if(isalnum(infix[i])) //If the current symbol is a digit or an alphabet, then add it to the postfix expression.
                                sufix[j++]=infix[i];
                        else if(infix[i]=='(') //If the current symbol is a (, then push it onto the stack.
                                push(infix[i]);
                        else if(infix[i]==')') //If the current symbol os a ), then pop all the elements from the stack and add them to the postfix until ( is found.
                        {
                                while(s[top]!='(')
                                        sufix[j++]=pop();
                                pop();
                        }
                        else //If the current symbol is an operator, then pop all the elements from the stack which have a priority greater than the the current symbol.
                        {
                                while(prior(s[top])>=prior(infix[i]))
                                        sufix[j++]=pop();
                                push(infix[i]);
                        }
                }
                while(s[top]!='#') //Pop all the elements from the stack.
                        sufix[j++]=pop();
                sufix[j]='\0';

                postfixEvaluation(); //Evaluate the postfix expression.
                j=0;
                top=-1;
                push('#');
                gets(infix);
        }
        return 1;
}




int mymkdir(char *argv[])
{

        if(!strcmp(argv[0],"mkdir"))
        {
                if(!mkdir(argv[1],0775))
                printf("%s was created\n",argv[1]);
        }
        else
                perror("mkdir");

        return 1;
}
int myrm(char *argv[])
{
        if(!strcmp(argv[0],"rm"))
        {
                if(!access(argv[1],F_OK))
                {
                        if(unlink(argv[1]))
                        {
                                perror("unlink");
                        }
                }
                else
                {
                        perror("access");
                }
        }
        return 1;
}
int mytouch(char *argv[])
{
        int fd;
        if(!strcmp(argv[0],"touch"))
        {
                if(!access(argv[1],F_OK))
                {
                        if( utime(argv[1],NULL)==0 )
                                printf("Access and Modification time for %s was changed\n",argv[1]);
                        else
                                perror("utime");
                }

                else
                {
                        fd=open(argv[1],O_CREAT|O_RDWR,0777);
                        if( fd > 0 )
                                printf("%s was created\n",argv[1]);
                        else
                                perror("open");
                }
        }
        return 1;
}
int mymv(char *argv[])
{
        if(access(argv[2],F_OK)==0)
        {
                printf("File %s exists",argv[2]);
                return ;
        }else
        {
                if(!link(argv[1],argv[2]))
                {
                        if(unlink(argv[1]))
                                perror("unlink");
                }else
                        perror("link");
        }
        return 1;
}
int mycp(char *argv[])
{
        int fd1,fd2;
        char *buffer;
        int count;

        if(!access(argv[1],F_OK))
        {
                fd1=open(argv[1],O_RDONLY);
                if(fd1 > 0)
                {
                        fd2=open(argv[2],O_CREAT|O_WRONLY,0777);
                        if(fd2 > 0)
                        {
                                while((count=read(fd1,&buffer,sizeof(buffer)))>0)
                                        write(fd2,&buffer,count);

                                close(fd2);
                                close(fd1);
                        }
                        else
                        {
                                perror("open");
                        }
                }
                else
                {
                        perror("open");
                }
        }
        else
        {
                perror("access");
        }

        return 1;
}




/*
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
#ifdef LSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define LSH_RL_BUFSIZE 1024
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
                free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

