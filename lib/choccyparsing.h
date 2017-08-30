/* 
 * choccyparsing.h
 * Header file for choccyparsing.c, declaring functions used.
 * Built with the help of www.buildyourownlisp.com.
 *
 * Written by:  Mohsin Rizvi
 * Last edited: 08/29/17
 */

#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <math.h>
#include "mpc/mpc.h"

/* Enumeration of possible types of cyvals: numbers and errors. */
enum { CYVAL_NUM, CYVAL_ERROR};
/* 
 * Enumeration of possible errors: dividing by zero, unknown operators,
 * and invalid numbers.
 */
enum { CYVAL_DIV_ZERO, CYVAL_INV_OPS, CYVAL_INV_NUM };

/*
 * Choccy data (cydata) union, meant to hold evaluated long int or
 * an int representing an error.
 */
typedef union {
    long num;
    int error;
} cydata;

/*
 * Choccy value (cyval) struct, meant to hold data (see cydata union).
 */
typedef struct {
    int data_type;
    cydata data;
} cyval;

/*
 * Purpose:    Construct a cyval number instance.
 * Parameters: A long int number value for the cyval to store.
 * Return:     A constructed cyval instance.
 */
cyval cyval_num(long num_value);

/*
 * Purpose:    Construct a cyval error instance.
 * Parameters: An int error value for the cyval to store.
 * Return:     A constructed cyval instance.
 */
cyval cyval_error(int error_value);

/*
 * Purpose:    Print a given cyval.
 * Parameters: A cyval to print.
 * Return:     Void
 */
void print_cyval(cyval value);

/*
 * Purpose:    Print a given cyval with a newline character at the end.
 * Parameters: A cyval to print.
 * Return:     Void
 */
void print_cyval_endl(cyval value);

/*
 * Purpose:    Recursively evaluate a line of choccy code.
 *             Algorithm provided by
 *             http://www.buildyourownlisp.com/chapter7_evaluation.
 * Parameters: A MPC abstract syntax tree node.
 * Return:     A cyval with the evaluated data.
 */
cyval evaluate(mpc_ast_t* node);

/*
 * Purpose:    Operate on two long ints using the given operator.
 *             Algorithm provided by
 *             http://www.buildyourownlisp.com/chapter7_evaluation.
 * Parameters: A cyval num first argument, a cyval num second argument, and a
 *             char* representing an operator.
 * Return:     A cyval with the finished data.
 */
cyval operate(cyval first, cyval second, char* ops);
