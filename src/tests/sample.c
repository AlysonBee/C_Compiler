/*
#include "../inc/typing.h"
#include "../inc/token.h"
#include <ctype.h>
#include "../inc/database.h"
#include "../inc/semantic.h"
#include "../inc/compiler.h"
#define FUNCTION 1
#define WHILE 2
#define ARR 7
//#define IF 3
#define FOR 4
//#define DO 5 
#define OMMITTED_VARIABLE "0XAE42D3FFF"
#define FUNCTION_CALL "0x241F1S9AE"

t_hashtable	*datatype_reference = NULL;
t_this_type	*current_type = NULL;
t_struct *all_structs = NULL;
t_function *functions = NULL;
t_token	*to_evaluate = NULL;
char	**start = NULL;
t_stack *stack = NULL;
*/
int	value_found(char *value, char **to_scan)
{
	int		i;
	char		**double_words;

	i = 0;
	while (to_scan[i])
	{
		if (strcmp(value, to_scan[i]) == 0)
		{
			return (true);
		}
		i++;
	}
	return (false);
} 



bool	validate_literal(char *str)
{
	if (str[0] == '8' && str[strlen(str) - 1] == 'o')
	{
		return (true);
	}
	return (false);
}

bool	validate_num(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (!isdigit(str[i]))
		{
			return (false);
		}
		i++;
	}
	return (true);
}

t_temp_var      *create_temp_var(t_token *token)
{
        int             depth;
        extern int      max_number;
        extern t_db     *list;
        t_hashtable     *variables;
        t_temp_var      *temp;
	t_token		*trav;
	char		*struct_manager;

        temp = (t_temp_var *)malloc(sizeof(t_temp_var));
        temp->depth = 0;
	temp->name = NULL;
	temp->type = NULL;
        trav = token;
	if (strcmp(trav->name, "struct") == 0)
	{
		struct_manager = join_with_space(trav->name, trav->next->name);
		if (value_found(struct_manager, start) == true)
		{
			temp->type = strdup(struct_manager);
			trav = trav->next->next;
		}
	}
        else if (temp->type = valid_datatypes(trav)) //value_found(trav->name, start) == true)
        {
               // temp->type = strdup(trav->name);
		trav = skip_distance(trav, temp->type);
                trav = trav->next;
        }
        else
	{
		temp->type = NULL;
	}
        while (strcmp(trav->name, "*") == 0)
        {
                temp->depth++;
                trav = trav->next;
        }
        if (strcmp(trav->type, "ID") == 0)
        {
                temp->name = strdup(trav->name);
                trav = trav->next;
        }
        else
	{
		temp->name = NULL;
	}
        temp->curr = trav;
        return (temp);
}

void	free_temp_var(t_temp_var *block)
{
	free(block->name);
	free(block->type);
	free(block);
}


bool	save_function(t_temp_var *temp_var, t_token *trav, char *function_name) 
{
	bool	ommitted_variable;
	t_fvars	*variables;
	extern char **start;
	t_function *sanity_ftest;
	t_fvars	**sanity_vtest;
	int v_param_count;

	// sanity testing
	v_param_count = 0;

	sanity_vtest = NULL;
	ommitted_variable = false;
	variables = NULL;

	// sanity testing
	sanity_ftest = get_function(function_name);

	if (!sanity_ftest)
		functions = push_function(functions, temp_var->name, temp_var->type, temp_var->depth);
       	if (strcmp(trav->next->name, "void") == 0 && strcmp(trav->next->next->name, ")") == 0 &&
         	(strcmp(trav->next->next->next->name, ";") == 0 ||
        	strcmp(trav->next->next->next->name, "{") == 0))
         	return (true);

        trav = trav->next;
        while (trav && strcmp(trav->name, ";") && strcmp(trav->name, "{"))
        {
        	if (strcmp(trav->name, ",") == 0 || strcmp(trav->name, ")") == 0)
                {
                	if (!temp_var->name || !temp_var->type)
                        {
                        	printf("error : incorrect variable naming convention\n");
                                return (false);
                        }
                        variables = create_new_parameter(temp_var->name, temp_var->type, temp_var->depth);
		     
			if (sanity_ftest)
			{
				sanity_vtest = add_to_param_list(sanity_vtest, variables, v_param_count);
				v_param_count++;
			}
			else
				functions = new_parameter(functions, function_name, variables);
                        param_free(variables);
                        free_temp_var(temp_var);
                        if (strcmp(trav->name, ")") == 0)
                        {
                        	trav = trav->next;
                                break;
                        }
                        trav = trav->next;
                 }
                 else if (value_found(trav->name, start))
                 {
                 	temp_var = create_temp_var(trav);
        		if (!temp_var->name || !temp_var->type)
			{
				trav = error_mode(trav, "Invalid syntax");
				return (false);
			}
		        trav = temp_var->curr;
                 }
		 else
		 {
			trav = error_mode(trav, "Invalid syntax");
			return (false);
		 }
        }
	
	if (sanity_ftest)
	{
		handle_redefinition(sanity_ftest, sanity_vtest, v_param_count,
			trav->name);
	}
	return (true);
}


