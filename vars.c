#include "shell.h"


/**
 * is_chain - test if current char in buffer is a chain delimeter
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 * Return: 1 if chain delimeter, 0 otherwise
 */

int is_chain(info_t *info, char *buf, size_t *p)
{
	size_t b = *p;

	if (buf[b] == '|' && buf[b + 1] == '|')
	{
	buf[b] = 0;
	b++;
	info->cmd_buf_type = CMD_OR;
	}
	else if (buf[b] == '&' && buf[b + 1] == '&')
	{
	buf[b] = 0;
	b++;
	info->cmd_buf_type = CMD_AND;
	}
	else if (buf[b] == ';') /* found end of this command */
	{
	buf[b] = 0; /* replace semicolon with null */
	info->cmd_buf_type = CMD_CHAIN;
	}
	else
	return (0);
	*p = b;
	return (1);
}

/**
 * check_chain - checks we should continue chaining based on last status
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 * @i: starting position in buf
 * @len: length of buf
 * Return: Void
 */

void check_chain(info_t *info, char *buf, size_t *p, size_t i, size_t len)
{
	size_t b = *p;

	if (info->cmd_buf_type == CMD_AND)
	{
	if (info->status)
	{
	buf[i] = 0;
	b = len;
	}
	}
	if (info->cmd_buf_type == CMD_OR)
	{
	if (!info->status)
	{
	buf[i] = 0;
	b = len;
	}
	}
	*p = b;
}

/**
 * replace_alias - replaces an aliases in the tokenized string
 * @info: the parameter struct
 * Return: 1 if replaced, 0 otherwise
 */

int replace_alias(info_t *info)
{
	int a;
	list_t *node;
	char *p;

	for (a = 0; a < 10; a++)
	{
	node = node_starts_with(info->alias, info->argv[0], '=');
	if (!node)
	return (0);
	free(info->argv[0]);
	p = _strchr(node->str, '=');
	if (!p)
	return (0);
	p = _strdup(p + 1);
	if (!p)
	return (0);
	info->argv[0] = p;
	}
	return (1);
}

/**
 * replace_vars - replaces vars in the tokenized string
 * @info: the parameter struct
 * Return: 1 if replaced, 0 otherwise
 */

int replace_vars(info_t *info)
{
	int a = 0;
	list_t *node;

	for (a = 0; info->argv[a]; a++)
	{
	if (info->argv[a][0] != '$' || !info->argv[a][1])
	continue;
	if (!_strcmp(info->argv[a], "$?"))
	{
	replace_string(&(info->argv[a]),
	_strdup(convert_number(info->status, 10, 0)));
	continue;
	}
	if (!_strcmp(info->argv[a], "$$"))
	{
	replace_string(&(info->argv[a]),
	_strdup(convert_number(getpid(), 10, 0)));
	continue;
	}
	node = node_starts_with(info->env, &info->argv[a][1], '=');
	if (node)
	{
	replace_string(&(info->argv[a]),
	_strdup(_strchr(node->str, '=') + 1));
	continue;
	}
	replace_string(&info->argv[a], _strdup(""));
	}
	return (0);
}

/**
 * replace_string - replaces string
 * @old: address of old string
 * @new: new string
 * Return: 1 if replaced, 0 otherwise
 */

int replace_string(char **old, char *new)
{
	free(*old);

	*old = new;

	return (1);
}
