
#include "../inc/tac.h"
#include "../inc/token.h"
#include "../inc/symbol.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "../inc/parser.h"
#include <stdbool.h>
#include "../inc/compiler.h"

char datatypes[13][10] = {
	"unsigned\0",
        "short\0",
        "long\0",
        "int\0",
        "char\0",
        "void\0",
        "float\0",
        "double\0",
        "signed\0",
        "struct\0",
        "union\0"
};

char		*this_var = NULL; // for printing out defective variable naming stuff.
t_function	*functions = NULL;
int		stack_height = 0;
t_pstack	*pstack = NULL;
t_hashtable	*ff_list = NULL;
bool		datatype_set = false;
char		*typing = NULL;
int		brackets = 0;
int		round_bracket = 0;
int		datatype_len = 0;
t_current_var	*current_variable = NULL;
int		asterisk_count = 0;
t_token		*function_stack = NULL;
bool		typecasting = false;
t_typecast	*typecast = NULL;

bool		false_error(t_token *token, int message);

t_token		*push_to_stack(t_token *function_stack, t_token *token)
{
	function_stack = push_token(function_stack, token->name, token->type,
		token->line, token->filename);
}

t_current_var	*new_curr_var(char *name)
{
	t_current_var *new;

	new = (t_current_var *)malloc(sizeof(t_current_var));
	new->str = strdup(name);
	new->next = NULL;
	return (new);
}

t_current_var	*add_curr_var(t_current_var *head, char *name)
{
	t_current_var	*trav;

	trav = head;
	while (trav->next)
		trav = trav->next;
	trav->next = new_curr_var(name);
	return (trav);
}

t_current_var	*push_curr_var(t_current_var *head, char *name)
{
	t_current_var	*trav;

	trav = head;
	if (trav == NULL)
	{
		trav = new_curr_var(name);
	}
	else
	{
		trav = add_curr_var(head, name);
		trav = head;
	}
	return (trav);
}

void		free_curr_var(t_current_var *head)
{
	t_current_var 	*trav;

	while (head)
	{
		trav = head;
		head = head->next;
		free(trav->str);
		free(trav);
	}
}

t_pstack	*new_ptoken(char *token, char *type)
{
	t_pstack *new;

	new = (t_pstack *)malloc(sizeof(t_pstack));
	new->token = strdup(token);
	new->type = strdup(type);
	new->construct = NULL;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}


t_pstack	*add_ptoken(t_pstack *head, char *token, char *type)
{
	t_pstack	*trav;

	trav = head;
	while (trav->next)
		trav = trav->next;
	trav->next = new_ptoken(token, type);
	trav->next->prev = trav;
	trav = trav->next;
	return (trav);
}

t_pstack	*push_ptoken(t_pstack *head, char *token, char *type)
{
	t_pstack 	*trav;

	trav = head;
	if (trav == NULL)
	{
		trav = new_ptoken(token, type);
	}
	else
	{
		trav = add_ptoken(head, token, type);
	}
	if (typing)
		trav->construct = strdup(typing);
	stack_height++;
	return (trav);
}

void	clear_pstack(void)
{
	t_pstack *trav;
	if (typing)
	{
		free(typing);
		typing = NULL;
	}
	if (!pstack)
		return ;
	while (pstack)
	{
		trav = pstack;
		pstack = pstack->prev;
		free(trav->token);
		free(trav->type);
		free(trav);
	}
	pstack = NULL;
	stack_height = 0;
}

bool	legal_datatype(char *to_find)
{
	int i;

	i = 0;
	while (i < 13)
	{
		if (strcmp(datatypes[i], to_find) == 0)
			return (true);
		i++;
	}
	return (false);
}

