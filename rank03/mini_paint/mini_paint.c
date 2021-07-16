#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXSIZE	300
#define F_ERROR_MSG	"Error: Operation file corrupted\n"

typedef struct s_canvas
{
	int			width;
	int			height;
	char		back_ground;
}				t_canvas;

typedef struct s_shape
{
	char	type;
	float	x;
	float	y;
	float	radius;
	char	fill_chr;
}				t_shape;


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
	write(2, error_msg, ft_strlen(error_msg));
	return (1);
}

void	init_canvas(t_canvas *canvas)
{
	canvas->height = 0;
	canvas->width = 0;
	canvas->back_ground = 0;
}

void	init_shape(t_shape *shape)
{
	shape->type = 0;
	shape->x = 0.0f;
	shape->y = 0.0f;
	shape->radius = 0.0f;
	shape->fill_chr = 0;
}

int	check_borders(t_canvas *canvas)
{
	if ((canvas->width > 0 && canvas->width <= MAXSIZE)
		|| (canvas->height > 0 && canvas->height <= MAXSIZE))
	{
		return (0);
	}
	return (1);
}

int	read_canvas(FILE *p_f, t_canvas *canvas)
{
	int	ret;

	ret = fscanf(p_f, "%d %d %c\n", &canvas->width, &canvas->height, &canvas->back_ground);
	if (ret < 0)
		return (1);
	if (ret != 3)
		return (1);
	return (check_borders(canvas));
}

int	record_backgnd(t_canvas *canvas, char **buffer)
{
	int	size;

	size = canvas->width * canvas->height;
	*buffer = calloc(size + 1, sizeof(char));
	if (!*buffer)
		return (1);
	memset(*buffer, canvas->back_ground, size + 1);
	return (0);
}

int	in_circle(t_shape *shape, float x, float y)
{
	int	vec_len;

	vec_len = sqrtf(powf(x - shape->x, 2.0f) + powf(y - shape->y, 2.0f));
	if (vec_len < shape->radius)
	{
		if (shape->radius - vec_len <= 1.0f)
			return (2);
		return (1);
	}
	return (0);
}

void	record_rectangle(t_canvas *canvas, t_shape *shape, char **buffer)
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
			// isfill = in_rectangle(shape, (float)i, (float)j);
			isfill = in_circle(shape, (float)i, (float)j);
			if (shape->type == 'c' && isfill == 2)
				*(*buffer + j * canvas->width + i) = shape->fill_chr;
			if (shape->type == 'C' && isfill)
				*(*buffer + j * canvas->width + i) = shape->fill_chr;
			j++;
		}
		i++;
	}
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

int	check_shapes(t_shape *shape)
{
	if (shape->radius <= 0.0f)
		return (1);
	else if (shape->type != 'c' && shape->type != 'C')
		return (1);
	return (0);
}

int	read_shapes(FILE *p_f, t_canvas *canvas, char **buffer)
{
	t_shape	shape;
	int		ret;

	init_shape(&shape);
	while ((ret = fscanf(p_f, "%c %f %f %f %c\n", &shape.type, &shape.x, &shape.y, &shape.radius, &shape.fill_chr)) == 5)
	{
		if (check_shapes(&shape))
			return (1);
		record_rectangle(canvas, &shape, buffer);
	}
	if (ret != EOF)
		return (1);
	return (0);
}

int	main(int ac, char **av)
{
	FILE		*p_f;
	t_canvas	canvas;
	char		*buffer;

	p_f = NULL;
	buffer = NULL;
	if (ac < 2 || ac > 2)
		return (exit_error(NULL, NULL, "Error: argument\n"));
	if (!(p_f = fopen(*(av + 1), "r")))
		return (exit_error(p_f, NULL, F_ERROR_MSG));
	if (read_canvas(p_f, &canvas))
		return (exit_error(p_f, NULL, F_ERROR_MSG));
	if (record_backgnd(&canvas, &buffer))
		return (exit_error(p_f, buffer, F_ERROR_MSG));
	if (read_shapes(p_f, &canvas, &buffer))
		return (exit_error(p_f, buffer, F_ERROR_MSG));
	draw(&canvas, buffer);
	free(buffer);
	fclose(p_f);
	return (0);
}
