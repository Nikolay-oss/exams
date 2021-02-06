#include "ft_printf.c"

int	main()
{
	// printf("|%d|\n", printf("Hello, %.s!A%s\n", "World", "Helll"));
	// printf("|%d|\n", ft_printf("Hello, %.s!A%s\n", "World", "Helll"));
	// printf("|%d|\n", printf("Hello, %.d!A%d\t%d\n", 42, -42, 0));
	printf("|%d|\n", ft_printf("%x\n", -2147483650));
	printf("|%d|\n", ft_printf("%x\n", -2147483650));
	// printf("%s\n", ft_itoa(2147483649));

	// printf("|%d|\n", printf("d10w4p %10.4d %10.4d %10.4d %10.4d %10.4d %10.4d %10.4d %10.4d\n", 0, 42, 1, 4554, 2147483647, (int)2147483648, (int)-2147483648, (int)-2147483649));
	// printf("|%d|\n", ft_printf("d10w4p %10.4d %10.4d %10.4d %10.4d %10.4d %10.4d %10.4d %10.4d\n", 0, 42, 1, 4554, 2147483647, (int)2147483648, (int)-2147483648, (int)-2147483649));
}