bool	evaluate_datatype(t_token *token)
{
	bool flag;

	flag = false;
	if (datatype_len > 3)
		return (false_error(token, 3));
	if (legal_datatype(token->name) == false)
		return (false_error(token, 1));
	pstack = push_ptoken(pstack, token->name, token->type);
	datatype_len++;
	if (datatype_len > 0 && pstack)
	{
		if (token->next && strcmp(token->next->type, "DATATYPE") != 0)
		{
			if (is_datatype_correct(pstack, datatype_len) == false)
			{
				datatype_len = 0;
				return (false_error(token, 3));
			}
			datatype_len = 0;
			if (typing && strcmp(typing, "FUNCTION") == 0)
			{
				if (strcmp(token->next->type, "ID") == 0 || 
					strcmp(token->next->name, ",") == 0)
					return (true);
				else if (strcmp(token->next->type, "NUM") == 0 ||
					strcmp(token->next->type, "LITERAL") == 0)
					return (false_error(token, 16));
			}
		}	
	}
	return (true);	
}
bool	false_error(t_token *token, int message)
{
	extern bool inside_for;

	inside_for = false;
	printf("%s:%d: ", token->filename, token->line);
	if (message == 1) printf("error : expected ';', ',' or 'asm' before '%s' token\n\n", token->next->name);
	else if (message == 2) printf("error : unknown type name '%s'\n\n", token->next->name);
	else if (message == 3) printf("error : two or more data types in declaration specifiers\n\n");
	else if (message == 4) printf("error : expected declaration specifiers or '...' before '%s' token\n\n",
		token->next->name);
	else if (message == 5) printf("error : expected '}' at the end\n\n");
	else if (message == 6) printf("error : expected ',' or ';' before ')' token\n\n");
	else if (message == 7) printf("error : expected ';' before ')' token\n\n");
	else if (message == 8) printf("error : expected identifier or ')' before string or numeric constant\n\n");
	else if (message == 9) printf("error : expected identiifer or '(' before string or numeric constant\n\n");
	else if (message == 10) printf("error : expected identifier or '(' before '%s' token\n\n",
		token->next->name);
	else if (message == 11) printf("error : expected identifier or ')' at end of input\n\n");
	else if (message == 12) printf("error : expected ';', ',', '=' or 'asm' before '%s'\n\n",
		token->next->name);
	else if (message == 13) printf("error : expected identifier or '(' at the end of input\n\n");
	else if (message == 14) printf("error : expected declaration specifiers before '%s' token\n\n",
		token->next->name);
	else if (message == 15) printf("error : declaration for parameter '%s' but no such poarameter\n\n",
		token->name);
	else if (message == 16) printf("error : expectin ';', ',' or ')' before numeric or string constant\n\n");
	else if (message == 17) printf("error : expected '}' at the end of input\n\n");
	else if (message == 18) printf("error : expected '{' at the end of input\n\n");
	else if (message == 19) printf("error : expected identifier or '(' before numeric or string constant\n\n");

	else if (message == 20) printf("error : suze of array has non-integer type\n\n");
	else if (message == 21) printf("error : expected expression before '%s' token\n\n",
		token->next->name);
	else if (message == 22) printf("error : conflicting type or redefiniton of variable '%s'\n\n", this_var); 
	else if (message == 23) printf("error : '%s' undeclared (first used in this function)\n\n",
		token->name); 
	else if (message == 24) printf("error : called object '%s' is not a function or function pointer\n\n",
		token->name);
	else if (message == 25) printf("error : lvalue required as left operand of assignment\n\n");
	else if (message == 26) printf("error : array '%s' assumed to have one element\n\n", 
		token->name); 
	else if (message == 27) printf("error : expected expression before '%s' token\n\n",
		token->next->name);
	else if (message == 28) printf("error : expected ')' before '%s' token\n\n",
		token->next->name);
	clear_pstack();
	if (current_variable)
	{
		free_curr_var(current_variable);
		current_variable = NULL;
	}
	inside_for = false;
	return (false);
}

