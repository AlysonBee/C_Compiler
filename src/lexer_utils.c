#include <ctype.h>
#include "../inc/token.h"
#include "../inc/compiler.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


t_token     *single_token(char *buffer, size_t index, char *tok_name)
{
    char    *var_name;
    t_token *tok;
    extern int read_count;

    var_name = NULL;
    var_name = charpush(var_name, buffer[index]);
    index++;
    read_count = index;
    tok = new_token(var_name, tok_name);
    free(var_name);
    return tok;
}

t_token     *double_token(char *buffer, size_t index, t_hashtable *table, char *first_name)
{
    char        *value;
    char        *var_name;
    char        *test;
    t_token     *token;
    extern int  read_count;

    var_name = NULL;
    token = NULL;
    test = NULL;
    var_name = charpush(var_name, buffer[index]);
    test = charpush(test, buffer[index]);
    test = charpush(test, buffer[index + 1]);
    value = ht_search(table, test);
    if (value == NULL)
    {
        token = new_token(var_name, first_name);
        index++;
    }
    else
    {
        if ((strcmp(test, "<<") == 0 || strcmp(test, ">>") == 0) &&
            buffer[index + 2] == '=')
        {
            test = charpush(test, buffer[index + 2]);
            value = ht_search(table, test);
            index++;
        }
        token = new_token(test, value);
        index += 2;
    }
    read_count = index;
    free(test);
    free(var_name);
    return (token);
}
