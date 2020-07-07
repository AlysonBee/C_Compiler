
#include "../inc/semantic.h"
#include "../inc/token.h"
#include "../inc/symbol.h"
#include "../inc/compiler.h"

#define FUNCTION 1
#define VARIABLE 2

t_scope             *g_symbol_table = NULL;
t_function_table    *function_talbe = NULL;

t_token    *skip_stars(t_token *token)
{
    while (token && strcmp(token->name, "*") == 0)
        token = token->next;
    if (token)
        return (token);
    return (NULL);
}

int     variable_or_function(t_token *tokens)
{
    t_token     *list;
    t_token     *head;

    list = tokens;
    if (assert_first_order(list->name) == true)
    {
        if (assert_second_order(list->next->name) == true)
            list = list->next->next;
    }
    else if (assert_second_order(list->name) == true)
    {
        list = list->next;
    }
    else
        return (-1);
    if (list && strcmp(list->name, "*") == 0)
        list = skip_stars(list);
    if (strcmp(list->type, "ID") == 0)
        list = list->next;
    if (strcmp(list->name, "(") == 0)
    {
        return (1);
    }
    if (strcmp(list->name, ";") == 0 || strcmp(list->name, "=") == 0)
    {
        printf("2\n");
        return (2);
    }
    return (-1);
}

t_token   *create_variable(t_token **list)
{
    t_variable  *variable;
    t_scope     *local_sym;
    t_token     *token;
    t_variable  *local_var;

    token = *list;
    variable = save_variable(&token, ";");
    free(variable->name);
    free(variable->datatype);
    free(variable->value);
    free(variable);
    return (token);
}

void    semantic_analysis(t_token *tokens)
{
    t_token *list;
    int     counter;
    extern t_scope *g_symbol_table;

    counter = 0;
    list = tokens;
    while (list)
    {
       if (variable_or_function(list) == 2)
       {
           save_variable(&list, ";");
             
       }
       list = list->next;
    }
}


int     main(void)
{
    t_token *list;

    g_symbol_table = (t_scope *)malloc(sizeof(t_scope));
    g_symbol_table->depth = 0;
    g_symbol_table->variables = NULL;
    g_symbol_table->next = NULL;
    char test[] = "char *hello_world = \"42\";int again = 42;void *value = NULL;"; 
    list = lexer(test);
    semantic_analysis(list);
    return (0);
}







