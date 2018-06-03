/*
 * $Id: eval.h,v 1.46 2014/03/10 01:28:37 sfeam Exp $
 */

/* GNUPLOT - eval.h */



#ifndef GNUPLOT_EVAL_H
# define GNUPLOT_EVAL_H

/* #if... / #include / #define collection: */

#include "syscfg.h"
#include "gp_types.h"

#include <stdio.h>    /* for FILE* */

#define STACK_DEPTH 250   /* maximum size of the execution stack */
#define MAX_AT_LEN 150    /* max number of entries in action table */

/* These are used by add_action() to index the subroutine list ft[] in eval.c */
enum operators {
  /* keep this in line with table in eval.c */
  PUSH, PUSHC, PUSHD1, PUSHD2, PUSHD, POP,
  CALL, CALLN, SUM, LNOT, BNOT, UMINUS,
  LOR, LAND, BOR, XOR, BAND, EQ, NE, GT, LT, GE, LE,
  LEFTSHIFT, RIGHTSHIFT, PLUS, MINUS,
  MULT, DIV, MOD, POWER, FACTORIAL, BOOLE,
  DOLLARS,
  CONCATENATE, EQS, NES, RANGE,
  ASSIGN,
  /* only jump operators go between jump and sf_start, for is_jump() */
  JUMP, JUMPZ, JUMPNZ, JTERN, SF_START,

  /* External function call */
#ifdef HAVE_EXTERNAL_FUNCTIONS
  CALLE,
#endif

  /* functions specific to using spec */
  COLUMN, STRINGCOLUMN
};
#define is_jump(operator) \
  ((operator) >=(int)JUMP && (operator) <(int)SF_START)

/* user-defined function table entry */
typedef struct udft_entry {
  struct udft_entry* next_udf; /* pointer to next udf in linked list */
  char* udf_name;    /* name of this function entry */
  struct at_type* at;    /* pointer to action table to execute */
  char* definition;    /* definition of function as typed */
  int dummy_num;     /* required number of input variables */
  t_value dummy_values[MAX_NUM_VAR]; /* current value of dummy variables */
} udft_entry;


/* user-defined variable table entry */
typedef struct udvt_entry {
  struct udvt_entry* next_udv; /* pointer to next value in linked list */
  char* udv_name;   /* name of this value entry */
  TBOOLEAN udv_undef;   /* true if not defined yet */
  t_value udv_value;    /* value it has */
} udvt_entry;

/* p-code argument */
typedef union argument {
  int j_arg;    /* offset for jump */
  struct value v_arg; /* constant value */
  struct udvt_entry* udv_arg; /* pointer to dummy variable */
  struct udft_entry* udf_arg; /* pointer to udf to execute */
#ifdef HAVE_EXTERNAL_FUNCTIONS
  struct exft_entry* exf_arg; /* pointer to external function */
#endif
} argument;


/* This type definition has to come after union argument has been declared. */
typedef void (*FUNC_PTR) __PROTO((union argument* arg));

/* standard/internal function table entry */
typedef struct ft_entry {
  const char* f_name;   /* pointer to name of this function */
  FUNC_PTR func;    /* address of function to call */
} ft_entry;

/* action table entry */
struct at_entry {
  enum operators index; /* index of p-code function */
  union argument arg;
};

struct at_type {
  /* count of entries in .actions[] */
  int a_count;
  /* will usually be less than MAX_AT_LEN is malloc()'d copy */
  struct at_entry actions[MAX_AT_LEN];
};


/* Variables of eval.c needed by other modules: */

extern const struct ft_entry GPFAR ft[]; /* The table of builtin functions */
extern struct udft_entry* first_udf; /* user-def'd functions */
extern struct udvt_entry* first_udv; /* user-def'd variables */
extern struct udvt_entry udv_pi; /* 'pi' variable */
extern struct udvt_entry* udv_NaN; /* 'NaN' variable */
extern struct udvt_entry** udv_user_head; /* first udv that can be deleted */
extern TBOOLEAN undefined;

/* Prototypes of functions exported by eval.c */

double gp_exp __PROTO((double x));

/* HBB 20010726: Moved these here, from util.h. */
double real __PROTO((struct value*));
int real_int __PROTO((struct value* val));
double imag __PROTO((struct value*));
double magnitude __PROTO((struct value*));
double angle __PROTO((struct value*));
struct value* Gcomplex __PROTO((struct value*, double, double));
struct value* Ginteger __PROTO((struct value*, int));
struct value* Gstring __PROTO((struct value*, char*));
struct value* pop_or_convert_from_string __PROTO((struct value*));
struct value* gpfree_string __PROTO((struct value* a));

void reset_stack __PROTO((void));
void check_stack __PROTO((void));
TBOOLEAN more_on_stack __PROTO((void));
struct value* pop __PROTO((struct value* x));
void push __PROTO((struct value* x));
void int_check __PROTO((struct value* v));

void f_bool __PROTO((union argument* x));
void f_jump __PROTO((union argument* x));
void f_jumpz __PROTO((union argument* x));
void f_jumpnz __PROTO((union argument* x));
void f_jtern __PROTO((union argument* x));

void execute_at __PROTO((struct at_type* at_ptr));
void evaluate_at __PROTO((struct at_type* at_ptr, struct value* val_ptr));
void free_at __PROTO((struct at_type* at_ptr));
struct udvt_entry* add_udv_by_name __PROTO((char* key));
struct udvt_entry* get_udv_by_name __PROTO((char* key));
void del_udv_by_name __PROTO((char* key, TBOOLEAN isWildcard));
void clear_udf_list __PROTO((void));

/* update GPVAL_ variables available to user */
void update_gpval_variables __PROTO((int from_plot_command));
/* note: the routines below work for any variable name, not just those beginning GPVAL_ */
void fill_gpval_string __PROTO((char* var, const char* value));
void fill_gpval_integer __PROTO((char* var, int value));
void fill_gpval_float __PROTO((char* var, double value));
void fill_gpval_complex __PROTO((char* var, double areal, double aimag));

/* C-callable versions of internal gnuplot functions word() and words() */
char* gp_word __PROTO((char* string, int i));
int gp_words __PROTO((char* string));

/* Evaluate the function linking secondary axis to primary axis */
double eval_link_function __PROTO((int, double));

#endif /* GNUPLOT_EVAL_H */