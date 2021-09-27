#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include <cairo/cairo.h>

cairo_surface_t *gui_cairo_create_x11_surface(int *x, int *y, int win);