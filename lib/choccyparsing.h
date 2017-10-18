/* 
 * choccyparsing.h
 * Header file for choccyparsing.c, declaring functions used and defining
 * data structures. Built with the help of www.buildyourownlisp.com.
 * Algorithms provided by www.buildyourownlisp.com.
 *
 * Written by:  Mohsin Rizvi
 * Last edited: 10/17/17
 */

#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <math.h>
#include "mpc/mpc.h"

/* Enumeration of possible types of cyvals: numbers and errors. */
enum { CYVAL_NUM, CYVAL_ERROR, CYVAL_SYM, CYVAL_S_EXP, CYVAL_Q_EXP};

/*
 * TODO--UPDATE UNION FOR TYPES OF CYVAL DATA
 *
 * Choccy data (cydata) union, meant to hold evaluated long int or
 * an int representing an error.
 *
 * typedef union {
 *      long num;
 *      int error;
 * } cydata;
*/

/*
 * Choccy value (cyval) struct, meant to hold data (see cydata union).
 */
typedef struct cyval {
        int data_type;
        long num;
        char* error;
        char* sym;
        /* Array of cyvals to point to */
        int len_cyvals;
        struct cyval** cyvals;
} cyval;

/*
 * Purpose:    Construct a cyval number instance on the heap.
 * Parameters: A long int number value for the cyval to store.
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_num(long num_value);

/*
 * Purpose:    Construct a cyval error instance on the heap.
 * Parameters: A c-string representing an error message.
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_error(char* msg);

/*
 * Purpose:    Construct a cyval symbol instance on the heap.
 * Parameters: A c-string representing a symbol.
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_sym(char* sym);

/*
 * Purpose:    Construct a cyval s-expression instance on the heap.
 * Parameters: Void
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_s_exp(void);

/*
 * Purpose:    Construct a cyval q-expression on the heap.
 * Parameters: Void
 * Return:     A pointer to an allocated cyval instance.
 */
cyval* cyval_q_exp(void);

/*
 * Purpose:    Deallocate heap memory used by the given cyval pointer.
 * Parameters: A pointer to a cyval to deallocate memory for.
 * Return:     Void
 */
void cyval_destructor(cyval* value);

/*
 * Purpose:    Print the cyval s-expression pointed to by the given pointer,
 *             and print print the opening char before and the ending char
 *             after.
 * Parameters: A pointer to a cyval s-expression to print from, a char
 *             opening to print first, and a char ending to print last.
 * Return:     Void
 */
void print_cyval_exp(cyval* value, char opening, char ending);

/*
 * Purpoes:    Print a cyval pointed to by the given pointer.
 * Parameters: A pointer to a cyval to print from.
 * Return:     Void
 */
void print_cyval(cyval* value);

/*
 * Purpose:    Print the cyval pointed to by the given cyval pointer
 *             with a newline char at the end.
 * Parameters: A pointer to a cyval to print.
 * Return:     Void
 */
void print_cyval_endl(cyval* value);

/*
 * Purpose:    Recursively read an MPC abstract syntax tree from the given
 *             MPC abstract syntax tree pointer and read it into a cyval
 *             pointer's target value.
 * Parameters: A pointer to an MPC abstract syntax tree to read from.
 * Return:     A pointer to a cyval with the MPC abstract syntax tree data.
 */
cyval* cyval_read_tree(mpc_ast_t* node);

/*
 * Purpose:    Add a given cyval pointer (second param) to a given cyval
 *             pointer's (first param) list of cyval pointers and reallocates
 *             memory accordingly.
 * Parameters: A pointer to a cyval (first param) whose cyval pointer list a
 *             cyval pointer (second param) will be added to.
 * Return:     A pointer to the cyval with an increased list of cyval
 *             pointers.
 */
cyval* cyval_add(cyval* value, cyval* to_add);

/*
 * Purpose:    Read a node from an MPC abstract syntax tree given by the
 *             given MPC abstract syntax tree pointer.
 * Parameters: A pointer to an MPC abstract syntax tree.
 * Return:     A pointer to a cyval representing the read node.
 */
cyval* cyval_read_node(mpc_ast_t* node);

