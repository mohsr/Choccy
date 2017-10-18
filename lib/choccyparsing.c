/* 
 * choccyparsing.c
 * A Lisp language (called Choccy) and interpreter written by me in C,
 * using the help of www.buildyourownlisp.com. Algorithms provided by
 * www.buildyourownlisp.com.
 *
 * Written by:  Mohsin Rizvi
 * Last edited: 10/17/17
 */

#include "choccyparsing.h"

/* 
 * Purpose:    Preprocessor macro to be used for error checking.
 * Parameters: A cyval pointer ARGS, a conditional statement CONDITION, and
 *             a C-string error code ERROR.
 * Return:     Either nothing, or a pointer to an error-type cyval.
 */
#define CY_ASSERT(ARGS, CONDITION, ERROR)  \
        if (!CONDITION) {                  \
                cyval_destructor(ARGS);    \
                return cyval_error(ERROR); \
        }

/* Purpose:    Execute the program and start the REPL.
 * Parameters: An int argc (argument count) and c-string argv
 *             (additional string arguments).
 * Return:     Returns an int.
 */
int main(int argc, char** argv) {
        char* version = "v0.0.0.0.6";
        char* read;
        cyval* s_expression;
        cyval* evaluated;
        (void) argc;
        (void) argv;
        (void) s_expression;

    /* Define the language */
        mpc_parser_t* num = mpc_new("num");
        mpc_parser_t* sym = mpc_new("sym");
        mpc_parser_t* s_exp = mpc_new("s_exp");
        mpc_parser_t* q_exp = mpc_new("q_exp");
        mpc_parser_t* exp = mpc_new("exp");
        mpc_parser_t* line = mpc_new("line");
    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                     \
        num      : /-?[0-9]+/ ;                               \
        sym      : \"list\" | \"head\" | \"tail\"             \
                 | \"join\" | \"eval\"                        \
                 | '-' | '+' | '*' | '/' | '%' | '^' ;        \
        s_exp    : '(' <exp>* ')' ;                           \
        q_exp    : '{' <exp>* '}' ;                           \
        exp      : <num> | <sym> | <s_exp> | <q_exp> ;        \
        line     : /^/ <exp>* /$/ ;                           \
        ", num, sym, s_exp, q_exp, exp, line);
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
                        evaluated = cyval_evaluate(
                                cyval_read_tree(result.output));
                        print_cyval_endl(evaluated);
                        cyval_destructor(evaluated);
                        
                        mpc_ast_delete(result.output);
                } else {
                        mpc_err_print(result.error);
                        mpc_err_delete(result.error);
                }
                free(read);
        }
    mpc_cleanup(6, num, sym, s_exp, q_exp, exp, line);

    return 0;
}

/*
 * Purpose:    Construct a cyval number instance on the heap.
 * Parameters: A long int number value for the cyval to store.
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_num(long num_value) {
        cyval* value = malloc(sizeof(*value));
        value -> data_type = CYVAL_NUM;

        value -> num = num_value;

        return value;
}

/*
 * Purpose:    Construct a cyval error instance on the heap.
 * Parameters: A c-string representing an error message.
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_error(char* msg) {
        cyval* value = malloc(sizeof(*value));
        value -> data_type = CYVAL_ERROR;

        value -> error = malloc(strlen(msg) + 1);
        strcpy(value -> error, msg);

        return value;
}

/*
 * Purpose:    Construct a cyval symbol instance on the heap.
 * Parameters: A c-string representing a symbol.
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_sym(char* symbol) {
        cyval* value = malloc(sizeof(*value));
        value -> data_type = CYVAL_SYM;

        value -> sym = malloc(strlen(symbol) + 1);
        strcpy(value -> sym, symbol);

        return value;
}

/*
 * Purpose:    Construct a cyval s-expression instance on the heap.
 * Parameters: Void
 * Return:     A pointer to a constructed cyval instance.
 */
cyval* cyval_s_exp(void) {
        cyval* value = malloc(sizeof(*value));
        value -> data_type = CYVAL_S_EXP;

        value -> cyvals = NULL;
        value -> len_cyvals = 0;

        return value;
}

