%option noyywrap
%{

	#include <stdio.h>
	#include "bison.tab.h"
	#include "func_bison.h"
	
%}

digitos		([-+]?[0-9])
identif 	[a-zA-Z][a-zA-Z0-9]*

%%	

"print"		{	
				return PRINT;
			}
{identif}	{	
				yylval.s = lookup(yytext);
				return ID;
			}
\n			{	
				return N_LINHA;			
			}
"("			{	
				return E_PAR;		
			}
")"			{	
				return D_PAR;		
			}
"+"			{	
				return MAIS;			
			}
"-"			{	
				return MENOS; 		
			}
"*"			{	
				return VEZES; 		
			}
"/"			{	
				return DIVIDE;		
			}
"^"			{	
				return POTENCIA;		
			}
";"			{	
				return PONTO_VIRGULA;		
			}
"="			{	
				return IGUAL;		
			}
{digitos}+	{ 	
				yylval.ival = atoi(yytext);
				return INTEIRO;
			}
{digitos}+"."{digitos}* {
				yylval.fval = atof(yytext);
				return REAL;
			}


%%