/*
 * Purpose:    Evaluate a cyval s-expression pointed to by the given pointer.
 * Parameters: A pointer pointing to a cyval s-expression.
 * Return:     A cyval pointer pointing to the evaluated result.
 */
cyval* cyval_evaluate_s_exp(cyval* value);

/*
 * Purpose:    Call a builtin function or operator.
 * Parameters: A cyval to call with and a C-string operator or function name.
 * Return:     A cyval with the operation result.
 */
cyval* builtins(cyval* value, char* func);

/*
 * Purpose:    A built-in function "head" that returns a Q-expression
 *             with only the first element.
 * Parameters: A pointer to a cyval that is a valid Q-expressions.
 * Return:     A pointer to a cyval.
 */
cyval* builtin_head(cyval* a);

/*
 * Purpose:    A built-in function "tail" that returns a Q-expression with
 *             all elements but the first one taken.
 * Parameters: A pointer to a cyval that is a valid Q-expression and contains
 *             the list of cyvals.
 * Return:     A pointer to a cyval.
 */
cyval* builtin_tail(cyval* value);

/*
 * Purpose:    Converts a given cyval S-expression to a Q-expression.
 * Parameters: A pointer to an S-expression cyval.
 * Return:     A pointer to a Q-expression cyval.
 */
cyval* builtin_list(cyval* value);

/*
 * Purpose:    Converts a given cyval Q-expression to an S-expression and
 *             evaluates it.
 * Parameters: A pointer to a cyval Q-expression to convert.
 * Return:     A pointer to an evaluated cyval S-expression.
 */
cyval* builtin_eval(cyval* value);

/*
 * Purpose:    Takes a cyval Q-expression and joins together its children.
 * Parameters: A pointer to a cyval to join children of.
 * Return:     The joined cyval.
 */
cyval* builtin_join(cyval* value);

/*
 * Purpose:    Takes two cyvals and joins the children of the second one to
 *             the first one. Helper function for builtin_join.
 * Parameters: A pointer to a cyval and then another pointer to a cyval whose
 *             children will be joined with the first cyval parameter.
 * Return:     A joined cyval.
 */
cyval* cyval_join(cyval* a, cyval* b);

/*
 * Purpose:    Evaluate the cyval pointed to by the given pointer if it
 *             represents an s-expression.
 * Parameters: A pointer to a cyval.
 * Return:     A pointer to a cyval that is evaluated if it is an
 *             s-expression, or is the same as the original argument
 *             otherwise.
 */
cyval* cyval_evaluate(cyval* value);

/*
 * Purpose:    Removes a cyval pointer from the given pointed to cyval's
 *             list of cyval pointers at the given int index, then returns it.
 * Parameters: A cyval pointer representing a cyval with a list of cyval
 *             pointers, and an int index i representing the cyval pointer to
 *             extract from the list.
 * Return:     A pointer to the extracted cyval.
 */
cyval* cyval_pop(cyval* value, int i);

/*
 * Purpose:    Removes a cyval pointer from the given pointed to cyval's
 *             list of cyval pointers at the given int index, deletes the
 *             list, then returns it.
 * Parameters: A cyval pointer representing a cyval with a list of cyval
 *             pointers, and an int index i representing the cyval pointer
 *             to extract from the list.
 * Return:     A pointer to the extracted cyval.
 */
cyval* cyval_take(cyval* value, int i);

/*
 * Purpose:    Operates on a given cyval representing arguments using the
 *             given c-string operator.
 * Parameters: A cyval s-expression pointer containing arguments to
 *             operate on and a given c-string operator.
 * Return:     A pointer to a cyval containing the result of operating.
 */
cyval* builtin_ops(cyval* value, char* ops);

/*
 * Purpose:    Recursively evaluate a line of choccy code.
 * Parameters: A MPC abstract syntax tree node.
 * Return:     A cyval with the evaluated data.
 */
cyval evaluate(mpc_ast_t* node);

/*
 * Purpose:    Operate on two long ints using the given operator.
 * Parameters: A cyval num first argument, a cyval num second argument, and a
 *             char* representing an operator.
 * Return:     A cyval with the finished data.
 */
cyval operate(cyval first, cyval second, char* ops);
