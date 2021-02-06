#include <stdio.h>
#include <stdlib.h>

static size_t	init_size(int n)
{
	size_t	size;

	size = (n >= 0) ? 1 : 2;
	while (n / 10)
	{
		n /= 10;
		size++;
	}
	return (size);
}

// char			*ft_itoa(int n)
// {
// 	long long int	num;
// 	char			*str;
// 	size_t			size;

// 	size = init_size(n);
// 	num = n;
// 	if (!(str = (char*)malloc(sizeof(char) * (size + 1))))
// 	// if (!(str = (char*)calloc(size + 1, 1)))
// 		return (str);
// 	if (num < 0)
// 	{
// 		num *= -1;
// 		*str = '-';
// 	}
// 	*(str + size) = '\0';
// 	while (size > 0)
// 	{
// 		*(str + --size) = (num % 10) + '0';
// 		num /= 10;
// 		if (*(str + size - 1) == '-' && size == 1)
// 			break ;
// 	}
// 	return (str);
// }


// из принтф

char	*ft_itoa(int nbr)
{
	char	*str;
	size_t		size;
	long long int num;

	size = init_size(nbr);
	if (!(str = (char*)malloc(sizeof(char) * (size + 1))))
		return (NULL);
	num = nbr;
	if (num < 0)
	{
		*str = '-';
		num *= -1;
	}
	*(str + size) = '\0';
	while (size > 0)
	{
		*(str + --size) = (num % 10) + '0';
		num /= 10;
		if (*(str + size - 1) == '-' && size == 1)
			break ;
	}
	return (str);
}

int		main()
{
	printf("%s\n", ft_itoa(2147483649));
}