

//
// tsh - A tiny shell program with job control
//
// Chakrya Ros (chro0474 )
// Partner: Muntaha Pasha (mupa044)
//

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>

#include "globals.h"
#include "jobs.h"
#include "helper-routines.h"

//
// Needed global variable definitions
//

static char prompt[] = "tsh> ";
int verbose = 0;

//
// You need to implement the functions eval, builtin_cmd, do_bgfg,
// waitfg, sigchld_handler, sigstp_handler, sigint_handler
//
// The code below provides the "prototypes" for those functions
// so that earlier code can refer to them. You need to fill in the
// function bodies below.
//
pid_t Fork(void);
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

//
// main - The shell's main routine
//
int main(int argc, char **argv)
{
  int emit_prompt = 1; // emit prompt (default)

  //
  // Redirect stderr to stdout (so that driver will get all output
  // on the pipe connected to stdout)
  //
  dup2(1, 2);

  /* Parse the command line */
  char c;
  while ((c = getopt(argc, argv, "hvp")) != EOF) {
    switch (c) {
    case 'h':             // print help message
      usage();
      break;
    case 'v':             // emit additional diagnostic info
      verbose = 1;
      break;
    case 'p':             // don't print a prompt
      emit_prompt = 0;  // handy for automatic testing
      break;
    default:
      usage();
    }
  }

  //
  // Install the signal handlers
  //

  //
  // These are the ones you will need to implement
  //
  Signal(SIGINT,  sigint_handler);   // ctrl-c
  Signal(SIGTSTP, sigtstp_handler);  // ctrl-z
  Signal(SIGCHLD, sigchld_handler);  // Terminated or stopped child

  //
  // This one provides a clean way to kill the shell
  //
  Signal(SIGQUIT, sigquit_handler);

  //
  // Initialize the job list
  //
  initjobs(jobs);

  //
  // Execute the shell's read/eval loop
  //
  for(;;) {
    //
    // Read command line
    //
    if (emit_prompt) {
      printf("%s", prompt);
      fflush(stdout);
    }

    char cmdline[MAXLINE];

    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      app_error("fgets error");
    }
    //
    // End of file? (did user type ctrl-d?)
    //
    if (feof(stdin)) {
      fflush(stdout);
      exit(0);
    }

    //
    // Evaluate command line
    //
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
  }

  exit(0); //control never reaches here
}

/////////////////////////////////////////////////////////////////////////////
//
// Fork - checks fork for errors
//system call error handling
//fork function is called onece but return twice
//once in the calling process(parent), and once in newly create child process

pid_t Fork(void){
    //Fork is responsible for creating the child process. Child is initially copy of parent, but can be used to run a separate program. When you fork, child and parent run parallel
    pid_t child;
    
    if((child = fork()) < 0){
        //Checking if the forking was incorrectly done
        unix_error("Fork error");
    }
    return child; //return 0 to child
    //PID of child to parent, or  -1 on error
}
/////////////////////////////////////////////////////////////////////////////
//
// eval - Evaluate the command line that the user has just typed in
//
// If the user has requested a built-in command (quit, jobs, bg or fg)
// then execute it immediately. Otherwise, fork a child process and
// run the job in the context of the child. If the job is running in
// the foreground, wait for it to terminate and then return.  Note:
// each child process must have a unique process group ID so that our
// background children don't receive SIGINT (SIGTSTP) from the kernel
// when we type ctrl-c (ctrl-z) at the keyboard.
//



void eval(char *cmdline)
{
    /* Parse command line */
    //
    // The 'argv' vector is filled in by the parseline
    // routine below. It provides the arguments needed
    // for the execve() routine, which you'll need to
    // use below to launch a process.
    //
    //execuv() load a new program
    char *argv[MAXARGS]; //max of agrument I can have for array
    
    //
    // The 'bg' variable is TRUE if the job should run
    // in background mode or FALSE if it should run in FG
    //
    int bg = parseline(cmdline, argv); //call parseline function
    if (argv[0] == NULL){
        return;   /* ignore empty lines */
    }
    
    pid_t pid;
    
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    
    
    if(!builtin_cmd(argv)){
        
        //Blocks SIGCHLD from triggering
        sigprocmask(SIG_BLOCK, &mask, NULL);
        
        //Create Child to run job
        if ((pid = Fork()) == 0){
            
            //Set child group pid to parent group pid
            if(setpgid(0,0) < 0){
                unix_error("setpgid error");
            }
            
            //unblock child
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            
            // test if command is found
            if(execve(argv[0], argv, environ) < 0){
                printf("%s: Command not found\n", argv[0]);
                exit(0); //if not found, terminate
            }
        }
        //PARENT FOREGROUND PROCESSES
        if(!bg){
            //make sure there is a job to kill
            if (!addjob(jobs, pid, FG, cmdline)){
                kill(-pid, SIGINT);
                return;
            }
            //unblock Parent
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            waitfg(pid);
        }
        
        //PARENT BACKGROUND PROCESSES
        else{
            //make sure there is a job to kill
            if (!addjob(jobs, pid, BG, cmdline)){
                kill(-pid, SIGINT);
                return;
            }
            //unblock Parent
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            //print job info
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
        }
    }
    
    
    return;
}