t_temp_var	*validate_variable_call(t_token *token)
{
	t_db	*object;
	t_token	*trav;
	int 	deref_depth;
	bool	ampersand;
	char	*name;
	t_temp_var *temp_var;

	ampersand = false;
	name = NULL;
	deref_depth = 0;
	trav = token;
	temp_var = (t_temp_var *)malloc(sizeof(t_temp_var));
	temp_var->name = NULL;
	temp_var->depth = 0;
	temp_var->type = NULL;
	if (strcmp(trav->name, "&") == 0)
	{
		ampersand = true;
		trav = trav->next;
	}
	while (strcmp(trav->name, "*") == 0 && ampersand == false)
		trav = trav->next;
	if (strcmp(trav->type, "ID") == 0)
	{
		name = strdup(trav->name);
		trav = trav->next;
	}
	else
	{
		printf("Error : Syntax error\n");
		trav = trav->next;
	}
	if (name)
	{
		printf("name is %s\n", name);
		object = get_object_from_db(name);
		if (object)
		{
			printf("Error : Variable doesn't exist\n");
			temp_var->depth = object->depth;
			temp_var->type = strdup(object->type);
		}
		temp_var->name = name;
		temp_var->depth += deref_depth;
		temp_var->curr = trav;
	}
	return (temp_var);
}
/*

t_token	*insepct_token(t_token *token, t_token *back)
{
	t_token 	*type;
	t_funciton	*function;
	int		brackets;

	type = NULL;
	if (strcmp(token->next, "(") == 0)
	{
		brackets = 1;
		token = token->next;
 		if (strcmp(token->next,
		validate_function(token);
		function = get_function();
		type = push_token(type,
			function->tyep,
			function->function_name,
			FUNCTION_CALL,
			function->depth
		);
	}
	else
	{
		type = push_token(type,
			back->type,
			back->name,
			OMMITTED_VARIABLE,
			back->depth
		);
					
	}	
}*/


bool	validate_call(t_temp_var *temp, t_token *trav, char *function_name)
{
	char 		*name;
	t_db		*variables;
	t_db		*object;	
	t_token		*back;
	t_token		*typelist;

	typelist = NULL;
	variables = NULL;
	trav = trav->next;
	while (trav && strcmp(trav->name, ";") != 0)
	{
		
		if (strcmp(trav->name, ",") == 0 || strcmp(trav->name, ")") == 0)
		{
			if (strcmp(back->type, "ID") == 0)
			{
				object = get_object_from_db(back->name);
				if (!object)
				{
					printf("Error : variable doesn't exist\n");
					break ;
				}
				typelist = push_token(typelist, object->name, object->type,
					back->line, back->filename);
				variables = push_object(
					variables,
					object->type,
					object->name,
					"none",
					object->depth
				);
				free(object->type); free(object->name); free(object);
			}
			else 
			{
				typelist = push_token(typelist, back->name, back->type,
					back->line, back->filename);
				printf("type is %s\n", back->type);
				variables = push_object(
					variables,
					back->type,
					"rbx",
					OMMITTED_VARIABLE,
					0
				);
			}
		}
		back = trav;
		trav = trav->next;
	}
	if (assert_parameter_correctness(typelist, function_name) == true)
		printf("CORRECT\n");
	else 
		printf("INCORRECT\n");
}

bool	equ_token(char *name)
{
	char list[10][10] = {
		"+=\0",
		"-=\0",
		">>=\0",
		"<<=\0",
		"!=\0",
		"&=\0",
		"|=\0",
		"=\0"
	};
	int	i;

	i = 0;
	while (i < 8)
	{
		if (strcmp(name, list[i]) == 0)
			return (true);
		i++;
	}
	return (false);
}

