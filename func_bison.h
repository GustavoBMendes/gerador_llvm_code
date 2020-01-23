/* Companion source code for "flex & bison", published by O'Reilly
 * Media, ISBN 978-0-596-15597-1
 * Copyright (c) 2009, Taughannock Networks. All rights reserved.
 * See the README file for license conditions and contact info.
 * $Header: /home/johnl/flnb/code/RCS/fb3-2.h,v 2.1 2009/11/08 02:53:18 johnl Exp $
 */
/*
 * Declarations for a calculator fb3-1
 */

/* symbol table */
struct symbol {		/* a variable name */
  char *name;
  double value;
  int flag;
  int i;
  int f;
  struct ast *func;	/* stmt for the function */
  struct symlist *syms; /* list of dummy args */
};

/* simple symtab of fixed size */
#define NHASH 9997
struct symbol symtab[NHASH];

struct symbol *lookup(char*);

/* list of symbols, for an argument list */
struct symlist {
  struct symbol *sym;
  struct symlist *next;
};

struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

/* node types
 *  + - * / |
 *  0-7 comparison ops, bit coded 04 equal, 02 less, 01 greater
 *  M unary minus
 *  L statement list
 *  I IF statement
 *  W WHILE statement
 *  N symbol ref
 *  = assignment
 *  S list of symbols
 *  F built in function call
 *  C user function call
 */ 

/* nodes in the Abstract Syntax Tree */
/* all have common initial nodetype */

struct ast {
  int nodetype;
  struct ast *l;
  struct ast *r;
  int i;
  int f;
};

struct numval {
  int nodetype;			/* type K */
  double number;
  int tipo;
};

struct symref {
  int nodetype;			/* type N */
  struct symbol *s;
};

struct symasgn {
  int nodetype;			/* type = */
  struct symbol *s;
  struct ast *v;		/* value */
  int tipo;
};

/* build an AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newcall(struct ast *l, struct ast *r);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v, int tipo);
struct ast *newnum(double d, int t);

/* evaluate an AST */
double eval(struct ast *);

/* delete and free an AST */
void treefree(struct ast *);

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);

extern int debug;
void dumpast(struct ast *a, int level);
