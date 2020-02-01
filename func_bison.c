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
  if(t == 1)
    a->strtype = "i32";
  if(t == 2)
    a->strtype = "float";
  
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
    s->type = "i32";
    a->tipo = 1;
    v->i = 1;
    v->f = 0;
  }
  else if(tipo == 2 && s->flag == 0){ //tipo float
    s->i = 0;
    s->f = 1;
    s->type = "float";
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
  s->alloc = 0;
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

  fprintf(tree_arq, "%*s", 2*level, "");	/* indent to this level */
  level++;

  if(!a) {
    printf("NULL\n");
    return;
  }

  switch(a->nodetype) {
    /* constant */
  case 'K': fprintf(tree_arq, "number %4.4g\n", ((struct numval *)a)->number); 
    break;

    /* name reference */
  case 'N': if(((struct symref *)a)->s->flag == 0) yyerror("Variável '%s' sem valor.", ((struct symref *)a)->s->name);
    fprintf(tree_arq, "ref %s\n", ((struct symref *)a)->s->name); break;

    /* assignment */
  case '=': 
    fprintf(tree_arq, "= %s\n", ((struct symref *)a)->s->name);
    dumpast( ((struct symasgn *)a)->v, level);
    return;

    /* expressions */
  case '+': case '-': case '*': case '/': case '^':
    fprintf(tree_arq, "binop %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;

  case 'C': fprintf(tree_arq,"call print\n");
    dumpast(a->r, level);
    return;

  default: fprintf(tree_arq, "bad %c\n", a->nodetype);
    return;
  }
}

int cont_stmt = 0;
char* type;
int ref_op = 0;
int iprint = 0;
int decl_pow = 0;
int arqtree = 0;

void gera_llvm(struct ast *a){
  
  if(!a){
    printf("NULL\n");
    return;
  }

  switch(a->nodetype) {
    // assignment 
    case '=':
      if(((struct symref *)a)->s->alloc == 0){
          fprintf(arq, "\t\%%\%d = alloca %s, align 4\n", cont_stmt, ((struct symref *)a)->s->type);

        ((struct symref *)a)->s->alloc = 1;
        ((struct symref *)a)->s->ref = cont_stmt;
        a->ref = cont_stmt;
        cont_stmt++;
      }
      
      double val = ((struct symasgn *)a)->s->value = atribui(((struct symasgn *)a)->v, ((struct symref *)a)->s->type);
      fprintf(arq, "\tstore %s %.4g, %s* \%%\%d, align 4\n", 
      ((struct symref *)a)->s->type, val, ((struct symref *)a)->s->type, a->ref);

      return;

    //print
    case 'C': 
      case_print(a->r);
      fprintf(arq, "\t\%%\%d call %s (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), %s \%%\%d)\n"
      , cont_stmt, type, type, cont_stmt-1);
      cont_stmt++;
      iprint = 1;
      return;

    default: printf("bad %c\n", a->nodetype);
      return;
  }
  return;
}

double case_print(struct ast *a){

  double op1, op2, v;

  if(!a){
    printf("NULL\n");
    return 0.0;
  }

  switch(a->nodetype){
    case 'K':
      v = ((struct numval *)a)->number;
      type  = ((struct numval *)a)->strtype;
      break;

    case 'N':
      type = ((struct symref *)a)->s->type;
      fprintf(arq, "\t\%%\%d = load %s, %s* \%%\%d, align 4\n", cont_stmt, type, type, ((struct symref *)a)->s->ref);
      cont_stmt++;
      v = ((struct symref *)a)->s->value;
      break;

    case '+': 
      op1 = case_print(a->l);
      op2 = case_print(a->r);
      fprintf(arq, "\t\%%\%d = add nsw %s %.4g, %.4g\n", cont_stmt, type, op1, op2);
      v = op1 + op2;
      cont_stmt++;
      break;
    
    case '-': 
      op1 = case_print(a->l);
      op2 = case_print(a->r);
      v = op1 - op2;
      fprintf(arq, "\t\%%\%d = sub nsw %s %.4g, %.4g\n", cont_stmt, type, op1, op2);
      cont_stmt++;
      break;

    case '*': 
      op1 = case_print(a->l);
      op2 = case_print(a->r);
      v = op1 * op2;
      fprintf(arq, "\t\%%\%d = mul nsw %s %.4g, %.4g\n", cont_stmt, type, op1, op2);
      cont_stmt++;
      break;
    
    case '/': 
      op1 = case_print(a->l);
      op2 = case_print(a->r);
      v = op1 / op2;
      fprintf(arq, "\t\%%\%d = div nsw %s %.4g, %.4g\n", cont_stmt, type, op1, op2);
      cont_stmt++;
      break;
    
    case '^':
      op1 = case_print(a->l);
      op2 = case_print(a->r);
      v = pow(op1, op2);
      fprintf(arq, "\t\%%\%d = call %s @pow(float %.4g, float %.4g)\n", cont_stmt, type, op1, op2);
      cont_stmt++;
      decl_pow = 1;
      break;

    default: 
      printf("bad %c\n", a->nodetype);
      break;
  }
  return v;
}

double atribui(struct ast *a, char* typ){
  double v;
  double op1;
  double op2;

  if(!a){
    printf("NULL\n");
    return 0.0;
  }

  switch(a->nodetype) {
    // constant 
    case 'K':
      //fprintf(arq, "%4.4g", ((struct numval *)a)->number);
      v = ((struct numval *)a)->number;
      break;

      // name reference 
    case 'N':
      fprintf(arq, "\t\%%\%d = load %s, %s* \%%\%d, align 4\n", cont_stmt, typ, typ, ((struct symref *)a)->s->ref);
      cont_stmt++;
      v = ((struct symref *)a)->s->value;
      break;

    case '+': 
      op1 = atribui(a->l, typ);
      op2 = atribui(a->r, typ);
      v = op1 + op2;
      fprintf(arq, "\t\%%\%d = add nsw %s %.4g, %.4g\n", cont_stmt, typ, op1, op2);
      cont_stmt++;
      break;
    
    case '-': 
      op1 = atribui(a->l, typ);
      op2 = atribui(a->r, typ);
      v = op1 - op2;
      fprintf(arq, "\t\%%\%d = sub nsw %s %.4g, %.4g\n", cont_stmt, typ, op1, op2);
      cont_stmt++;
      break;

    case '*': 
      op1 = atribui(a->l, typ);
      op2 = atribui(a->r, typ);
      v = op1 * op2;
      fprintf(arq, "\t\%%\%d = mul nsw %s %.4g, %.4g\n", cont_stmt, typ, op1, op2);
      cont_stmt++;
      break;
    
    case '/': 
      op1 = atribui(a->l, typ);
      op2 = atribui(a->r, typ);
      v = op1 / op2;
      fprintf(arq, "\t\%%\%d = div nsw %s %.4g, %.4g\n", cont_stmt, typ, op1, op2);
      cont_stmt++;
      break;
    
    case '^':
      op1 = atribui(a->l, typ);
      op2 = atribui(a->r, typ);
      v = pow(op1, op2);
      fprintf(arq, "\t\%%\%d = call %s @pow(float %.4g, float %.4g)\n", cont_stmt, typ, op1, op2);
      cont_stmt++;
      decl_pow = 1;
      break;

    default: printf("bad %c\n", a->nodetype);
      break;
  }
  return v;
}

int main(int argc, char **argv){

  char* nome_arq_entrada;

  if(argc > 1){
    if(strcmp(argv[1], "-a") == 0){
      yyin = fopen(argv[5], "r");
      nome_arq_entrada = argv[5];
      arq = fopen(argv[4], "w");
      tree_arq = fopen(argv[2], "w");
      arqtree = 1;
    }
    else if(strcmp(argv[1], "-h") == 0){
      printf("Forma de uso: ./front -a teste.tree -o teste teste.calc\n");
      printf("Onde: -a <YYYY> imprime a árvore sintática abstrata (opcional)\n");
      printf("-o <XXXX> nome do arquivo de saída\n");
      printf("-h ajuda\n");
      return 0;
    }
    else{
      yyin = fopen(argv[3], "r");
      nome_arq_entrada = argv[3];
      arq = fopen(argv[2], "w");
      arqtree = 0;
    }
  }

  else{
    printf("Forma de uso: ./front -a teste.tree -o teste teste.calc\n");
    printf("Onde: -a <YYYY> imprime a árvore sintática abstrata (opcional)\n");
    printf("-o <XXXX> nome do arquivo de saída\n");
    printf("-h ajuda");
    return 0;
  }
  char *txt = "%d %d\00";
  fprintf(arq, "source_filename: '%s'\n", nome_arq_entrada);
  fprintf(arq, "target datalayout = 'e-m:e-i64:64-f80:128-n8:16:32:64-S128'\n");
  fprintf(arq, "target triple = 'x86_64-pc-linux-gnu'\n");
  fprintf(arq, "@.str = private unnamed_addr constant [6 x i8] c %s, align 1", txt);
  fprintf(arq, "\n");
  fprintf(arq, "\ndefine void @main() \%%\%d {\n", cont_stmt);
  cont_stmt++;

  yyparse();
  
  fprintf(arq, "\tret void\n}\n\n");
  if(iprint == 1)
    fprintf(arq, "declare i32 @printf(i8*, ...) #1\n");
  if(decl_pow == 1)
    fprintf(arq, "declare dso_local float @pow(float, float) #1");

  fclose(arq);
  if(arqtree == 1)
    fclose(tree_arq);

  return 0;

}