bool	evaluate_id(t_token *token)
{
	char *pointer_number;
	int flag;
	int call_trigger;
	pointer_number = NULL;

	call_trigger = false;
	if (!typing)
	{
		if (brackets < 0)
			return (false_error(token, 15));
	}
	else if (typing && strcmp(typing, "ASSIGN") == 0)
	{
		search_for_label(token->name, token->next->name);	
		return (true);
	}
	else if (typing && strcmp(typing, "CALL") == 0)
	{
		call_trigger = true;
		function_stack = push_to_stack(function_stack, token);
		if (token->next && strcmp(token->next->name, ")") == 0 ||
			strcmp(token->next->name, ",") == 0 ||
			sum_tokens(token->next->name) == true)
			return (true);
	}
	if (!pstack)
	{	
		if (token->next)
		{
			if (strcmp(token->next->name, "(") == 0)
			{
				if (call_trigger == false)
					function_stack = push_to_stack(function_stack, token);
				typing = strdup("CALL");
				return (true);	
			}
			else if (strcmp(token->next->name, "[") == 0)
			{
				return (true);
			}
			flag = search_for_label(token->name, token->next->name);
			if (flag == 1) return (false_error(token, 23));	
			else if (flag == 2) return (false_error(token, 24));
		}
	}
	if (typing && strcmp(typing, "FUNCTION") == 0)
	{
		if (brackets < 0) return (false_error(token, 15));
	}
	if (token->next && strcmp(token->next->name, "(") == 0)
	{
		pstack = push_ptoken(pstack, token->name, token->type);
		pstack->construct = strdup("FUNCTION");	
		typing = strdup("FUNCTION");
	}
	else if (token->next && strcmp(token->next->name, ";") == 0)
	{
		pstack = push_ptoken(pstack, token->name, token->type);
		pstack->construct = strdup("VARIABLE");	
		typing = strdup("VARIABLE");
	}
	else if (token->next && strcmp(token->next->name, "[") == 0)
	{
		pstack = push_ptoken(pstack, token->name, token->type);
		pstack->construct = strdup("VARIABLE");
		typing = strdup("VARIABLE");
	}
	else if (token->next && (sum_tokens(token->next->name) == true  || 
			equ_tokens(token->next->name) == true))
		//strcmp(token->next->name, "=") == 0)
	{
		pstack = push_ptoken(pstack, token->name, token->type);
		pstack->construct = strdup("ASSIGN");
		typing = strdup("ASSIGN");	
	}
	else if (token->next && strcmp(token->next->name, "]") == 0)
	{
		pstack = push_ptoken(pstack, token->name, token->type);
		pstack->construct = strdup("ASSIGN");
		typing = strdup("ASSIGN");
	}
	else if (token->next && strcmp(token->next->name, ",") == 0 ||
			strcmp(token->next->name, ")") == 0 || 
			strcmp(token->next->name, "]") == 0) 
	{
		if (!typing || strcmp(typing, "FUNCTION") != 0)
			return (false);
	}
	else
	{
		if (!pstack) return (false_error(token, 2));
		else return (false_error(token, 12));
	}
	if (asterisk_count > 0)
		pointer_number = itoa(asterisk_count);
	else 
		pointer_number = strdup("0");	
	current_variable = push_curr_var(current_variable, pointer_number);
	current_variable = push_curr_var(current_variable, token->name);
	free(pointer_number);
	asterisk_count = 0;
	return (true);
}

bool	peek(t_token *token)
{
	char *value;
	char **segments;
	int i;

	value = ht_search(ff_list, token->name);
	if (!value)
		value = ht_search(ff_list, token->type);
	if (!value)
		return (false);
	segments = split(value, ' ');
	i = 0;
	while (segments[i])
	{
		if (strcmp(token->next->name, segments[i]) == 0 ||
			strcmp(token->next->type, segments[i]) == 0)
			return (true);
		i++;
	}
	free2d(segments);
	return (false);
}

bool	evaluate_bracket(t_token *token)
{
	extern bool inside_for;

	asterisk_count = 0;
	round_bracket++;
	if (inside_for == true)
	{
		if (token->next && strcmp(token->next->name, ";") == 0)
			return (true);
	}
	if ((typing && strcmp(typing, "FUNCTION") != 0) &&
			strcmp(typing, "CALL") != 0 || !typing)
	{
		typecasting = true;
		typecast = (t_typecast *)malloc(sizeof(t_typecast));
		typecast->type = NULL;
		typecast->depth = 0;
	}
	if (typing && strcmp(typing, "FUNCTION") == 0)
	{
		if (token->next && strcmp(token->next->type, "DATATYPE") == 0 ||
			strcmp(token->next->name, ")") == 0)
		{
			return (true);
		}
		else if (token->next && strcmp(token->next->type, "NUM") == 0)
		{
			return (false_error(token, 4));
		}
		else if (token->next && strcmp(token->next->type, "ID") == 0)
		{
			return (false_error(token, 2));
		}
		else 
			return (false_error(token, 4));
	}
	else if (typing && strcmp(typing, "CALL") == 0)
	{
		function_stack = push_to_stack(function_stack, token);
		if (token->next && strcmp(token->next->type, "ID") == 0 ||
			strcmp(token->next->type, "NUM") == 0 || 
			strcmp(token->next->type, "LITERAL") == 0)
			return (true);
	}
	if (token->next && (strcmp(token->next->type, "DATATYPE") == 0 ||
		strcmp(token->next->name, ")") == 0))
	{
		return (true);
	} 
	else if (token->next && strcmp(token->next->type, "NUM") == 0)
	{
		return (true);
	}
	if (token->next && strcmp(token->next->type, "ID") == 0 ||
		strcmp(token->next->name, ";") == 0)
		return (true);
	else if (token->next && legal_datatype(token->next->name) == true)
	{
		typecasting = true;
		return (true);	
	}
	else if (token->next && legal_datatype(token->next->name) == false)
	{
		if (strcmp(token->name, "ID") != 0)
			return (false_error(token, 4));
		else return (false_error(token, 2));
	}
	return (true);
}

