#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define FLG_NONE	0b00000000
#define	FLG_DOT		0b00000001

typedef	struct 		s_spec
{
	va_list			ap;
	unsigned int	flag;
	int				width;
	int				precision;
	int				bytes_count;
}					t_spec;

void	init_spec(t_spec *spec)
{
	spec->flag = FLG_NONE;
	spec->width = -1;
	spec->precision = -1;
	spec->bytes_count = 0;
}

int		ft_strlen(char *str)
{
	int	count;

	count = 0;
	while (*(str + count))
		count++;
	return (count);
}

int		ft_putnstr(char *str, int c)
{
	int	count;

	if (!str)
		return (0);
	count = 0;
	while (*(str + count) && *(str + count) != c)
		count++;
	write(1, str, count);
	return (count);
}

int		ft_putlstr(char *str, int len)
{
	if (!str)
		return (0);
	write(1, str, len);
	return (len);
}

size_t		init_size(int nbr)
{
	size_t size;

	size = (nbr >= 0) ? 1 : 2;
	while (nbr / 10)
	{
		nbr /= 10;
		size++;
	}
	return (size);
}

int		init_size_x(unsigned int nbr)
{
	int size;

	size = 1;
	while (nbr / 16)
	{
		nbr /= 16;
		size++;
	}
	return (size);
}

char	*ft_itoa_x(unsigned int nbr)
{
	char *str;
	int size;
	char *arr_base;

	size = init_size_x(nbr);
	if (!(str = (char*)malloc(sizeof(char) * (size + 1))))
		return (NULL);
	*(str + size) = '\0';
	arr_base = "0123456789abcdef";
	while (size > 0)
	{
		*(str + --size) = *(arr_base + nbr % 16);
		nbr /= 16;
	}
	return (str);
}

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

int		ft_search(const char *dst, int c)
{
	while (*dst)
	{
		if (*dst == c)
			return (1);
		dst++;
	}
	return (0);
}

void	check_point(t_spec *spec)
{
	if ((spec->flag & FLG_DOT) && spec->precision < 0)
		spec->precision = 0;
	if ((spec->flag & FLG_DOT) && spec->width < 0)
		spec->width = 0;
}

void	ft_repeat(int count, int c)
{
	if (!count)
		return ;
	while (count--)
		write(1, &c, 1);
}

void	width_s(t_spec *spec, char *str, int size)
{
	int width;

	width = spec->width > size ? spec->width - size : 0;
	ft_repeat(width, 32);
	ft_putlstr(str, size);
	spec->bytes_count += width + size;
}

void	prec_s(t_spec *spec, char *str, int size)
{
	int precision;

	precision = spec->precision > size ? 0 : size - spec->precision;
	width_s(spec, str, size - precision);
}

void	ft_display_str(t_spec *spec)
{
	char	*str;
	int		size;

	str = va_arg(spec->ap, char *);
	size = ft_strlen(str);
	if (spec->width < 0 && spec->precision < 0)
		spec->bytes_count += ft_putlstr(str, size);
	else if (spec->width > -1 && spec->precision < 0)
		width_s(spec, str, size);
	else if (spec->precision > -1)
		prec_s(spec, str, size);
}

void	ft_disp(t_spec *spec, char *nbr, int count)
{
	int sign;

	sign = *nbr == '-' ? 1 : 0;
	spec->bytes_count += ft_putlstr(nbr, sign);
	ft_repeat(count, '0');
	spec->bytes_count += ft_putnstr(nbr + sign, '\0');
}

void	width_and_prec(t_spec *spec, char *nbr, int size)
{
	int width;
	int precision;

	width = 0;
	precision = 0;
	if (spec->precision >= size)
		precision = spec->precision - (*nbr == '-' ? size - 1 : size);
	if (spec->width > size + precision)
		width = spec->width - (size + precision);
	if (!spec->precision && *nbr == '0')
	{
		ft_repeat(spec->width, 32);
		spec->bytes_count += spec->width;
		return ;
	}
	ft_repeat(width, 32);
	ft_disp(spec, nbr, precision);
	spec->bytes_count += width + precision;
}

void	ft_display_d(t_spec *spec)
{
	char	*nbr;
	int		size;
	int		n;

	n = va_arg(spec->ap, int);
	if (!(nbr = ft_itoa(n)))
	{
		spec->bytes_count = -1;
		return ;
	}
	size = ft_strlen(nbr);
	if (spec->width < 0 && spec->precision < 0)
		spec->bytes_count += ft_putlstr(nbr, size);
	else
		width_and_prec(spec, nbr, size);
	free(nbr);
}

void	ft_display_x(t_spec *spec)
{
	char *nbr;
	int size;

	size = 0;
	if (!(nbr = ft_itoa_x(va_arg(spec->ap, unsigned int))))
	{
		spec->bytes_count = -1;
		return ;
	}
	size = ft_strlen(nbr);
	if (spec->width < 0 && spec->precision < 0)
		spec->bytes_count += ft_putlstr(nbr, size);
	else
		width_and_prec(spec, nbr, size);
	free(nbr);
}

void	ft_modes(t_spec *spec, int c)
{
	if (c == 'd')
		ft_display_d(spec);
	else if (c == 's')
		ft_display_str(spec);
	else if (c == 'x')
		ft_display_x(spec);
}

int		ft_isdigit(int c)
{
	return ((c >= '0' && c <= '9') ? 1 : 0);
}

int		ft_atoi(const char *str)
{
	long long int nbr;

	nbr = 0;
	while (*str && ft_isdigit(*str))
	{
		nbr *= 10;
		nbr += *str - '0';
		str++;
	}
	return ((int)nbr);
}

void	get_width_prec(t_spec *spec, char **str)
{
	if (spec->width < 0 && !(spec->flag & FLG_DOT))
		spec->width = ft_atoi(*str);
	else if (spec->precision < 0 && (spec->flag & FLG_DOT))
		spec->precision = ft_atoi(*str);
	while (**str && ft_isdigit(**str))
		(*str)++;
}

void	follow_line(t_spec *spec, char **str)
{
	while (**str)
	{
		if (ft_isdigit(**str))
			get_width_prec(spec, str);
		if (**str == '.')
			spec->flag |= FLG_DOT;
		if (ft_search("dsx", **str))
		{
			check_point(spec);
			ft_modes(spec, **str);
			(*str)++;
			return ;
		}
		(*str)++;
	}
}

int		ft_parser(char *format, t_spec *spec)
{
	int		bytes_count;
	char	*current;
	int 	count;

	bytes_count = 0;
	bytes_count = ft_putnstr(format, '%');
	current = format + bytes_count;
	count = 0;
	while (*current)
	{
		current++;
		init_spec(spec);
		follow_line(spec, &current);
		if (spec->bytes_count == -1)
			return (-1);
		count = ft_putnstr(current, '%');
		current += count;
		bytes_count += spec->bytes_count + count;
	}
	return (bytes_count);
}

int		ft_printf(const char *format, ...)
{
	t_spec	spec;
	int		bytes_count;

	bytes_count = 0;
	va_start(spec.ap, format);
	bytes_count = ft_parser((char*)format, &spec);
	va_end(spec.ap);
	return (bytes_count);
}