bool	validate_function(t_token *token)
{
	t_token 	*trav;
	char		*possible_function_name;
	extern t_db	*list;
	t_temp_var 	*temp_var;
	char		*value;
	bool		to_check;
	bool		called;
	extern char 	*g_function_name;

	called = false;
	to_check = NULL;
	if (strcmp(token->type, "ID") == 0 || strcmp(token->name, "*") == 0 ||
		strcmp(token->name, "@") == 0 || strcmp(token->name, "(") == 0)
	{
		if (equ_token(token->next->name) == true)
		{
			token = token->next->next;
			value = value_checker(token);
			return (true);
		}
		called = true;
		to_check = does_variable_exist(token->name);
		if (strcmp(token->name, "*") != 0 && strcmp(token->name, "(") != 0 && strcmp(token->name, "@") != 0 && 
			!does_variable_exist(token->name) && !does_function_exist(token->name))
		{
			token = error_mode(token, " : variable doesn't exist");
			return (false);
		}
		is_valid_equation(token, ";");
		return (true);
	}
	else 
	{
		temp_var = create_temp_var(token);
		if (temp_var->name == NULL)
			return (false);
	}

	possible_function_name = strdup(temp_var->name);
	trav = temp_var->curr;
	if (strcmp(trav->name, ";") == 0)
		insert_into_db(temp_var->type, temp_var->name, NULL, temp_var->depth);
	else if (strcmp(trav->name, "(") == 0)
	{
		g_function_name = strdup(possible_function_name);
		if (called == true)
			validate_call(temp_var, trav, possible_function_name);
		else
		{
			save_function(temp_var, trav, possible_function_name);
		}
	}
	else if (strcmp(trav->name, "=") == 0)
	{
		trav = trav->next;
		value = value_checker(trav);
		if (called == true)
			update_variable_value(temp_var->name, value);	
		else
			insert_into_db(temp_var->type, temp_var->name, value, temp_var->depth);
	}
	return (true);
}


char	*determine_token_type(char *token)
{
	if (validate_id(token) == true)
		return (strdup("ID"));
	else if (validate_literal(token) == true)
		return (strdup("LITERAL"));
	else if (validate_num(token) == true)
		return (strdup("NUM"));
	else if (token[0] == '\'' && token[strlen(token) - 1] == '\'')
		return (strdup("CHAR"));
	return (strdup(token));
}


bool	check_next_token(t_hashtable *ff_list, char *next_token, char *current_token)
{
	char	*name;	
	char 	*value;	
	char 	*second;
	char 	**pieces;	

	name = determine_token_type(current_token);
	value = ht_search(ff_list, name);
	if (value)
	{
		if (strcmp(value, "DONE") == 0)
		{
			printf("TRUE DONE\n");
			return (true);
		}
		pieces = split(value, ' ');
		second = determine_token_type(next_token);
		if (value_found(second, pieces) == true)
			return (true);
		
	}
	return (false);
}

bool	scan_commands(char **commands, char *to_find)
{
	int i;

	i = 0;
	while (commands[i])
	{
		if (strcmp(commands[i], to_find) == 0)
			return (true);
		i++;
	}
	return (false);
}

char	**command_blocks(void)
{
	char **blocks;
	char *list = "for while if";
	blocks = split(list, ' ');
	return (blocks);
}

 
bool	assert_inner_loop(void)
{
	if (stack)
	{
		if (stack->scope_name == FOR || stack->scope_name == WHILE ||
			stack->scope_name  == DO)
			return (true);
	}
	return (false);
}


void	save_sizeof(void)
{
	t_function *all_functions;
	t_fvars *parameters;

	parameters = create_new_parameter("n", "int", 0);
	all_functions = push_function(all_functions, "sizeof", "size_t", 0);
	functions = new_parameter(all_functions, "sizeof", parameters);
	param_free(parameters);
}