bool	evaluate_bracket2(t_token *token)
{
	extern bool inside_for;
	round_bracket--;

	if (typecasting == true)
	{
	//	printf("typecast name %d\n", typecast->depth);
	//	printf("typecast depth %s\n", typecast->type);
		typecasting = false;
		if (pstack)
			clear_pstack();
		if (token->next && strcmp(token->next->type, "ID") == 0 ||
			strcmp(token->next->type, "NUM") == 0 ||
			strcmp(token->next->type, "LITERAL") == 0)
		{
			if (current_variable) 
			{
				free_curr_var(current_variable); 
				current_variable = NULL;
			}
			asterisk_count = 0;
			return (true);
		}
		return (true);
	//	else { printf("incorrect typecast usage\n"); return (false);}
	}	
	if (round_bracket == 0)
	{
		if (inside_for == true) inside_for = false;	
	}
	if (typing && strcmp(typing, "FUNCTION") == 0) 
	{
		if (!token->next)
		{
			return (false_error(token, 17));
		}
		else if (token->next && strcmp(token->next->name, ";") == 0)
		{
			if (brackets > 0) return (false_error(token, 6));
			else if (brackets < 0) return (false_error(token, 7));
			// successful run : clean up and return true
			return (true);
		}
		else if (token->next && strcmp(token->next->name, ")") == 0 && brackets == 0)
		{
			printf("%s:%d: error : expected declaration specifier before ')' token\n\n"
				,token->filename, token->line);
		}
		else if (token->next && strcmp(token->next->name, "{") != 0 )
		{
			return (false_error(token, 18));
		}
	}
	else if (typing && strcmp(typing, "CALL") == 0)
	{
		function_stack = push_to_stack(function_stack, token);
	}
	return (true);
}

bool	evaluate_comma(t_token *token)
{
	if (typing && strcmp(typing, "FUNCTION") == 0)
	{
		symbol_table_manager(current_variable, typing); 
		free_curr_var(current_variable);
		current_variable = NULL;
		if (token->next && legal_datatype(token->next->name) == true)
			return (true);
		return (false_error(token, 4));
	}
	else if (typing && strcmp(typing, "CALL") == 0)
	{
		function_stack = push_to_stack(function_stack, token);
	}
	return (true);
}

bool	evaluate_number(t_token *token)
{
	extern t_token *left;
	extern t_token *right;

	if (typing && strcmp(typing, "FUNCTION") == 0) 
	{
		if (brackets > 0)
			return (false_error(token, 16));
		else
			return (false_error(token, 8));
	}
	else if (typing && strcmp(typing, "CALL") == 0)
	{
		function_stack = push_to_stack(function_stack, token);
		if (token->next)
		{
			if (strcmp(token->next->name, ")") == 0 ||
				strcmp(token->next->name, ",") == 0)
				return (true);
			else
			{
				return (false_error(token, 4));
			}
		}
	}
	else if (typing && strcmp(typing, "ASSIGN") == 0)
	{
		
		if (!left) {
			left = push_token(left, token->name, 
			token->type, -1, token->filename);	
		}
		else if (!right) {
			right = push_token(right, token->name, 
			token->type, -1, token->filename);
		}
	//	evaluate_equation();
		if (token->next && strcmp(token->next->name, ")") == 0)
		{
			return (true);
		}	
		if (token->next && equ_tokens(token->next->name) == true)
		{
			return (false_error(token, 25));
		}
		else if (token->next && sum_tokens(token->next->name) == true)
		{
			return (true);
		}
		else if (token->next && strcmp(token->next->name, ";") != 0)
		{
			return (false_error(token, 8));
		}

	}
	else if (!typing)
	{
		if (token->next && strcmp(token->next->name, ")") == 0 ||
			strcmp(token->next->name, "]") == 0 ||
			strcmp(token->next->name, ";") == 0 ||
			sum_tokens(token->next->name) == true)
		{
			return (true);
		}
		return (false_error(token, 19));
	}
	return (true);
}