/*
 * Purpose:    Construct a cyval q-expression on the heap.
 * Parameters: Void
 * Return:     A pointer to an allocated cyval instance.
 */
cyval* cyval_q_exp(void) {
        cyval* value = malloc(sizeof(*value));
        value -> data_type = CYVAL_Q_EXP;

        value -> cyvals = NULL;
        value -> len_cyvals = 0;

        return value;
}

/*
 * Purpose:    Deallocate heap memory used by the given cyval pointer.
 * Parameters: A pointer to a cyval to deallocate memory for.
 * Return:     Void
 */
void cyval_destructor(cyval* value) {
        int i;

        if (value != NULL) {
                /* Handle the case where the cyval represents an error. */
                if (value -> data_type == CYVAL_ERROR)
                        free(value -> error);
                /* Handle the case where the cyval represents a symbol. */
                else if (value -> data_type == CYVAL_SYM)
                        free(value -> sym);
                /*
                 * Recursively handle the case where the cyval represents
                 * an expression, freeing all expressions it holds.
                 */
                else if (value -> data_type == CYVAL_S_EXP ||
                         value -> data_type == CYVAL_Q_EXP) {
                        for (i = 0; i < value -> len_cyvals; i++)
                                cyval_destructor(value -> cyvals[i]);
                        free(value -> cyvals);
                }
        }
        free(value);
}

/*
 * Purpose:    Print the cyval s-expression pointed to by the given pointer,
 *             and print print the opening char before and the ending char
 *             after.
 * Parameters: A pointer to a cyval s-expression to print from, a char
 *             opening to print first, and a char ending to print last.
 * Return:     Void
 */
void print_cyval_exp(cyval* value, char opening, char ending) {
        int i;

        putchar(opening);

        /* Print everything within the s-expression. */
        for (i = 0; i < value -> len_cyvals; i++) {
                print_cyval(value -> cyvals[i]);

                if (i != (value -> len_cyvals - 1))
                        putchar(' ');
        }

        putchar(ending);
}

/*
 * Purpoes:    Print a cyval pointed to by the given pointer.
 * Parameters: A pointer to a cyval to print from.
 * Return:     Void
 */
void print_cyval(cyval* value) {
        if (value -> data_type == CYVAL_NUM)
                printf("%li", value -> num);
        else if (value -> data_type == CYVAL_ERROR)
                printf("Error: %s", value -> error);
        else if (value -> data_type == CYVAL_SYM)
                printf("%s", value -> sym);
        else if (value -> data_type == CYVAL_Q_EXP)
                print_cyval_exp(value, '{', '}');
        else if (value -> data_type == CYVAL_S_EXP)
                print_cyval_exp(value, '(', ')');
}

/*
 * Purpose:    Print the cyval pointed to by the given cyval pointer
 *             with a newline char at the end.
 * Parameters: A pointer to a cyval to print.
 * Return:     Void
 */
void print_cyval_endl(cyval* value) {
        print_cyval(value);
        putchar('\n');
}

/*
 * Purpose:    Recursively read an MPC abstract syntax tree from the given
 *             MPC abstract syntax tree pointer and read it into a cyval
 *             pointer's target value.
 * Parameters: A pointer to an MPC abstract syntax tree to read from.
 * Return:     A pointer to a cyval with the MPC abstract syntax tree data.
 */
