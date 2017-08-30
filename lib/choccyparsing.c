/* 
 * choccyparsing.c
 * A Lisp interpreter written by me in C, using the help of
 * www.buildyourownlisp.com.
 *
 * Written by:  Mohsin Rizvi
 * Last edited: 08/29/17
 */

#include "choccyparsing.h"

/* Purpose:    Execute the program and start the REPL.
 * Parameters: An int argc (argument count) and cstring argv
 *             (additional string arguments).
 * Return:     Returns an int.
 */
int main(int argc, char** argv) {
	char* version = "v0.0.0.0.3";
	char* read;
	cyval evaluated;

    /* Define the language */
	mpc_parser_t* num = mpc_new("num");
	mpc_parser_t* ops = mpc_new("ops");
	mpc_parser_t* exp = mpc_new("exp");
	mpc_parser_t* line = mpc_new("line");
    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                     \
        num      : /-?[0-9]+/ ;                               \
        ops      : '-' | '+' | '*' | '/' | '%' | '^' ;        \
        exp      : <num> | '(' <ops> <exp>+ ')' ;             \
        line     : /^/ <ops> <exp>+ /$/ ;                     \
        ", num, ops, exp, line);
	/* Begin the REPL */
	printf("choccy %s\nTo exit, press ctrl+c\n", version);
	while (1) {
		read = readline("choccy> ");
		add_history(read);
		/*
		 * Parse input from standard input into read, checking for
		 * correct choccy statement.
		 */
		mpc_result_t result;
		/* Process input based on validity. */
		if (mpc_parse("<stdin>", read, line, &result)) {
			evaluated = evaluate(result.output);
			print_cyval_endl(evaluated);
			mpc_ast_delete(result.output);
		}
		else {
			mpc_err_print(result.error);
			mpc_err_delete(result.error);
		}
		free(read);
	}
    mpc_cleanup(4, num, ops, exp, line);

    return 0;
}

/*
 * Purpose:    Construct a cyval number instance.
 * Parameters: A long int number value for the cyval to store.
 * Return:     A constructed cyval instance.
 */
cyval cyval_num(long num_value) {
	cyval value;

	value.data_type = CYVAL_NUM;
	value.data.num = num_value;

	return value;
}

/*
 * Purpose:    Construct a cyval error instance.
 * Parameters: An int error value for the cyval to store.
 * Return:     A constructed cyval instance.
 */
cyval cyval_error(int error_value) {
	cyval value;

	value.data_type = CYVAL_ERROR;
	value.data.error = error_value;

	return value;
}

/*
 * Purpoes:    Print a given cyval.
 * Parameters: A cyval to print.
 * Return:     Void
 */
void print_cyval(cyval value) {
	if (value.data_type == CYVAL_NUM)
		printf("%li", value.data.num);

	else if (value.data_type == CYVAL_ERROR) {
		if (value.data.error == CYVAL_DIV_ZERO)
			printf("Error: division by zero");
		else if (value.data.error == CYVAL_INV_OPS)
			printf("Error: invalid operator");
		else if (value.data.error == CYVAL_INV_NUM)
			printf("Error: invalid number");
	}
}

/*
 * Purpose:    Print a given cyval with a newline character at the end.
 * Parameters: A cyval to print.
 * Return:     Void
 */
void print_cyval_endl(cyval value) {
	print_cyval(value);
	putchar('\n');
}

/*
 * Purpose:    Recursively evaluate a line of choccy code.
 *             Algorithm provided by
 *             http://www.buildyourownlisp.com/chapter7_evaluation.
 * Parameters: An MPC abstract syntax tree node.
 * Return:     A cyval with the evaluated data.
 */
cyval evaluate(mpc_ast_t* node) {
    char* ops;
    cyval opsExp;
    long num;
    int i;

    /* Base case: Tree contains just a number. */
    if (strstr(node->tag, "num")) {
    	/* Check that number is a valid long int. */
    	errno = 0;
		num = strtol(node->contents, NULL, 10);
		if (errno != ERANGE)
			return cyval_num(num);
		else
			return cyval_error(CYVAL_INV_NUM);
    }
    /* Process remaining children based on given operation. */
    ops = node->children[1]->contents;
    opsExp = evaluate(node->children[2]);
    for (i = 3; strstr(node->children[i]->tag, "exp"); i++)
    	opsExp = operate(opsExp, evaluate(node->children[i]), ops);
    /* Return result of operation. */
    return opsExp;
}

/*
 * Purpose:    Operate on two long ints using the given operator.
 *             Algorithm provided by
 *             http://www.buildyourownlisp.com/chapter7_evaluation.
 * Parameters: A cyval num first argument, a cyval num second argument, and a
 *             char* representing an operator.
 * Return:     A cyval with the finished data.
 */
cyval operate(cyval first, cyval second, char* ops) {
	long arg1;
	long arg2;

	if (first.data_type == CYVAL_ERROR)
		return first;
	else if (second.data_type == CYVAL_ERROR)
		return second;
	arg1 = first.data.num;
	arg2 = second.data.num;
	/* Operate according to given operator. */
	if (strcmp(ops, "*") == 0)
		return cyval_num(arg1 * arg2);
	else if (strcmp(ops, "/") == 0) {
		if (arg2 == 0)
			return cyval_error(CYVAL_DIV_ZERO);
		else
			return cyval_num(arg1 / arg2);
	}
	else if (strcmp(ops, "+") == 0)
		return cyval_num(arg1 + arg2);
	else if (strcmp(ops, "-") == 0)
		return cyval_num(arg1 - arg2);
	else if (strcmp(ops, "%") == 0)
		return cyval_num(arg1 % arg2);
	else if (strcmp(ops, "^") == 0)
		return cyval_num(powl(arg1, arg2));
	return cyval_error(CYVAL_INV_OPS);
}