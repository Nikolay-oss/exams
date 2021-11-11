#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define SIDE_OUT 0
#define SIDE_IN 1

#define STDOUT 1
#define STDIN 0
#define STDERR 2

#define TYPE_END 0
#define TYPE_PIPE 1
#define TYPE_BREAK 2

typedef struct s_list
{
	char **args;
	int length;
	int type;
	int pipes[2];
	struct s_list *previous;
	struct s_list *next;
} t_list;

int ft_strlen(const char *str)
{
	int count;

	count = 0;
	while (str[count])
		count++;
	return count;
}

void print_error(const char *str)
{
	write(STDERR, str, ft_strlen(str));
}

void *exit_fatal()
{
	write(STDERR, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(1);
	return NULL;
}

char *ft_strdup(const char *str)
{
	char *str_copy;
	int i;

	if (!(str_copy = (char *)malloc(sizeof(char) * ft_strlen(str) + 1)))
		return exit_fatal();
	i = 0;
	while (str[i])
	{
		str_copy[i] = str[i];
		i++;
	}
	str_copy[i] = 0;
	return str_copy;
}

void add_arg(t_list *cmds, char *arg)
{
	char **tmp;
	int i;

	if (!(tmp = (char **)malloc(sizeof(char *) * (cmds->length + 2))))
		exit_fatal();
	i = 0;
	while (i < cmds->length)
	{
		tmp[i] = cmds->args[i];
		i++;
	}
	if (cmds->length > 0)
		free(cmds->args);
	cmds->args = tmp;
	cmds->args[i++] = ft_strdup(arg);
	cmds->args[i] = NULL;
	cmds->length++;
}

void list_push(t_list **cmds, char *arg)
{
	t_list *cmd_next;

	if (!(cmd_next = (t_list *)malloc(sizeof(t_list))))
		exit_fatal();
	cmd_next->args = NULL;
	cmd_next->length = 0;
	cmd_next->type = TYPE_END;
	cmd_next->previous = NULL;
	cmd_next->next = NULL;
	if (*cmds)
	{
		(*cmds)->next = cmd_next;
		cmd_next->previous = *cmds;
	}
	*cmds = cmd_next;
	add_arg(cmd_next, arg);
}

void list_rewind(t_list **cmds)
{
	while (*cmds && (*cmds)->previous)
		*cmds = (*cmds)->previous;
}

void list_clear(t_list **cmds)
{
	t_list *tmp;
	int i;

	while (*cmds)
	{
		i = 0;
		while ((*cmds)->args[i])
			free((*cmds)->args[i++]);
		free((*cmds)->args);
		tmp = (*cmds)->next;
		free(*cmds);
		*cmds = tmp;
	}
	*cmds = NULL;
}

void parse_arg(t_list **cmds, char *arg)
{
	int is_break;

	is_break = (strcmp(arg, ";") == 0);
	if (is_break && !*cmds)
		return ;
	else if (!is_break && (!*cmds || (*cmds)->type > TYPE_END))
		list_push(cmds, arg);
	else if (!strcmp(arg, "|"))
		(*cmds)->type = TYPE_PIPE;
	else if (is_break)
		(*cmds)->type = TYPE_BREAK;
	else
		add_arg(*cmds, arg);
}

void exec_cmd(t_list *cmd, char **env)
{
	pid_t pid;
	int pipe_open;

	pipe_open = 0;
	if (cmd->type == TYPE_PIPE || (cmd->previous && cmd->previous->type == TYPE_PIPE))
	{
		pipe_open = 1;
		if (pipe(cmd->pipes) < 0)
			exit_fatal();
	}
	if ((pid = fork()) < 0)
		exit_fatal();
	else if (pid == 0)
	{
		if (cmd->type == TYPE_PIPE && dup2(cmd->pipes[SIDE_IN], STDOUT) < 0)
			exit_fatal();
		if (cmd->previous && cmd->previous->type == TYPE_PIPE
				&& dup2(cmd->previous->pipes[SIDE_OUT], STDIN) < 0)
			exit_fatal();
		if (execve(cmd->args[0], cmd->args, env) < 0)
		{
			print_error("error: cannot execute ");
			print_error(cmd->args[0]);
			print_error("\n");
		}
		exit(-1);
	}
	else
	{
		waitpid(pid, NULL, 0);
		if (pipe_open)
		{
			close(cmd->pipes[SIDE_IN]);
			if (!cmd->next || cmd->type == TYPE_BREAK)
				close(cmd->pipes[SIDE_OUT]);
		}
		if (cmd->previous && cmd->previous->type == TYPE_PIPE)
			close(cmd->previous->pipes[SIDE_OUT]);
	}
}

void exec_cmds(t_list *cmds, char **env)
{
	t_list *cmd;

	cmd = cmds;
	while (cmd)
	{
		if (!strcmp(cmd->args[0], "cd"))
		{
			if (cmd->length < 2)
			{
				print_error("error: cd: bad arguments\n");
			}
			else if (chdir(cmd->args[1]))
			{
				print_error("error: cd: cannot change directory to ");
				print_error(cmd->args[1]);
				print_error("\n");
			}
		}
		else
		{
			exec_cmd(cmd, env);
		}
		cmd = cmd->next;
	}
}

int main(int ac, char **av, char **env)
{
	t_list *cmds;
	int i;

	cmds = NULL;
	i = 1;
	while (i < ac)
		parse_arg(&cmds, av[i++]);
	if (cmds)
	{
		list_rewind(&cmds);
		exec_cmds(cmds, env);
		list_clear(&cmds);
	}
	return 0;
}