cyval* cyval_read_tree(mpc_ast_t* node) {
        int i;
        cyval* value = NULL;

        /* If tree is a number node, read it and store it. */
        if (strstr(node -> tag, "num"))
                return cyval_read_node(node);
        /* If tree is a symbol node, read it and store it. */
        if (strstr(node -> tag, "sym"))
                return cyval_sym(node -> contents);

        /* Create empty list at start of input or at s-expression. */
        if (strcmp(node -> tag, ">") == 0 ||
            strstr(node -> tag, "s_exp"))
                value = cyval_s_exp();
        if (strstr(node -> tag, "q_exp"))
                value = cyval_q_exp();
        /* Read and store valid expressions from MPC abstract syntax tree. */
        for (i = 0; i < node -> children_num; i++) {
                /* Skip over invalid expressions. */
                if ((strcmp(node -> children[i] -> contents, "(") == 0) ||
                    (strcmp(node -> children[i] -> contents, ")") == 0) ||
                    (strcmp(node -> children[i] -> tag, "regex") == 0)  ||
                    (strcmp(node -> children[i] -> contents, "{") == 0) ||
                    (strcmp(node -> children[i] -> contents, "}") == 0))
                	continue;
                value = cyval_add(value,
                                  cyval_read_tree(node -> children[i]));
        }

        return value;
}

/*
 * Purpose:    Read a node from an MPC abstract syntax tree given by the
 *             given MPC abstract syntax tree pointer.
 * Parameters: A pointer to an MPC abstract syntax tree.
 * Return:     A pointer to a cyval representing the read node.
 */
cyval* cyval_read_node(mpc_ast_t* node) {
        long num;

        errno = 0;
        num = strtol(node -> contents, NULL, 10);

        if (errno != ERANGE)
                return cyval_num(num);
        else
                return cyval_error("Invalid number");
}

/*
 * Purpose:    Add a given cyval pointer (second param) to a given cyval
 *             pointer's (first param) list of cyval pointers and reallocates
 *             memory accordingly.
 * Parameters: A pointer to a cyval (first param) whose cyval pointer list a
 *             cyval pointer (second param) will be added to.
 * Return:     A pointer to the cyval with an increased list of cyval
 *             pointers.
 */
cyval* cyval_add(cyval* value, cyval* to_add) {
        /* Increase amt of pointers stored and allocate space accordingly. */
        if (value == NULL)
        	return NULL;
        value -> len_cyvals++;
        value -> cyvals = realloc(value -> cyvals,
                                    sizeof(cyval*) * value -> len_cyvals);
        /* Add the given cyval pointer to the list. */
        value -> cyvals[value -> len_cyvals - 1] = to_add;
        return value;
}

/*
 * Purpose:    Evaluate a cyval s-expression pointed to by the given pointer.
 * Parameters: A pointer pointing to a cyval s-expression.
 * Return:     A cyval pointer pointing to the evaluated result.
 */
cyval* cyval_evaluate_s_exp(cyval* value) {
        int i;
        cyval* first;
        cyval* result;
        /* Evaluate children of the given cyval s-expression. */
        for (i = 0; i < value -> len_cyvals; i++)
                value -> cyvals[i] = cyval_evaluate(value -> cyvals[i]);
        /* Check for errors in the given cyval s-expression. */
        for (i = 0; i < value -> len_cyvals; i++)
                if(value -> cyvals[i] -> data_type == CYVAL_ERROR)
                        return cyval_take(value, i);
        /* Check for s-expression with either zero elements or one element. */
        if (value -> len_cyvals == 0)
                return value;
        if (value -> len_cyvals == 1)
                return cyval_take(value, 0);
        /* Check if the first element in s-expression is a symbol. */
        first = cyval_pop(value, 0);
        if (first -> data_type != CYVAL_SYM) {
                cyval_destructor(first);
                cyval_destructor(value);
                return cyval_error("S-expression doesn't start with symbol");
        }
        /* Call the builtin operator or function. */
        result = builtins(value, first -> sym);
        cyval_destructor(first);
        return result;
}

/*
 * Purpose:    Removes a cyval pointer from the given pointed to cyval's
 *             list of cyval pointers at the given int index, then returns it.
 * Parameters: A cyval pointer representing a cyval with a list of cyval
 *             pointers, and an int index i representing the cyval pointer to
 *             extract from the list.
 * Return:     A pointer to the extracted cyval.
 */
