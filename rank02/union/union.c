#include <unistd.h>

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

void	ft_union(char **strs)
{
	int isused[255];
	int i;

	i = 0;
	while (i < 255)
		isused[i++] = 0;
	i = 0;
	while (**strs)
	{
		if (!isused[(unsigned int)(**strs)])
		{
			isused[(unsigned int)(**strs)] = 1;
			ft_putchar(**strs);
		}
		(*strs)++;
		if (!**strs && i != 1)
		{
			strs++;
			i++;
		}
	}
}

int		main(int ac, char **av)
{
	if (ac == 3)
		ft_union(++av);
	ft_putchar('\n');
	return (0);
}
