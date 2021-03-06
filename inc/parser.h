

#ifndef PARSER_H
#define PARSER_H
#include "compiler.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "tac.h"

typedef struct s_parse_stack
{
	char *token;
	char *type;
	char *construct;
	struct s_parse_stack *next;
	struct s_parse_stack *prev;
}	t_pstack;

typedef struct	s_current_variable
{
	char *str;
	struct s_current_variable *next;
}	t_current_var;

t_tree	*parser(t_token *tokens);
bool	is_datatype_correct(t_pstack *stack, int datatype_len);
char 	**psplit(char *str, char c);
t_current_var *push_curr_var(t_current_var *head, char *name);
void	free_curr_var(t_current_var *head);
bool	symbol_table_manager(t_current_var *current_variable, char *typing);
bool	false_error(t_token *token, int message);

bool	evaluate_equation(void);
bool    equ_tokens(char *name);
bool    sum_tokens(char *name);

bool	false_error(t_token *token, int number);

bool	evaluate_literal(t_token *token);
bool    evaluate_for(t_token *token);
bool    evaluate_return(t_token *token);
bool    evaluate_while(t_token *token);
bool    evaluate_do(t_token *token);
bool    evaluate_return(t_token *token);
bool    evaluate_else(t_token *token);
bool    evaluate_if(t_token *token);


t_current_var *add_index_depth(t_current_var *current_var, int index_depth);





















#endif
