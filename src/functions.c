



#include "../inc/compiler.h"
#include "../inc/semantic.h"
/*
typedef struct	s_function_list
{
	char 	*function_name;
	char	**parameters;
	struct s_function_list *next;
} 	t_function;



*/

t_function *new_function(char *function_name, char *type, int depth)
{
	t_function *new;

	new = (t_function *)malloc(sizeof(t_function));
	new->function_name = strdup(function_name);
	new->type = strdup(type);
	new->depth = depth;
	new->parameters = NULL;
	new->param_number = 0;
	return (new);	
}

t_function	*add_function(t_function *head, char *function_name, char *type, int depth)
{
	t_function *trav;
	
	trav = head;
	while (trav->next)
		trav = trav->next;
	trav->next = new_function(function_name, type, depth);
	return (trav);
}

t_function	*push_function(t_function *head, char *function_name, char *type, int depth)
{
	t_function *trav;

	trav = head;
	if (trav == NULL)
	{
		trav = new_function(function_name, type, depth);
	}
	else
	{
		trav = add_function(head, function_name, type, depth);
		trav = head;
	}
	return (trav);
}

t_fvars		*create_new_parameter(char *name, char *type, int depth)
{
	t_fvars	*head;

	head = (t_fvars *)malloc(sizeof(t_fvars));
	head->name = strdup(name);
	head->type = strdup(type);
	head->depth = depth;
	head->size = 0; // Make a function that takes a datatuype and outputs its size.
	return (head);
}

void		param_free(t_fvars *parameter)
{
	free(parameter->name);
	free(parameter->type);
	free(parameter);
}

void		free_parameter(t_fvars **params, size_t list_size)
{
	size_t	i;

	i = 0;
	while (i < list_size)
	{
		free(params[i]->name);
		if (params[i]->value);
			free(params[i]->value);
		free(params[i]->type);
		free(params[i]);
		i++;
	}
}	

t_fvars		**add_to_param_list(t_fvars **array, t_fvars *to_add, size_t list_size)
{
	int	i;
	t_fvars	**new_array;
	
	new_array = NULL;
	if (array == NULL)
	{
		new_array = (t_fvars **)malloc(sizeof(t_fvars *) * 2);
		new_array[0] = create_new_parameter(to_add->name,
			to_add->type, to_add->depth);
		new_array[1] = NULL;
	}
	else
	{	
		new_array = (t_fvars **)malloc(sizeof(t_fvars *) * (list_size + 2));
		i = 0;
		while (i < list_size)
		{
			new_array[i] = create_new_parameter(
				array[i]->name,
				array[i]->type,
				array[i]->depth);
			i++;
		}
		new_array[i] = create_new_parameter(to_add->name, to_add->type,
			to_add->depth);
		new_array[++i] = NULL;
	}
	return (new_array);
}

t_function 	*new_parameter(t_function *all_functions, char *function_name, t_fvars *new_param)
{
	t_function *this_function;

	this_function = all_functions;
	while (this_function)
	{
		if (strcmp(this_function->function_name, function_name) == 0)
			break;
		this_function = this_function->next;
	}
	if (this_function == NULL)
		printf("You're quite the hackerman, aren't you\n");
	this_function->parameters = add_to_param_list(
		this_function->parameters,
		new_param, 
		this_function->param_number
	);
	this_function->param_number++;
	return (all_functions);
}

// char	**split(char *str, char c);
// char *join(char *s1, char *s2);

int	main(void)
{
	t_function *functions = NULL;
	t_fvars *parameters = NULL;
	
	functions = push_function(functions, "split", "char", 2);
	parameters = create_new_parameter("str", "char", 1);
	
	functions = new_parameter(functions, "split", parameters);
	param_free(parameters);

	parameters = create_new_parameter("c", "char", 0);
	functions = new_parameter(functions, "split", parameters);
	param_free(parameters);	

	functions = push_function(functions, "join", "char", 1);
	
	parameters = create_new_parameter("s1", "char", 1);
	functions = new_parameter(functions, "join", parameters);
	param_free(parameters);

	parameters = create_new_parameter("s2", "char", 1);
	functions = new_parameter(functions, "join", parameters);
	param_free(parameters);
	printf("function name %s\n", functions->function_name);
	printf("parameter 1 : %s\n", functions->parameters[0]->name);
	printf("parameter 2 : %s\n", functions->parameters[1]->name);

	printf("function name %s\n", functions->next->function_name);
	printf("parameter 1 : %s\n", functions->next->parameters[0]->name);
	printf("parameter 2 : %s\n", functions->next->parameters[1]->name);
	return (0);
}