bool	semantic_analysis(t_token *tokens)
{
	t_token		*trav;
	char		**next;
	char		**commands;
	char		*prev;
	bool		entering_command_block;
	bool		in_function;
	t_hashtable	*ff_list;
	t_token		*head;
	extern int	max_number;
	int		brackets;
	t_token		*error;	
	t_token		*back;
	bool		IS_ARR;
	bool		do_trigger;

	do_trigger = false;
	IS_ARR = false;
	error = NULL;
	brackets = 0;
	max_number= 0;
	in_function = false;
	start = split("char bool const void struct int short double float size_t long signed void unsigned", ' ');
        ff_list = first_and_follow();
//	save_sizeof();
	commands = command_blocks();
	if (value_found(tokens->name, start) == false)
	{
		printf("Your code is shit and you deserve to die\n");
		return (false);
	}
	entering_command_block = false;
	trav = tokens;
	head = trav;
	while (trav)
	{
		if (strcmp(trav->name, "{") == 0 )
		{	
			if (prev && strcmp(prev, "=") == 0)
				IS_ARR = true;
			brackets++;
		}
		else if (strcmp(trav->name, "}") == 0)
		{
			IS_ARR = false;
			//if (!(trav->next && strcmp(trav->next->name, ";") == 0))
			brackets--;
		}
		if (strcmp(trav->name, "extern") == 0)
		{

		}
		else if (handle_native_csg(prev, trav->name) == 3)
		{	
			if (!(trav->next && strcmp(trav->next->name, ";") == 0))
				drop_last_table();
		//	stack = pop_stack(stack);
		}
		else if (strcmp(trav->name, "continue") == 0 || strcmp(trav->name, "break") == 0)
		{
			if (assert_inner_loop() == false)
				error_mode(trav, "cannot have 'continue' or 'break' outside of scope loop");
		}
		else if (strcmp(trav->name, "sizeof") == 0)
		{
			trav = semantic_sizeof(trav);
		}
		else if (strcmp(trav->name, "return") == 0)
		{
			if (stack == NULL)
			{
				error_mode(trav, "return statement outside of function block");
			}
		}
		else if (strcmp(trav->name, "struct") == 0 && strcmp(back->name, "(") != 0)
		{
			printf("struct checker\n");
			print_x(trav, 6);
			trav = struct_loop(trav);	
			if (strcmp(trav->name, ";") != 0)
				trav = error_recover(trav, "Missing semicolon", 
					push_token(error, ";", "SEMICOLON", trav->line, trav->filename));	
			printf("after structn\n");
			print_x(trav, 6);
			head = trav->next;
		}
		else if (strcmp(trav->name, "do") == 0)
		{
			do_trigger = false;
		}
		else if (value_found(trav->name, commands))
		{
			if (strcmp(trav->name, "for") == 0)
				trav = semantic_for(prev, trav, ff_list);
			else if (strcmp(trav->name, "while") == 0)
			{
				trav = semantic_while(prev, trav, ff_list, do_trigger);
			}
			else if (strcmp(trav->name, "if") == 0) 
			{
				trav = semantic_if(prev, trav, ff_list);
			}
			else if (strcmp(trav->name, "else") == 0)
				trav = semantic_else(prev, trav, ff_list);
			
			stack = push_stack(stack, FOR);
			add_new_table();
			head = trav;
			if (head && strcmp(head->name, "{") == 0)
			{
				brackets++;
				head = head->next;
			}
			entering_command_block = false;
		}
		else if (handle_native_csg(prev, trav->name) == SCOPE
			 || strcmp(trav->name, ";") == 0)
		{
			if (strcmp(head->name, "}") == 0)
				head = head->next;
			validate_function(head);
			head = trav;
			if (trav && strcmp(trav->name, "{") == 0)
			{
				set_defined(NULL, 2);
				add_new_table();
			}
			if (head && strcmp(head->name, ";") == 0)
			{
				set_defined(NULL, 1);
				head = head->next;
			}
			else if (head && strcmp(head->name, "{") == 0 && in_function == false)
			{
				stack = push_stack(stack, FUNCTION);
				head = head->next;
				in_function = true;
			}
		}
		else if (trav->next && check_next_token(ff_list, trav->next->name, trav->name) == true)
		{
			if (strcmp(trav->name, ",") == 0 && IS_ARR == true && value_found(trav->next->name, start))
			{
				error = NULL;
				trav = forward_recovery(trav, "can only have literals in arrays",
					push_token(error, "'CHAR'", "CHAR", trav->line, trav->filename)); 
			}
			if (scan_commands(commands, trav->name) && in_function == true)	
				entering_command_block = true;
			else if (scan_commands(commands, trav->name) && in_function == false)
				printf("error : can't have %s outside function scope\n",
					trav->name);
			prev = trav->name;
		}
		else
		{
			trav = panic_mode(trav, back, brackets);
			if (!trav)
				break ;
		}
		if (brackets < 0)
		{
			printf("Error : curly brace mismatch\n\n");
		}
		if (strcmp(trav->name, "}") && trav->next == NULL)
			break ;
		back = trav;
		prev = strdup(trav->name);
		trav = trav->next;
	}
	if (brackets != 0)
	{
		error_mode(back, "bracket mismatch in program");
	}
	printf("SUMMARY =====\n");
	print_variables();
	print_structs(all_structs);
	print_functions(functions); 
	return (true);
}

