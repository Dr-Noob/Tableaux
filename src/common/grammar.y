%{
#include <stdio.h>
#include <string.h>
#include "Formula.h"
#include "Global.h"
#include "Controlador.h"
int yylex (void);
void yyerror (FILE *fich, const char *msg);
extern int yylineno;
/*Para debug*/
//#define YYDEBUG 1
//int yydebug = -1;

%}

%union {
	char a;
	Formula formula;
}

%locations
%parse-param {FILE *fich}
%token LPAREN RPAREN AND OR NOT IMP DIMP COMA END_OF_FILE
%token <a> ATOM

%type <formula> e
%type <formula> z
%type <formula> i
%type <formula> c
%type <formula> d
%type <formula> n
%type <formula> g

%%

oracion : e END_OF_FILE
		{
			printMsgGreen(MESSAGE_EXPRESION_CORRECTA);
			ResolverTableaux($1,fich);
			return 0;
		}
	| error END_OF_FILE
		{
			printMsgRed(MESSAGE_ERROR_SEMANTICO,@1.first_column,@1.last_column);
			YYABORT;
		};

e: e COMA z
		{
			$$=Concatenar($1,$3);
		}
	| z
		{
			$$=$1;
		};

z: z DIMP i
		{
			$$ = Unir($1,COD_DIMP,$3);
		}
	| i
		{
			$$=$1;
		};

i: c
		{
			$$=$1;
		}
	| i IMP c
		{
			$$ = Unir($1,COD_IMP,$3);
		};

c: d
		{
			$$=$1;
		}
	| c OR d
		{
			$$ = Unir($1,COD_OR,$3);
		};

d: n
		{
			$$=$1;
		}
	| d AND n
		{
			$$ = Unir($1,COD_AND,$3);
		};

n: NOT n
		{
			$$ = NegarFormula($2);
		}
	| g
		{
			$$=$1;
		};
g: LPAREN e RPAREN
		{
			$$=$2;
		}
	| ATOM
		{
			Atomo a = CrearAtomo($1,SIN_NEGAR);
			$$=CrearFormula(a);
			LiberarAtomo(a);
		};

%%

void yyerror (FILE *fich, const char *msg) {
	//printMsgRed(MESSAGE_ERROR_SEMANTICO);
}