bool	evaluate_asterisk(t_token *token)
{
	/*if (typecasting == true)
	{
		printf("TRUE\n");
		asterisk_count++;	
		if (token->next && strcmp(token->next->name, ")") == 0)
		{
			typecast->depth = asterisk_count;
			asterisk_count++;
		}
		if (token->next && strcmp(token->next->name, "*") == 0 ||
			strcmp(token->next->name, ")") == 0)
			return (true);
		else return (false_error(token, 28));
	} */
	if (typing && strcmp(typing, "CALL") == 0)
	{
		if (token->next && strcmp(token->next->type, "ID") == 0 ||
			strcmp(token->next->name, "*") == 0)
		{
			function_stack = push_to_stack(function_stack, token);
			asterisk_count++;
			return (true);
		}
	}
	else if (typing && strcmp(typing, "ASSIGN") == 0)
	{
		if (token->next && strcmp(token->next->type, "ID") == 0 ||
			strcmp(token->next->type, "NUM") == 0)
			return (true);
	}
	if (!pstack)
	{
		if (token->next && strcmp(token->next->name, "*") == 0 || 
			token->next && strcmp(token->next->type, "ID") == 0)
		{
			asterisk_count++;
			return (true);
		}
		else return (false_error(token, 21));	
	}
	if (typing && strcmp(typing, "FUNCTION") == 0)
	{
		if (token->next && strcmp(token->next->type, "NUM") == 0)
			return (false_error(token, 9));
	}
	else if (typing && strcmp(typing, "VARIABLE") == 0)
	{
		if (token->next && strcmp(token->next->type, "NUM") == 0)
			return (false_error(token, 9));
	}
	else if (token->next && strcmp(token->next->type, "ID") != 0 &&
			strcmp(token->next->name, "*") != 0)
	{
		return (false_error(token, 10));
	}
	else if (!token->next)
		return (false_error(token, 13));
	asterisk_count++;
	return (true);
}

bool	evaluate_block1(t_token *token)
{
	if (typing && strcmp(typing, "ASSIGN") == 0)
	{
		if (token->next && strcmp(token->next->name, "NUM") != 0 ||
			strcmp(token->next->name, "ID") != 0)
			return (false_error(token, 26));
		else
			return (true);
	}
	else
	{
		if (typing && strcmp(typing, "CALL") == 0)
		{
			function_stack = push_to_stack(function_stack, token);
		}
		if (token->next && strcmp(token->next->name, "]") == 0)
			return (true);
		else if (token->next && strcmp(token->next->type, "NUM") == 0)
			return (true);
		
		if (token->next && strcmp(token->next->type, "ID") == 0)
			return (true);
		if (token->next && strcmp(token->next->type, "NUM") != 0 ||
			strcmp(token->next->type, "ID") != 0 ||
			strcmp(token->next->name, "]") != 0)
			return (false_error(token, 20));
		else if (token->next && strcmp(token->next->type, "ID") == 0)
			printf("Check the type of this variable\n");
		else if (token->next && strcmp(token->next->name, "]") == 0)
			return (true);
		else 
			return (false_error(token, 21));
	}
	return (false_error(token, 10));
}