cyval* cyval_pop(cyval* value, int i) {
        cyval* extracted;

        extracted = value -> cyvals[i];

        /* Shift memory over to remove the extracted cyval pointer. */
        memmove(&value -> cyvals[i], 
                    &value -> cyvals[i + 1],
                    sizeof(cyval*) * (value -> len_cyvals - i - 1));
        value -> len_cyvals--;

        /* Reallocate the memory. */
        value -> cyvals = realloc(value -> cyvals,
                                    sizeof(cyval*) * value -> len_cyvals);

        return extracted;
}

/*
 * Purpose:    Removes a cyval pointer from the given pointed to cyval's
 *             list of cyval pointers at the given int index, deletes the
 *             list, then returns it.
 * Parameters: A cyval pointer representing a cyval with a list of cyval
 *             pointers, and an int index i representing the cyval pointer
 *             to extract from the list.
 * Return:     A pointer to the extracted cyval.
 */
cyval* cyval_take(cyval* value, int i) {
        cyval* extracted;

        /* Extract the pointer and delete the remaining ones. */
        extracted = cyval_pop(value, i);
        cyval_destructor(value);

        return extracted;
}

/*
 * Purpose:    Call a builtin function or operator.
 * Parameters: A cyval to call with and a C-string operator or function name.
 * Return:     A cyval with the operation result.
 */
cyval* builtins(cyval* value, char* func) {
        if (strcmp("head", func) == 0)
                return builtin_head(value);
        else if (strcmp("tail", func) == 0)
                return builtin_tail(value);
        else if (strcmp("list", func) == 0)
                return builtin_list(value);
        else if (strcmp("join", func) == 0)
                return builtin_join(value);
        else if (strcmp("eval", func) == 0)
                return builtin_eval(value);
        else if (strstr("+-/*%^", func))
                return builtin_ops(value, func);

        cyval_destructor(value);

        return cyval_error("Unknown function");
}

/*
 * Purpose:    Operates on a given cyval representing arguments using the
 *             given c-string operator.
 * Parameters: A cyval s-expression pointer containing arguments to
 *             operate on and a given c-string operator.
 * Return:     A pointer to a cyval containing the result of operating.
 */
cyval* builtin_ops(cyval* value, char* ops) {
        int i;
        cyval* extract;
        cyval* next;
        /* Check if all arguments in the s-expression are valid numbers. */
        for (i = 0; i < value -> len_cyvals; i++)
                if (value -> cyvals[i] -> data_type != CYVAL_NUM) {
                        cyval_destructor(value);
                        return cyval_error(
                                "Non-number passed as operation argument");
                }
        /* Extract the first element. */
        extract = cyval_pop(value, 0);
        /* If subtracting with no additonal arguments, negate the number. */
        if ((strcmp(ops, "-") == 0) && (value -> len_cyvals == 0))
                extract -> num = -(extract -> num);
        /* Perform operations while arguments remain. */
        while (value -> len_cyvals > 0) {
                next = cyval_pop(value, 0);
                /* Perform operations. */
                if (strcmp(ops, "+") == 0)
                        extract -> num += next -> num;
                else if (strcmp(ops, "-") == 0)
                        extract -> num -= next -> num;
                else if (strcmp(ops, "*") == 0)
                        extract -> num *= next -> num;
                else if (strcmp(ops, "/") == 0) {
                        /* Check for division by zero. */
                        if (next -> num == 0) {
                                cyval_destructor(extract);
                                cyval_destructor(next);
                                extract = cyval_error("Division by zero");
                                break;
                        }
                        extract -> num /= next -> num;
                }
                cyval_destructor(next);
        }
        cyval_destructor(value);
        return extract;
}

/*
 * Purpose:    A built-in function "head" that returns a Q-expression
 *             with only the first element.
 * Parameters: A pointer to a cyval that is a valid Q-expressions.
 * Return:     A pointer to a cyval.
 */
