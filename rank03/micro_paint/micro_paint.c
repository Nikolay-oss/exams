#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 300
#define ERRORMSG "Error: Operation file corrupted\n"

typedef struct s_canvas
{
	int		width;
	int		height;
	char	chr_backgnd;
}			t_canvas;

typedef struct s_shape
{
	char	type;
	float	x;
	float	y;
	float	width;
	float	height;
	char	chr_fill;
}		t_shape;

void	init_canvas(t_canvas *canvas)
{
	canvas->width = 0;
	canvas->height = 0;
	canvas->chr_backgnd = 0;
}

void	init_shape(t_shape *shape)
{
	shape->type = 0;
	shape->x = 0.f;
	shape->y = 0.f;
	shape->width = 0.f;
	shape->height = 0.f;
	shape->chr_fill = 0;
}

size_t	ft_strlen(const char *str)
{
	size_t	size;

	size = 0;
	while (*(str + size))
		size++;
	return (size);
}

int	exit_error(FILE *p_f, char *buffer, const char *error_msg)
{
	if (p_f)
		fclose(p_f);
	if (buffer)
		free(buffer);
	write(1, error_msg, ft_strlen(error_msg));
	return (1);
}

int	read_canvas(FILE *p_f, t_canvas *canvas)
{
	if (fscanf(p_f, "%d %d %c\n", &canvas->width, &canvas->height, &canvas->chr_backgnd) != 3)
		return (1);
	if (canvas->width < 1 || canvas->width > MAXSIZE || canvas->height < 1 || canvas->height > MAXSIZE)
		return (1);
	return (0);
}

int	set_backgnd(t_canvas *canvas, char **buffer)
{
	int	size;

	size = canvas->width * canvas->height;
	if (!(*buffer = (char *)malloc(size * sizeof(char))))
		return (1);
	memset(*buffer, canvas->chr_backgnd, size);
	return (0);
}

int	in_rectangle(t_shape *shape, float x, float y)
{
	if (x < shape->x || x > shape->x + shape->width
		|| y < shape->y || y > shape->y + shape->height)
	{
		return (0);
	}
	if (x - shape->x < 1.f || shape->x + shape->width - x < 1.f
		|| y - shape->y < 1.f || shape->y + shape->height - y < 1.f)
	{
		return (2);
	}
	return (1);
}

void	set_shapes(t_canvas *canvas, t_shape *shape, char *buffer)
{
	int	i;
	int	j;
	int	isfill;

	i = 0;
	while (i < canvas->width)
	{
		j = 0;
		while (j < canvas->height)
		{
			isfill = in_rectangle(shape, (float) i, (float) j);
			if (isfill == 2 || (isfill && shape->type == 'R'))
				*(buffer + j * canvas->width + i) = shape->chr_fill;
			j++;
		}
		i++;
	}
}

int	read_shapes(FILE *p_f, t_canvas *canvas, t_shape *shape, char *buffer)
{
	int	ret;

	while ((ret = fscanf(p_f, "%c %f %f %f %f %c\n", &shape->type, &shape->x, &shape->y, &shape->width, &shape->height, &shape->chr_fill)) == 6)
	{
		if (shape->width <= 0.f || shape->height <= 0.f || (shape->type != 'r' && shape->type != 'R'))
			return (1);
		set_shapes(canvas, shape, buffer);
	}
	if (ret != -1)
		return (1);
	return (0);
}

void	draw(t_canvas *canvas, char *buffer)
{
	int	i;

	i = -1;
	while (++i < canvas->height)
	{
		write(1, buffer + i * canvas->width, canvas->width);
		write(1, "\n", 1);
	}
}

int	parser(FILE *p_f, t_canvas *canvas, char **buffer)
{
	t_shape	shape;

	init_canvas(canvas);
	init_shape(&shape);
	if (read_canvas(p_f, canvas))
		return (1);
	if (set_backgnd(canvas, buffer))
		return (exit_error(p_f, NULL, ERRORMSG));
	if (read_shapes(p_f, canvas, &shape, *buffer))
		return (exit_error(p_f, *buffer, ERRORMSG));
	return (0);
}

int	main(int ac, char **av)
{
	FILE		*p_f;
	t_canvas	canvas;
	char		*buffer;

	p_f = NULL;
	buffer = NULL;
	if (ac != 2)
		return (exit_error(NULL, NULL, "Error: argument\n"));
	if (!(p_f = fopen(*(av + 1), "r")))
		return (exit_error(NULL, NULL, ERRORMSG));
	if (parser(p_f, &canvas, &buffer))
		return (1);
	draw(&canvas, buffer);
	free(buffer);
	fclose(p_f);
	return (0);
}
