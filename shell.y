
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT LESS NEWLINE

%token AMPERSAND PIPE EXIT

%token APPEND APPEND_AMPERSAND GREAT_AMPERSAND



%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"

%}

%%

goal:
    commands
    ;

commands:
    command
    | commands command
	
    ;

command:
         simple_command
        ;

simple_command:
    pipeline iomodifier_opt background NEWLINE {
        printf("   Yacc: Execute command\n");
        Command::_currentCommand.execute();
    }
    | EXIT{
        Command::_currentCommand._exit=1;
        Command::_currentCommand.execute();
    }
    | NEWLINE{
	 Command::_currentCommand.execute();
	}
    | error NEWLINE { yyerrok; }
    ;
    
pipeline:
    pipeline PIPE command_and_args {
	Command::_currentCommand._pipe = 1;
	}
    | command_and_args
    ;
                

command_and_args:
	 command_word arg_list {
		Command::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand );
	}
	
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
            printf("   Yacc: insert argument \"%s\"\n", $1);
            
	       Command::_currentSimpleCommand->insertArgument( $1 );

	}
	;

command_word:
	WORD {
            printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 0; 
	}
	| APPEND WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 1;
	}
	| LESS WORD{
	printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;

	}
	| APPEND_AMPERSAND WORD {
		printf("   Yacc: append output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._append = 1;
	}
	| GREAT_AMPERSAND WORD {
		printf("   Yacc: append output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._append = 0;

	}
	| iomodifier_opt LESS WORD
    	{
        printf("   Yacc: insert input \"%s\"\n", $3);
        Command::_currentCommand._inputFile = $3;
   	 }
    	| iomodifier_opt GREAT WORD
    	{
        printf("   Yacc: insert output \"%s\"\n", $3);
        Command::_currentCommand._outFile = $3;
	Command::_currentCommand._append = 0;

     
    	}
    	| iomodifier_opt APPEND WORD
    	{
        printf("   Yacc: append output \"%s\"\n", $3);
        Command::_currentCommand._outFile = $3;
        Command::_currentCommand._append = 1;
    	}
	|/* can be empty */ 
	
	;
    
background:
    AMPERSAND{
        Command::_currentCommand._background = 1;
    }
    | //nothing
    ;

%%
void
yyerror(const char * s)
    {
        fprintf(stderr,"%s", s);
    }

#if 0
main()
{
    yyparse();
}
#endif