cyval* builtin_head(cyval* value) {
        cyval* args;
        CY_ASSERT(value, (value -> len_cyvals == 1), "\"head\" function \
                  passed too many args");
        CY_ASSERT(value, (value -> cyvals[0] -> data_type == CYVAL_Q_EXP),
                  "\"head\" function passed incorrect types");
        CY_ASSERT(value, (value -> cyvals[0] -> len_cyvals != 0),
                  "\"head\" function passed no args");

        /* Get the arguments and destroy all but the last. */
        args = cyval_take(value, 0);
        while (args -> len_cyvals > 1)
                cyval_destructor(cyval_pop(args, 1));
        return args;
}

/*
 * Purpose:    A built-in function "tail" that returns a Q-expression with
 *             all elements but the first one taken.
 * Parameters: A pointer to a cyval that is a valid Q-expression and contains
 *             the list of cyvals.
 * Return:     A pointer to a cyval.
 */
cyval* builtin_tail(cyval* value) {
        cyval* args;
        CY_ASSERT(value, (value -> len_cyvals == 1), "\"tail\" function \
                  passed too many args");
        CY_ASSERT(value, (value -> cyvals[0] -> data_type == CYVAL_Q_EXP),
                  "\"tail\" function passed incorrect types");
        CY_ASSERT(value, (value -> cyvals[0] -> len_cyvals != 0),
                  "\"tail\" function passed no args");

        /* Get arguments and destroy the first. */
        args = cyval_take(value, 0);
        cyval_destructor(cyval_pop(args, 0));
        return args;
}

/*
 * Purpose:    Converts a given cyval S-expression to a Q-expression.
 * Parameters: A pointer to an S-expression cyval.
 * Return:     A pointer to a Q-expression cyval.
 */
cyval* builtin_list(cyval* value) {
        value -> data_type = CYVAL_Q_EXP;
        return value;
}

/*
 * Purpose:    Converts a given cyval Q-expression to an S-expression and
 *             evaluates it.
 * Parameters: A pointer to a cyval Q-expression to convert.
 * Return:     A pointer to an evaluated cyval S-expression.
 */
cyval* builtin_eval(cyval* value) {
        cyval* args;

        CY_ASSERT(value, (value -> len_cyvals == 1), "\"eval\" function \
                  passed too many args");
        CY_ASSERT(value, (value -> cyvals[0] -> data_type == CYVAL_Q_EXP),
                  "\"eval\" function passed incorrect types");

        args = cyval_take(value, 0);
        args -> data_type = CYVAL_S_EXP;
        return cyval_evaluate(args);
}

/*
 * Purpose:    Takes a cyval Q-expression and joins together its children.
 * Parameters: A pointer to a cyval to join children of.
 * Return:     The joined cyval.
 */
cyval* builtin_join(cyval* value) {
        cyval* args;
        int i;

        for (i = 0; i < value -> len_cyvals; i++)
                CY_ASSERT(value, (value -> cyvals[i] -> data_type ==
                          CYVAL_Q_EXP), "\"join\" function passed incorrect \
                          types");

        /* Join each given argument. */
        args = cyval_pop(value, 0);
        while (value -> len_cyvals)
                args = cyval_join(args, cyval_pop(value, 0));
        cyval_destructor(value);

        return args;
}

/*
 * Purpose:    Takes two cyvals and joins the children of the second one to
 *             the first one. Helper function for builtin_join.
 * Parameters: A pointer to a cyval and then another pointer to a cyval whose
 *             children will be joined with the first cyval parameter.
 * Return:     A joined cyval.
 */
cyval* cyval_join(cyval* a, cyval* b) {
        /* Add each child of cyval b to cyval a. */
        while (b -> len_cyvals)
                a = cyval_add(a, cyval_pop(b, 0));

        cyval_destructor(b);
        return a;
}

/*
 * Purpose:    Evaluate the cyval pointed to by the given pointer if it
 *             represents an s-expression.
 * Parameters: A pointer to a cyval.
 * Return:     A pointer to a cyval that is evaluated if it is an
 *             s-expression, or is the same as the original argument
 *             otherwise.
 */
cyval* cyval_evaluate(cyval* value) {
        if (value -> data_type == CYVAL_S_EXP)
                return cyval_evaluate_s_exp(value);
        
        return value;
}
