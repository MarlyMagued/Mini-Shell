
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

"exit"	{
		return EXIT;
	}


"|"	{
		return PIPE;
	}

"<"	{
		return LESS;
	}
">"	{
		return GREAT;
	}

">>" 	{
		return APPEND;
	}
">>&"	{
		return APPEND_AMPERSAND;                	
	}

">&"	{
		return GREAT_AMPERSAND;         
	}

"&"	{
		return AMPERSAND;
	}

\n	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	


[^ \t\n|<>][^ \t\n|<>]*  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}

.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
