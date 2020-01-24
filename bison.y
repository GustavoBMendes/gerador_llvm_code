%{

    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include "func_bison.h"
    extern int yylex(void);
    extern int yyparse();
    extern FILE* yyin;

%}

%union{
    struct ast *a;
    int     ival;
    double  fval;
    struct symbol *s
}

%token <ival> INTEIRO
%token <fval> REAL
%token <s> ID
%token <a> PRINT
%token MAIS MENOS VEZES DIVIDE POTENCIA VIRGULA IGUAL E_PAR D_PAR
%token N_LINHA
%token PONTO_VIRGULA
%right IGUAL
%left MAIS MENOS
%left VEZES DIVIDE
%precedence POTENCIA

%type<a> iexp line

%start entrada

%%
    entrada: 
            | entrada line PONTO_VIRGULA N_LINHA        {
                                                    dumpast($2, 0);
                                                    code_llvm($2);
                                                    printf("= %4.4g\n> ", eval($2));
                                                    yylineno++;
                                                }

            | entrada N_LINHA {}

    ;

    line: 
        ID IGUAL iexp   {
                            if(ti == 1)     $$ = newasgn($1, $3, 1);
                            if(tf == 1)     $$ = newasgn($1, $3, 2);
                        }
        | PRINT iexp    {$$ = newcall($1, $2); }
    ;

    iexp: 
        iexp POTENCIA iexp    {$$ = newast('^', $1, $3); }
        | iexp VEZES iexp       {$$ = newast('*', $1, $3); }
        | iexp DIVIDE iexp      {$$ = newast('/', $1, $3); }
        | iexp MAIS iexp        {$$ = newast('+', $1, $3); }
        | iexp MENOS iexp       {$$ = newast('-', $1, $3); }
        | INTEIRO               {$$ = newnum($1, 1); ti = 1; tf = 0;} 
        | REAL                  {$$ = newnum($1, 2); ti = 0; tf = 1;} 
        | ID                    {$$ = newref($1); }
        | E_PAR iexp D_PAR      {$$ = $2; }
        
    ;
    
%%
