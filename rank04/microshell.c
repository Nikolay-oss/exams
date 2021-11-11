#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// #define UBUNTA

#ifdef UBUNTA
	#include <wait.h>
#endif

#define STDOUT 1
#define STDIN 0

#define FD_READ 0
#define FD_WRITE 1

typedef struct s_list
{
	char **av;
	int ac;
	int pipe_write;
	int pipe_read;
	int pipes[2];
	struct s_list *next;
	struct s_list *prev;
} t_list;

size_t ft_strlen(const char *s)
{
	size_t i;

	i = 0;
	while (s[i])
		i++;
	return i;
}

void print_error(const char *s)
{
	write(2, s, ft_strlen(s));
}

void exit_fatal()
{
	print_error("error: fatal\n");
	exit(1);
}

t_list *create_node()
{
	t_list *node;

	if (!(node = (t_list *)malloc(sizeof(t_list))))
		exit_fatal();
	node->av = NULL;
	node->ac = 0;
	node->pipe_read = 0;
	node->pipe_write = 0;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

t_list *lst_getlast(t_list *cmds)
{
	t_list *cur;

	cur = cmds;
	while (cur->next)
		cur = cur->next;
	return cur;
}

void lst_push_back(t_list **cmds, t_list *node)
{
	t_list *last;

	if (!*cmds)
	{
		*cmds = node;
		return ;
	}
	last = lst_getlast(*cmds);
	last->next = node;
	node->prev = last;
}

void lst_free(t_list *cmds)
{
	t_list *tmp;

	while (cmds)
	{
		tmp = cmds->next;
		free(cmds);
		cmds = tmp;
	}
}

t_list *parser(int ac, char **av)
{
	static int i = 1;
	t_list *cmds;
	t_list *tmp;

	cmds = NULL;
	tmp = NULL;
	while (i < ac)
	{
		if (!strcmp(av[i], ";"))
		{
			if (cmds)
			{
				av[i++] = NULL;
				return cmds;
			}
		}
		else if (!strcmp(av[i], "|"))
		{
			tmp = lst_getlast(cmds);
			tmp->pipe_write = 1;
			tmp = create_node();
			tmp->pipe_read = 1;
			tmp->av = &av[i + 1];
			tmp->ac++;
			lst_push_back(&cmds, tmp);
			av[i] = NULL;
		}
		else
		{
			if (!cmds)
			{
				cmds = create_node();
				cmds->av = &av[i];
			}
			cmds->ac++;
		}
		i++;
	}
	return cmds;
}

void exec_cmds(t_list *cmds, char **env)
{
	pid_t pid;

	if (cmds->pipe_write && pipe(cmds->pipes) < 0)
		exit_fatal();
	if ((pid = fork()) < 0)
		exit_fatal();
	if (pid == 0)
	{
		if (cmds->pipe_write && dup2(cmds->pipes[FD_WRITE], STDOUT) < 0)
			exit_fatal();
		else if (cmds->pipe_read && dup2(cmds->prev->pipes[FD_READ], STDIN) < 0)
			exit_fatal();
		if (execve(cmds->av[0], cmds->av, env))
		{
			print_error("error: cannot execute ");
			print_error(cmds->av[0]);
			print_error("\n");
		}
		exit(-1);
	}
	else
	{
		waitpid(pid, NULL, 0);
		if (cmds->pipe_write && close(cmds->pipes[FD_WRITE]) < 0)
			exit_fatal();
		else if (cmds->pipe_read && close(cmds->prev->pipes[FD_READ]) < 0)
			exit_fatal();
	}
}

void ft_cd(t_list *cmds)
{
	if (cmds->ac != 2)
	{
		print_error("error: cd: bad arguments\n");
	}
	else if (chdir(cmds->av[1]) == -1)
	{
		print_error("error: cd: cannot change directory to ");
		print_error(cmds->av[1]);
		print_error("\n");
	}
}

int main(int ac, char **av, char **env)
{
	t_list *cmds;
	t_list *tmp;

	while ((cmds = parser(ac, av)))
	{
		tmp = cmds;
		while (tmp)
		{
			if (!strcmp(tmp->av[0], "cd"))
				ft_cd(tmp);
			else
				exec_cmds(tmp, env);
			tmp = tmp->next;
		}
		lst_free(cmds);
		cmds = NULL;
	}
	return 0;
}