bool	evaluate_block2(t_token *token)
{
	if (typing && (strcmp(typing, "VARIABLE") == 0 || strcmp(typing, "ASSIGN") == 0))
	{
		asterisk_count++;
		if (token->next && strcmp(token->next->name, "[") == 0 ||
			strcmp(token->next->name, "=") == 0 ||
			strcmp(token->next->name, ";") == 0)
				return (true);
		else if (token->next && strcmp(token->next->type, "NUM") == 0 ||
			strcmp(token->next->type, "LITERAL") == 0)
			return (false_error(token, 12));		
	}
	else if (typing && strcmp(typing, "CALL") == 0)
	{
		function_stack = push_to_stack(function_stack, token);
		return (true);
	}
	else if (token->next && strcmp(token->next->name, ";") == 0)
	{
		return (true);
	}
	return (false_error(token, 1));
}

bool	evaluate_equ(t_token *token) 
{
	char *symbol_type;
	t_current_var *trav;
	int symtab_manager;
	extern t_token *left;

	if (asterisk_count > 0)
		current_variable = add_index_depth(current_variable, asterisk_count);
	trav = current_variable;
	if (current_variable && current_variable->next->next)
		left = push_token(left, current_variable->next->next->str, "ID", -1, token->filename);
	symtab_manager = symbol_table_manager(current_variable, typing);
	free_curr_var(current_variable);
	current_variable = NULL;
	asterisk_count = 0;	
	if (symtab_manager == 1) return (false_error(token, 22));
	if (typing && strcmp(typing, "VARIABLE") == 0)
	{
		if (token->next && strcmp(token->next->name, "{") == 0)
		{
			free(typing);
			typing = strdup("ARRAY");
			return (true);
		}
		else if (token->next && strcmp(token->next->type, "ID") == 0)
		{
			if (symbol_type && strcmp(symbol_type, "variable") == 0)
			{
				free(typing);
				typing = strdup("ASSIGN");
			}
			else if (symbol_type && strcmp(symbol_type, "function") == 0)
			{
				free(typing);
				typing = strdup("CALL");
			}
			else if (strcmp(token->next->type, "LITERAL") == 0 || 
				strcmp(token->next->type, "NUM") == 0)
			{
				free(typing);
				typing = strdup("ASSIGN");
			}
			return (true);		
		}	
	}
	else if (typing && strcmp(typing, "ASSIGN") == 0)
	{
		if (token->next && strcmp(token->next->type, "NUM") == 0 ||
			strcmp(token->next->type, "ID") == 0)
		{
			return (true);
		}
	}
	return (true);
}

bool	evaluate_semicolon(t_token *token)
{
	int symtab_manager;
	extern t_token *left;
	extern t_token *right;
	extern bool inside_for;
	extern int forloop_count;

	if (inside_for == true)
	{
		if (forloop_count < 2)
			forloop_count++;
		else 
			return (false_error(token, 26));
	}
	if (strcmp(token->name, ";") == 0)
	{
		// print current_var
		if (asterisk_count > 0 && current_variable)
			current_variable = add_index_depth(current_variable, asterisk_count);
		t_current_var *trav = current_variable;
		symtab_manager = symbol_table_manager(current_variable, typing);	
		if (current_variable)
		{
			free_curr_var(current_variable);
			current_variable = NULL;
		}
		if (typing)	
		{
			free(typing);
			typing = NULL;
		}
		if (function_stack)
		{
			free_tokens(function_stack);
			function_stack = NULL;
		}
		asterisk_count = 0;
		if (left) 
		{
			free(left);
			left = NULL;
		}
		if (right)
		{
			free(right);
			right = NULL;
		}
		if (symtab_manager == 1) return (false_error(token, 22));
		clear_pstack();
		return (true);
	}
	return (false);
}

bool	evaluate_curly(t_token *token)
{
	extern int scope_depth;
	scope_depth++;
	if (typing && strcmp(typing, "FUNCTION") == 0)
	{
		free(typing);
		typing = NULL;
		free_curr_var(current_variable);
		current_variable = NULL;
		clear_pstack();
		if (token->next && strcmp(token->next->type, "ID") == 0 ||
			strcmp(token->next->type, "DATATYPE") == 0 ||
			strcmp(token->next->name, "*") == 0 ||
			strcmp(token->next->name, "}") == 0 ||
			strcmp(token->next->name, "NUM") == 0 ||
			strcmp(token->next->name, "for") == 0)
			return (true);
			
	}
	return (true);
}

