#include "get_next_line.h"

static	size_t	ft_strlen(char *str)
{
	size_t count;

	count = 0;
	while (*str)
	{
		count++;
		str++;
	}
	return (count);
}

static	char	*ft_strjoin(char *s1, char *s2)
{
	char	*str;
	size_t	size;

	size = ft_strlen(s1) + ft_strlen(s2);
	if (!(str = (char*)malloc(sizeof(char) * (size + 1))))
		return (NULL);
	size = 0;
	while (*s1)
	{
		*(str + size++) = *s1;
		s1++;
	}
	while (*s2)
	{
		*(str + size++) = *s2;
		s2++;
	}
	*(str + size) = '\0';
	return (str);
}

int		get_next_line(char **line)
{
	char	c[2];
	int		res;
	char	*current;

	if (!line)
		return (-1);
	if (!(*line = ft_strjoin("", "")))
		return (-1);
	while ((res = read(0, c, 1)) > 0)
	{
		c[res] = '\0';
		if (*c == '\n')
			return (1);
		current = *line;
		if (!(*line = ft_strjoin(*line, c)))
			return (-1);
		free(current);
	}
	return (res < 0 ? -1 : 0);
}
