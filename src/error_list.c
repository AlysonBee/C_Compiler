
#include "../inc/semantic.h"
#include "../inc/token.h"
#include "../inc/compiler.h"


void	print_error_message(t_token *position)
{
	t_file	*trav;
	extern t_file *files;

	trav = files;
	while (trav)
	{
		if (strcmp(position->filename, files->filename) == 0)
		{
			printf(": %s\n\n", trav->content[position->line - 1]);
			break;
		}
		trav = trav->next;
	}
}

t_token	*panic_mode(t_token *trav, t_token *back, int brackets)
{
	t_token	*error;
	extern t_stack *stack;
	extern char **start;

	if (strcmp(trav->type, "ID") == 0 || strcmp(trav->type, "NUM") == 0 || 
		strcmp(trav->type, "LITERAL") == 0 || strcmp(trav->type, "CHAR") == 0)
       	{
		if (stack && stack->scope_name == 4)
                {
               		error = NULL; printf("%s %d ", trav->filename, trav->line);
                       	trav = error_recover(trav, "missing semilcolon",
                        	push_token(error, ";", "SEMICOLON", trav->line, trav->filename));       				    print_error_message(trav);	
                }
                else if (brackets != 0)
                {
                       	error = NULL; printf("%s %d ", trav->filename, trav->line);
                        trav = forward_recovery(trav, "missing a closing '}'",
                        	push_token(error, "}", "CLOSINGBRACKET", trav->line, trav->filename));
                	print_error_message(trav); 
		}
                 else
                 {
                 	error = NULL; printf("%s %d ", trav->filename, trav->line);
                        trav = forward_recovery(trav, "missing semicolon",
                        	push_token(error, ";", "SEMICOLON", trav->line, trav->filename));
                	print_error_message(trav);
		}
         }
         else if (strcmp(trav->name, "}") == 0)
         {
          	error = NULL; printf("%s %d ", trav->filename, trav->line);
          	trav = forward_recovery(trav, "missing semicolon",
                	push_token(error, ";", "SEMICOLON", trav->line, trav->filename));
        	print_error_message(trav); 
	}
         else if (strcmp(trav->name, ",") == 0)
         {
          	if (strcmp(back->type, "CHAR") == 0)
                {
                	error = NULL; printf("%s %d ", trav->filename, trav->line);
                        trav = forward_recovery(trav, "incomplete list",
                                push_token(error, "'C'", back->type, trav->line, trav->filename));
                	print_error_message(trav);
		}
         }
         else if (value_found(trav->name, start) == true)
         {
          	error = NULL; printf("%s %d ", trav->filename, trav->line);
                trav = forward_recovery(trav, "variable missing",
                        push_token(error, "X", "ID", trav->line, trav->filename));
        	print_error_message(trav); 
	}
	 return	(trav);
}