bool	evaluate_curly2(t_token *token)
{
	extern int scope_depth;

	if (scope_depth == 0)
	{	
		if (token->next == NULL)
			return (true);
		return (false_error(token, 10));
	}
	drop_scope_block();
	if (typing)
	{
		free(typing);
		typing = NULL;
	}
	if (current_variable)
	{
		free_curr_var(current_variable);
		current_variable = NULL;
	}
	clear_pstack();
	return (true);
}

bool	evaluate_sum(t_token *token)
{

	if (token->next && strcmp(token->next->type, "ID") == 0 ||
		strcmp(token->next->type, "NUM") == 0)
	{
		if (typing && strcmp(typing, "CALL") == 0)
		{
			function_stack = push_to_stack(function_stack, token);
		}
		return (true);
	}
	else
		return (false_error(token, 21));
}

void	error_cleanup(void)
{
	if (current_variable)
	{
		free_curr_var(current_variable);
		current_variable = NULL;
	}
	if (typing)
	{
		free(typing);
		typing = NULL;
	}
	asterisk_count = 0;
}

t_tree	*parser(t_token *token)
{
	t_token *trav;
	bool guidance;
	t_tree *ast;
	t_token *tree_piece;
	char *block_name;
	int curly_count;

	curly_count = -1;
	tree_piece = NULL;
	ast = NULL;
	guidance = true;
	trav = token;
	while (trav)
	{
		if (strcmp(trav->name, "(") == 0) brackets++;
		else if (strcmp(trav->name, ")") == 0) brackets--;

		// these tokens are allowed to start off a phrase
		if (strcmp(trav->type, "DATATYPE") == 0) guidance = evaluate_datatype(trav);
		else if (strcmp(trav->type, "ID") == 0) guidance = evaluate_id(trav);
		if (guidance == true)
		{
			if (strcmp(trav->name, "(") == 0) guidance = evaluate_bracket(trav);
			else if (strcmp(trav->name, "*") == 0) guidance = evaluate_asterisk(trav);
			else if (strcmp(trav->name, ")") == 0) guidance = evaluate_bracket2(trav);
			else if (strcmp(trav->name, ",") == 0) guidance = evaluate_comma(trav);
			else if (strcmp(trav->type, "NUM") == 0 || strcmp(trav->type, "CHAR") == 0 ||
				strcmp(trav->type, "LITERAL") == 0) 
				guidance = evaluate_number(trav);
			else if (sum_tokens(trav->name) == true) guidance = evaluate_sum(trav);
			else if (equ_tokens(trav->name) == true) guidance = evaluate_equ(trav);
			else if (strcmp(trav->name, ";") == 0) guidance = evaluate_semicolon(trav);
			else if (strcmp(trav->name, "[") == 0) guidance = evaluate_block1(trav);
			else if (strcmp(trav->name, "]") == 0) guidance = evaluate_block2(trav);
			else if (strcmp(trav->name, "{") == 0) guidance = evaluate_curly(trav);
			else if (strcmp(trav->name, "}") == 0) guidance = evaluate_curly2(trav);
			else if (strcmp(trav->name, "if") == 0) guidance = evaluate_if(trav);
			else if (strcmp(trav->name, "else") == 0) guidance = evaluate_else(trav);
			else if (strcmp(trav->name, "while") == 0) guidance = evaluate_while(trav);
			else if (strcmp(trav->name, "return") == 0) guidance = evaluate_return(trav);
			else if (strcmp(trav->name, "for") == 0) guidance = evaluate_for(trav);
			if (guidance == false) 
				error_cleanup();
		}
		if (strcmp(trav->name, "{") == 0)
		{
			if (curly_count == -1) curly_count = 1;
			else curly_count++;
		}
		else if (strcmp(trav->name,";") == 0)
		{
			block_name = get_syntactic_name(tree_piece);
			ast = push_tree(ast, block_name, tree_piece, curly_count);
			free(block_name);
			tree_piece = NULL;
		}
		else if (strcmp(trav->name, "}") == 0)
		{
			curly_count--;
			if (curly_count == 0)
			{	
				//ast = push_tree(ast, "COMPONENT", tree_piece);
				tree_piece = NULL;
				curly_count = -1;
			}
		}
	        else if (strcmp(trav->name, ";") != 0)
		{
			tree_piece = push_token(tree_piece, trav->name,
				trav->type, trav->line, trav->filename);
		}
		trav = trav->next;
	}
	return (ast);
}
