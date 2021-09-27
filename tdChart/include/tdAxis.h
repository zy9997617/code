#ifndef _TDAXIS_H_
#define _TDAXIS_H_

#include <TWidget/TWidget.h>
#include <TWidget/TWidget_dev.h>
#include "struct.h"
#include "main.h"
#include <tdCairo.h>


//饼图的参数


void _ChartCreateAxis(void *obj, int w, int h);
void _ChartSetScale(void *obj, SCALE_TYPE scale_type, double start, double end, double unit);
void chart_update(TDisplayCell *dest_vc, cairo_surface_t *surface, cairo_t *cr, int w, int h);




#endif