/////////////////////////////////////////////////////////////////////////////
//
// builtin_cmd - If the user has typed a built-in command then execute
// it immediately. The command name would be in argv[0] and
// is a C string. We've cast this to a C++ string type to simplify
// string comparisons; however, the do_bgfg routine will need
// to use the argv array as well to look for a job number.
//
int builtin_cmd(char **argv)
{
    string cmd(argv[0]);
    // Exit tsh if cmd is quit
    if (cmd == "quit" or cmd=="exit") {
        exit(0);
    }
    
    // Run Jobs if cmd is jobs
    if (cmd == "jobs"){
        listjobs(jobs);
        return 1;
    }
    
    if (cmd == "bg" or cmd =="fg"){
        do_bgfg(argv);
        return 1;
    }
    
    // Ignoring singleton &
    if (cmd == "&"){
        return 1;
    }
    
    return 0;     /* not a builtin command */
}

/////////////////////////////////////////////////////////////////////////////
//
// do_bgfg - Execute the builtin bg and fg commands
//
void do_bgfg(char **argv)
{
    struct job_t *jobp=NULL;
    
    /* Ignore command if no argument */
    if (argv[1] == NULL) {
        //Make sure you have both arguments, for example when you run ./myspin 1 &, it has to have the 1, and the &, you can't only type ./myspin
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    
    /* Parse the required PID or %JID arg */
    if (isdigit(argv[1][0])) {
        pid_t pid = atoi(argv[1]);
        if (!(jobp = getjobpid(jobs, pid))) {
            printf("(%d): No such process\n", pid);
            return;
        }
    }
    else if (argv[1][0] == '%') {
        int jid = atoi(&argv[1][1]);
        if (!(jobp = getjobjid(jobs, jid))) {
            printf("%s: No such job\n", argv[1]);
            return;
        }
    }
    else {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }
    string cmd(argv[0]);
    
    //Continue the job
    if(!kill(-jobp->pid, SIGCONT)){
        //printf("Job [%d] (%d) continued\n", jid, pid);
    }
    
    //If foreground, then change status and wait
    if(cmd == "fg"){
        jobp->state = FG;
        waitfg(jobp->pid);
    }
    
    //If background, then change status, print, and return
    if(cmd == "bg"){
        jobp->state = BG;
        printf("[%d] (%d) %s", pid2jid(jobp->pid), jobp->pid, jobp->cmdline);
    }
    
    return;
}

/////////////////////////////////////////////////////////////////////////////
//
// waitfg - Block until process pid is no longer the foreground process
//
void waitfg(pid_t pid)
{
    // Wait for there to be no foreground job
    while(pid == fgpid(jobs)){
        ;
    }
    
    return;
}

/////////////////////////////////////////////////////////////////////////////
//
// Signal handlers
//


/////////////////////////////////////////////////////////////////////////////
//
// sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
//     a child job terminates (becomes a zombie), or stops because it
//     received a SIGSTOP or SIGTSTP signal. The handler reaps all
//     available zombie children, but doesn't wait for any other
//     currently running children to terminate.
//
void sigchld_handler(int sig) //notified if child dies
{
    pid_t wpid;
    int status;
    // WNOHANG means not interested in waiting around if not functions are stopped
    // WUNTRACED get unreproted stopped children (Gets missing child to be reaped)
    
    
    //Will reap all zombie children //W|W doesnt hang for stopped jobs
    while((wpid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0){
        //Check if it was signalled (sigint)
        if(WIFSIGNALED(status)){
            printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(wpid), wpid, WTERMSIG(status));
        }
        //Check if it was stopped (sigstp)
        else if(WIFSTOPPED(status)){
            printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(wpid), wpid, WSTOPSIG(status));
            getjobpid(jobs, wpid)->state = ST;
            return;
        }
        // as long as it is a terminated job, this should trigger!
        deletejob(jobs, wpid);
    }
    //if successful, return
    if (errno == 0){
        return;
    }
    //if there was an error, print the error and exit the program
    if (errno != ECHILD){
        unix_error("waitpid error");
    }
    return;
}

/////////////////////////////////////////////////////////////////////////////
//
// sigint_handler - The kernel sends a SIGINT to the shell whenver the
//    user types ctrl-c at the keyboard.  Catch it and send it along
//    to the foreground job.
//
void sigint_handler(int sig)
{
    //get current foreground process
    pid_t pid = fgpid(jobs);
    
    //stops current foreground process
    if (pid != 0){
        kill(-pid, SIGINT);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
//     the user types ctrl-z at the keyboard. Catch it and suspend the
//     foreground job by sending it a SIGTSTP.
//
void sigtstp_handler(int sig)
{
    //get current foreground process
    pid_t pid = fgpid(jobs);
    //ends current foreground process
    if (pid != 0){
        kill(-pid, SIGTSTP);
    }
    
    return;
}

/*********************
 * End signal handlers
 *********************/

