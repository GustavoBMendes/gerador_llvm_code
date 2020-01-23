/* Companion source code for "flex & bison", published by O'Reilly
 * Media, ISBN 978-0-596-15597-1
 * Copyright (c) 2009, Taughannock Networks. All rights reserved.
 * See the README file for license conditions and contact info.
 * $Header: /home/johnl/flnb/code/RCS/fb3-2funcs.c,v 2.1 2009/11/08 02:53:18 johnl Exp $
 */
/*
 * helper functions for fb3-2
 */
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include <string.h>
#  include <math.h>
#  include "func_bison.h"

extern FILE * yyin;

/* symbol table */
/* hash a symbol */
static unsigned symhash(char *sym){
  unsigned int hash = 0;
  unsigned c;

  while(c = *sym++) 
    hash = hash*9 ^ c;

  return hash;
}

struct symbol *lookup(char* sym){
  struct symbol *sp = &symtab[symhash(sym)%NHASH];
  int scount = NHASH;		/* how many have we looked at */

  while(--scount >= 0) {
    if(sp->name && !strcmp(sp->name, sym)) { return sp; }

    if(!sp->name) {		/* new entry */
      sp->name = strdup(sym);
      sp->value = 0;
      sp->func = NULL;
      sp->syms = NULL;
      return sp;
    }

    if(++sp >= symtab+NHASH) sp = symtab; /* try the next entry */
  }
  yyerror("symbol table overflow\n");
  abort(); /* tried them all, table is full */

}



struct ast *newast(int nodetype, struct ast *l, struct ast *r){
  struct ast *a = malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = nodetype;
  a->l = l;
  a->r = r;
  
  return a;
}

struct ast *newnum(double d, int t){
  struct numval *a = malloc(sizeof(struct numval));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'K';
  a->number = d;
  a->tipo = t;
  
  return (struct ast *)a;
}

struct ast *newcall(struct ast *l, struct ast *r){
  struct ast *a = malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'C';
  a->l = l;
  a->r = r;
  return a;
}

struct ast *newref(struct symbol *s){
  struct symref *a = malloc(sizeof(struct symref));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'N';
  a->s = s;
  return (struct ast *)a;
}

struct ast *newasgn(struct symbol *s, struct ast *v, int tipo){
  struct symasgn *a = malloc(sizeof(struct symasgn));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = '=';
  a->s = s;

  if(tipo == 1 && s->flag == 0){ //tipo int
    s->i = 1;
    s->f = 0;
    a->tipo = 1;
    v->i = 1;
    v->f = 0;
  }
  else if(tipo == 2 && s->flag == 0){ //tipo float
    s->i = 0;
    s->f = 1;
    a->tipo = 2;
    v->i = 0;
    v->f = 1;
  }
  else if(tipo == 1 && s->f == 1){
    yyerror("Variável float recebendo int");
  }
  else if(tipo == 2 && s->i == 1){
    yyerror("Variável int recebendo float");
  }

  s->flag = 1;
  a->v = v;

  return (struct ast *)a;
}

int verificaTipo(double op){
  int i;
  char str[20];
  sprintf(str, "%.4g", op);
  for(i = 0; i < 20; i++){
    if(str[i] == '.'){
      return 2; //tipo float 
    }
  }
  return 1; //tipo int 
}

//motor de execução
double eval(struct ast *a){
  double v;
  double op1;
  double op2;
  int i1;
  int i2;

  if(!a) {
    yyerror("internal error, null eval");
    return 0.0;
  }

  switch(a->nodetype) {
    /* constant */
  case 'K': v = ((struct numval *)a)->number; break;

    /* name reference */
  case 'N': v = ((struct symref *)a)->s->value; break;

    /* assignment */
  case '=': v = ((struct symasgn *)a)->s->value =
      eval(((struct symasgn *)a)->v); break;

    /* expressions */
  case '+':
    op1 = eval(a->l);
    op2 = eval(a->r);
    i1 = verificaTipo(op1);
    i2 = verificaTipo(op2);
    if(i1 == i2)
      v = op1 + op2; 
    else
      yyerror("Operação envolvendo dois tipos diferentes.");    
    break;

  case '-': 
    op1 = eval(a->l);
    op2 = eval(a->r);
    i1 = verificaTipo(op1);
    i2 = verificaTipo(op2);
    if(i1 == i2)
      v = op1 - op2; 
    else
      yyerror("Operação envolvendo dois tipos diferentes.");   
    break;

  case '*': 
    op1 = eval(a->l);
    op2 = eval(a->r);
    i1 = verificaTipo(op1);
    i2 = verificaTipo(op2);
    if(i1 == i2)
      v = op1 * op2; 
    else
      yyerror("Operação envolvendo dois tipos diferentes.");   
    break;

  case '/':
    op1 = eval(a->l);
    op2 = eval(a->r);
    i1 = verificaTipo(op1);
    i2 = verificaTipo(op2);
    if(i1 == i2)
      v = op1 / op2; 
    else
      yyerror("Operação envolvendo dois tipos diferentes.");    
    break;

  case '^': 
    op1 = eval(a->l);
    op2 = eval(a->r);
    i1 = verificaTipo(op1);
    i2 = verificaTipo(op2);
    if(i1 == i2)
      v = pow(op1, op2); 
    else
      yyerror("Operação envolvendo dois tipos diferentes.");   
    break;

  case 'C': v = eval(a->r); break;

  default: printf("internal error: bad node %c\n", a->nodetype);
  }
  return v;
}


void treefree(struct ast *a) {
  switch(a->nodetype) {

    /* two subtrees */
  case '+':
  case '-':
  case '*':
  case '/':
  case '^':
    treefree(a->r);

    /* no subtree */
  case 'K': case 'N':
    break;

  case '=': case 'C':
    free( ((struct symasgn *)a)->v);
    break;

  default: printf("internal error: free bad node %c\n", a->nodetype);
  }	  
  
  free(a); /* always free the node itself */

}

void yyerror(char *s, ...){
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");

  exit(0);  
}

//Analisador semântico
void dumpast(struct ast *a, int level){

  printf("%*s", 2*level, "");	/* indent to this level */
  level++;

  if(!a) {
    printf("NULL\n");
    return;
  }

  switch(a->nodetype) {
    /* constant */
  case 'K': printf("number %4.4g\n", ((struct numval *)a)->number); 
    break;

    /* name reference */
  case 'N': if(((struct symref *)a)->s->flag == 0) yyerror("Variável '%s' sem valor.", ((struct symref *)a)->s->name);
    printf("ref %s\n", ((struct symref *)a)->s->name); break;

    /* assignment */
  case '=': 
    printf("= %s\n", ((struct symref *)a)->s->name);
    dumpast( ((struct symasgn *)a)->v, level);
    return;

    /* expressions */
  case '+': case '-': case '*': case '/': case '^': case 'L':
    printf("binop %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;

  case 'C': printf("call print\n");
    dumpast(a->r, level);
    return;

  default: printf("bad %c\n", a->nodetype);
    return;
  }
}

int main(int argc, char **argv){

  if(argc > 1)
    yyin = fopen(argv[1], "r");

  else{
    printf("Forma de uso: ./front arquivo_entrada");
    return 0;
  }

  yyparse();
  return 0;

}