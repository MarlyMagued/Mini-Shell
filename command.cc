
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"
#include <time.h>
#include <string.h>


void child_handler(int child_num);
SimpleCommand::SimpleCommand()
{
	// Create available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

    
    if ( _errFile != _outFile) {
        //printf("%s",_errFile);
        free( _errFile );
    }
    
    if ( _inputFile ) {
        free( _inputFile );
    }

	if ( _outFile ) {
        //printf("output %s\n",_outFile);
		free( _outFile );
	}


	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
    _append = 0;
    _pipe = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
        printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

void
Command::execute()
{
    int defaultin = dup( 0 );
    int defaultout = dup( 1 );
    int defaulterr = dup( 2 );
    int inputFD;
    int outputFD ;
    int errorFD;
    
    if (_exit){
        printf("\n              Good bye!!\n\n");
        exit(0);
    }
    
    // Don't do anything if there are no simple commands
    if ( _numberOfSimpleCommands == 0 ) {
        prompt();
        return;
    }
    
    // Print contents of Command data structure
    print();
    
    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec
    
    //ITERATE THROUGH SIMPLE COMMANDS
    for( int i = 0 ; i < _numberOfSimpleCommands ; i++)
    {
       
        int fdpipe[2];
        
        //INPUT COMMING FROM LAST COMMAND
        dup2(inputFD,0);
        close(inputFD);
        
        //INITIATING PIPE
        if ( i < _numberOfSimpleCommands - 1){
            if ( pipe(fdpipe) == -1) {
                printf("error creating a pipe\n");
                exit( 2 );
            }
            
            inputFD = fdpipe[0];
            outputFD = fdpipe[1];
            //printf("pipe initiated\n");
        }
        
       
        //CD COMMAND
        if (! strcmp(_simpleCommands[i]->_arguments[0],"cd") )
        {
            //cd to home
            if( ! _simpleCommands[i]->_arguments[1] ){
                
                chdir(getenv("HOME"));
                printf("    directory changed to home\n");
                
            }
            else
            
            {
                
                //cd any dother directory
                int flag = chdir(_simpleCommands[0]->_arguments[1]);
                if (flag == -1 )
                    printf("No such directory");
                printf("        directory changed to %s\n",_simpleCommands[0]->_arguments[1]);
                
            }
            
        }
        
        //ANY OTHER COMMAND
        else
        {
            
            //EXCUTE LAST COMMAND IN PIPE NORMALY
            if (i  ==  _numberOfSimpleCommands - 1)
            {
                if (_inputFile)
                {
                    
                    inputFD = open(_inputFile, O_RDONLY );
                    
                    if (inputFD < 0) 
                    {
                        perror("Cannot open input file");
                        exit(1);
                    }
                    
                    dup2(inputFD,0);
                    close(inputFD);
                    
                }
                else
                {
                    inputFD = defaultin ;
                }
                
                if (_outFile) 
                {
                    
                    
                    if (_append)
                    {
                        outputFD = open(_outFile,O_WRONLY | O_CREAT | O_APPEND, 0666);
                    }
                    
                    else
                    {
                        outputFD = creat(_outFile, 0666 );
                    }
                    
                    if (outputFD < 0) 
                    {
                        perror("Cannot open output file");
                        exit(1);
                    }
                    

                }
                else
                {
                    outputFD = defaultout;
                }
                
                if (_errFile) 
                {
                    
                    errorFD = open(_outFile,O_WRONLY | O_CREAT | O_APPEND, 0666);
                    
                    if (errorFD < 0) 
                    {
                        perror("Cannot open error file");
                        exit(1);
                    }
                    
                    dup2(errorFD, 2);  // Redirect stderr
                    close(errorFD);
                }
                
            }
            
            dup2(outputFD,1);
            close(outputFD);
            
            
            //FORKING CHILD PROCESS
            int pid = fork();
            if ( pid == -1 ) 
            {
                printf("error forking process\n");
                exit( 2 );
            }
            
            else if (pid == 0) 
            {
               
                close( defaultin );
                close( defaultout );
                close( defaulterr );
                
                execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
                printf("failure of excution");
                exit(1);
            }
            
            
            
            //PARENT PROCESS
            else 
            {
                signal(SIGCHLD, child_handler); //children processes logs
                
                if(!_background)
                {
                    waitpid(pid,0, 0);
                }
                
            }
        }
    }
    // Restore default file descriptors
    dup2(defaultin, 0);
    dup2(defaultout, 1);
    dup2(defaulterr, 2);
    
    // Close file descriptors
    close(defaultin);
    close(defaultout);
    close(defaulterr);
    
    
        // Clear to prepare for next command
        clear();
        
        // Print new prompt
        prompt();
        

}
// Shell implementation


void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

void child_handler(int child_num){
    FILE *logs = fopen("logfile.txt", "a");
    
    time_t now = time(NULL);
    
    fprintf(logs," child number %d terminated at: %s",child_num,ctime(&now));
    fclose(logs);

}

int yyparse();


int main() {
    
    signal(SIGINT, SIG_IGN);

   
    Command::_currentCommand.prompt();
                yyparse();
            
   

    }



