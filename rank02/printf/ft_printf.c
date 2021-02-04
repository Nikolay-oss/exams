#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#define FLG_NONE	0b00000000
#define	FLG_DOT		0b00000001
#define	FLG_STAR	0b00000010

typedef	struct 		s_spec
{
	va_list			ap;
	unsigned int	flag;
	int				width;
	int				precision;
	int				bytes_count;
}					t_spec;

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
	return (count);
}

int		ft_parser(char *format, t_spec *spec)
{

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
