#include "tdChart.h"
#include "tdAxis.h"
#include "main.h"
#include "tools.h"
#include <tdCairo.h>
extern void merge_surface(cairo_t *dest_cr, cairo_surface_t *source_sfc, cairo_operator_t op);

Pixmap img = 0;


//此处存在一个重大问题：每次生成png图片的时候，会消耗大量时间，导致出现出图卡顿
void chart_update(TDisplayCell *dest_vc, cairo_surface_t *surface, cairo_t *cr, int w, int h)
{
    int timeuse;
    TwFreeImage(img);

    gettimeofday(&start_time, NULL);

    cairo_surface_write_to_png(surface, "test.png");
    timeuse= 1000000*(end_time.tv_sec-start_time.tv_sec)+end_time.tv_usec-start_time.tv_usec;
    // printf("time write :%d us \n", timeuse);
    img = TwLoadImage("test.png");
    DcSetImage(dest_vc, img, w, h);
    DcUpdateShow();

    // system("rm test.png");
    
}
// static void paint_coordinates(TDisplayCell *dest_vc, cairo_surface_t *surface, cairo_t *cr, int w, int h)
static void paint_coordinates(void *obj)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_t *dest_cr = chart->cr;
    cairo_t *cr = chart->axis_cr;
    cairo_surface_t *axis_surface = chart->axis_surface;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;

    double temp_x, temp_y;
    cairo_set_source_rgba(cr, 1,1,1,0);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);

    cairo_move_to(cr, w/15, h-h/15);
    cairo_get_current_point(cr, &chart->cairo_info.origin_x, &chart->cairo_info.origin_y);
    cairo_line_to(cr, w-w/15, h-h/15);
    chart->cairo_info.axis_w = w-2*w/15;
    cairo_get_current_point(cr, &temp_x, &temp_y);
    cairo_line_to(cr, temp_x-4, temp_y-4);
    cairo_move_to(cr, temp_x, temp_y);
    cairo_line_to(cr, temp_x-4, temp_y+4);

    cairo_move_to(cr, w/15, h-h/15);
    cairo_line_to(cr, w/15, h/15);
    chart->cairo_info.axis_h = h-2*h/15;
    cairo_get_current_point(cr, &temp_x, &temp_y);
    cairo_line_to(cr, temp_x-4, temp_y+4);
    cairo_move_to(cr, temp_x, temp_y);
    cairo_line_to(cr, temp_x+4, temp_y+4);
    cairo_stroke(cr);
    
}

// static void paint_echarts(TDisplayCell *dest_vc, cairo_surface_t *surface, cairo_t *cr, int w, int h)
static void paint_echarts(void *obj)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_t *cr = chart->cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;

    double temp_x, temp_y;
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);

    cairo_move_to(cr, 10, h-10);
    cairo_get_current_point(cr, &chart->cairo_info.origin_x, &chart->cairo_info.origin_y);
    cairo_line_to(cr, w-10, h-10);
    chart->cairo_info.axis_w = w-20;
    cairo_move_to(cr, 10, h-10);
    cairo_line_to(cr, 10, 10);
    chart->cairo_info.axis_h = h-20;
    cairo_get_current_point(cr, &temp_x, &temp_y);
    cairo_line_to(cr, temp_x-4, temp_y+4);
    cairo_move_to(cr, temp_x, temp_y);
    cairo_line_to(cr, temp_x+4, temp_y+4);

    cairo_move_to(cr, w-10, h-10);
    cairo_line_to(cr, w-10, 10);
    cairo_get_current_point(cr, &temp_x, &temp_y);
    cairo_line_to(cr, temp_x-4, temp_y+4);
    cairo_move_to(cr, temp_x, temp_y);
    cairo_line_to(cr, temp_x+4, temp_y+4);
    cairo_stroke(cr);

    chart_update(dest_vc, surface, cr, w, h);
}

static void paint_pie(void *obj)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_t *cr = chart->cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    
    int w = chart->surface_w;
    int h = chart->surface_h;

    double temp_x, temp_y;
    double ori_r = (w/3.0 < h/3.0)?w/3.0:h/3.0;
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_move_to(cr, w/2.0+ori_r, h/2.2);
    cairo_arc(cr, w/2.0, h/2.2, ori_r, 0, 2*pi);
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_stroke(cr);

    chart_update(dest_vc, surface, cr, w, h);
    chart->cairo_info.origin_x = w/2.0;
    chart->cairo_info.origin_y = h/2.2;
    chart->cairo_info.pie_r = ori_r;
    
}

//雷达图默认圆形，当有节点时才显示对应形状
static void paint_radar(void *obj)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_t *cr = chart->cr;
    int w = chart->surface_w;
    int h = chart->surface_h;  

    double temp_x, temp_y;
    double ori_r = (w/3.0 < h/3.0)?w/3.0:h/3.0;
    
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_move_to(cr, w/2.0+ori_r, h/2.0);
    cairo_arc(cr, w/2.0, h/2.0, ori_r, 0, 2*pi);
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_fill_preserve(cr);
    cairo_set_line_width(cr, 1);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_stroke(cr);

    chart_update(dest_vc, surface, cr, w, h);
    chart->cairo_info.origin_x = w/2.0;
    chart->cairo_info.origin_y = h/2.0;
    chart->cairo_info.pie_r = ori_r;  
}




//刻度标注
static void set_scale_tag(void *obj, char *content, double x, double y, int x_y, int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_t *cr = chart->cr;
    int w = chart->surface_w;
    int h = chart->surface_h;

    int text_w, text_h, font_size;
    int zh_num = 0, ch_num = 0;
   
    font_size = chart->cairo_info.scale_font_size;
    char *temp_str = malloc(256);
    sprintf(temp_str, "%s", content);
    for(int i = 0; i < strlen(temp_str); i++)
    {
        if(is_zh_ch(temp_str[i]))
        {
            i+=2;
            zh_num++;
        }
        else 
        {
            ch_num++;
        }
    }
    text_w = zh_num * font_size + ch_num * font_size/2;
    text_h = font_size;
    
    TDisplayCell *parent_dc = chart->vc;
    TWidget *widget = (TWidget*)obj;
    if(x_y == 1)
    {
        if(!chart->dc_x[chart->x_dc_num])
            chart->dc_x[chart->x_dc_num] = DcCreate(DC_INPUT_WINDOW, parent_dc, x-text_w, y, 2*text_w, 2*text_h, widget);
        else
            DcMove(chart->dc_x[chart->x_dc_num], x-text_w, y);
        char t[32] = {0};
        sprintf(t, "%s", content);
        char *test = malloc(strlen(content));
        sprintf(test, "%s", content);
        DcSetText(chart->dc_x[chart->x_dc_num], test, -1, 0);
        DcSetTextAline(chart->dc_x[chart->x_dc_num], DC_TEXT_X_MIDDLE);
        // sprintf(chart->cairo_info.scale_string_info.x_string[num], "%s", content);
        chart->x_dc_num++;

        // DcSetBackgroundColor(dc, T_RGB(255,0,0));
    }
    else if(x_y == 2)
    {
        chart->dc_y[chart->y_dc_num] = DcCreate(DC_INPUT_WINDOW, parent_dc, x-2*text_w-2, y-text_h, 2*text_w, 2*text_h, widget);
        
        char t[32] = {0};
        sprintf(t, "%s", content);
        char *test = malloc(strlen(content));
        sprintf(test, "%s", content);
        DcSetText(chart->dc_y[chart->y_dc_num], test, -1, 0);
        DcSetTextAline(chart->dc_y[chart->y_dc_num], DC_TEXT_X_MIDDLE);
        // DcSetBackgroundColor(dc, T_RGB(255,0,0));   
        chart->y_dc_num++;
    }
    else if(x_y == 3)
    {
        chart->dc_y2[chart->y2_dc_num] = DcCreate(DC_INPUT_WINDOW, parent_dc, x+text_w+text_w/4, y-text_h, 2*text_w, 2*text_h, widget);
        
        char t[32] = {0};
        sprintf(t, "%s", content);
        char *test = malloc(strlen(content));
        sprintf(test, "%s", content);
        DcSetText(chart->dc_y2[chart->y2_dc_num], test, -1, 0);
        DcSetTextAline(chart->dc_y2[chart->y2_dc_num], DC_TEXT_X_MIDDLE);
        chart->y2_dc_num++;
    }
    // DcSetBackgroundColor(dc, T_RGB(255,0,0));
    free(temp_str);

}

static void set_x_scale_double(void *obj, double scales[], int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    chart->x_dc_num = 0;
    chart->cairo_info.x_scale_num = num;

    double temp_x, temp_y;
    // cairo_move_to(cr, origin_x, origin_y);
    double move_unit = 1.0*axis_w/(0.6+num);
    chart->cairo_info.x_unit = move_unit;
    cairo_set_source_rgb(cr, 1, 1, 0);
    double actual_scale;
    char text[32] = {0};//刻度值显示所需字符串
    // sprintf(text, "%.1lf", start);
    // set_scale_tag(chart, text, origin_x+5, origin_y, 1, 0);
    cairo_move_to(cr, origin_x, origin_y);
    for(int i = 0; i < num; i++)
    {
        cairo_set_source_rgb(cr, 1, 1, 0);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x+move_unit, temp_y);
        actual_scale = scales[i];
        sprintf(text, "%.1lf", actual_scale);
        // if(if_show_grid == FALSE)
        // {
            cairo_line_to(cr, temp_x+move_unit, temp_y-5);
            set_scale_tag(chart, text, temp_x+move_unit, temp_y, 1, i+1);
            cairo_move_to(cr, temp_x+move_unit, temp_y);
            
        // }
        // else
        // {
            cairo_move_to(grid_cr, temp_x+move_unit, temp_y+1);
            cairo_line_to(grid_cr, temp_x+move_unit, temp_y-axis_h);
            cairo_stroke(grid_cr);
            
        // }
        chart->cairo_info.scale_double_info.x_scale[i] = scales[i];
    }
    cairo_stroke(cr);

    chart->cairo_info.x_unit = move_unit;
    // chart_update(dest_vc, surface, cr, w, h);
}

static void set_x_scale_int(void *obj, int scales[], int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    chart->x_dc_num = 0;
    chart->cairo_info.x_scale_num = num;

    double temp_x, temp_y;
    // cairo_move_to(cr, origin_x, origin_y);
    double move_unit = 1.0*axis_w/(0.6+num);
    chart->cairo_info.x_unit = move_unit*1.0;
    cairo_set_source_rgb(cr, 1, 1, 0);
    int actual_scale;
    char text[32] = {0};//刻度值显示所需字符串
    // sprintf(text, "%d", start);
    // set_scale_tag(chart, text, origin_x+5, origin_y, 1, 0);

    cairo_move_to(cr, origin_x, origin_y);
    for(int i = 0; i < num; i++)
    {
        cairo_set_source_rgb(cr, 1, 1, 0);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x+move_unit, temp_y);
        actual_scale = scales[i];
        sprintf(text, "%d", actual_scale);
        // if(if_show_grid == FALSE)
        // {
            cairo_line_to(cr, temp_x+move_unit, temp_y-5);
            set_scale_tag(chart, text, temp_x+move_unit, temp_y, 1, i+1);
            cairo_move_to(cr, temp_x+move_unit, temp_y);
            
        // }
        // else
        // {
            cairo_move_to(grid_cr, temp_x+move_unit, temp_y+1);
            cairo_line_to(grid_cr, temp_x+move_unit, temp_y-axis_h);
            cairo_stroke(grid_cr);
        // }
        chart->cairo_info.scale_int_info.x_scale[i] = scales[i];
    }
    cairo_stroke(cr);
    
    chart->cairo_info.x_unit = move_unit;
    // chart_update(dest_vc, surface, cr, w, h);
}

static void set_x_scale_string(void *obj, char **content, int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    chart->cairo_info.x_scale_num = num;

    chart->x_dc_num =0;
    double temp_x, temp_y;
    double move_unit = 1.0*axis_w/(0.6+num);
    chart->cairo_info.x_unit = move_unit;
    char text[32] = {0};
    cairo_move_to(cr, origin_x, origin_y);

    for(int i = 0; i < num; i++)
    {
        sprintf(text, "%s", content[i]);
        cairo_set_source_rgba(cr, 1, 1, 0, 1);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x + move_unit, temp_y);
        // if(!if_show_grid)
            cairo_line_to(cr, temp_x + move_unit, temp_y-5);
        // else 
            cairo_move_to(grid_cr, temp_x+move_unit, temp_y+1);
            cairo_line_to(grid_cr, temp_x+move_unit, temp_y-axis_h);
            cairo_stroke(grid_cr);
        cairo_stroke(cr);
        set_scale_tag(chart, text, temp_x + move_unit, temp_y, 1, i);
        cairo_move_to(cr, temp_x + move_unit, temp_y);
        sprintf(chart->cairo_info.scale_string_info.x_string[i], "%s", content[i]);
    }
    chart->cairo_info.x_unit = move_unit;

}

static void set_y_scale_string(void *obj, char **content, int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    chart->cairo_info.y_scale_num = num;

    chart->y_dc_num =0;
    double temp_x, temp_y;
    double move_unit = 1.0*axis_h/(0.2+num);
    chart->cairo_info.y_unit = move_unit;
    char text[32] = {0};
    cairo_move_to(cr, origin_x, origin_y);
    for(int i = 0; i < num; i++)
    {
        sprintf(text, "%s", content[i]);
        cairo_set_source_rgba(cr, 1, 1, 0, 1);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x, temp_y - move_unit);
        // if(!if_show_grid)
            cairo_line_to(cr, temp_x + 5, temp_y - move_unit);
        // else 
            cairo_move_to(grid_cr, temp_x, temp_y-move_unit);
            cairo_line_to(grid_cr, temp_x+axis_w, temp_y-move_unit);
            cairo_set_line_width(grid_cr, 1);
            cairo_stroke(grid_cr);
        cairo_stroke(cr);
        set_scale_tag(chart, text, temp_x, temp_y - move_unit, 2, i);
        cairo_move_to(cr, temp_x, temp_y-move_unit);
    }
    chart->cairo_info.y_unit = move_unit;
}

static void set_y_scale_double(void *obj, double scales[], int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    chart->y_dc_num = 0;
    chart->cairo_info.y_scale_num = num;

    chart->cairo_info.scale_double_info.y_start = scales[0];
    chart->cairo_info.scale_double_info.y_end = scales[num-1];

    double temp_x, temp_y;
    // cairo_move_to(cr, origin_x, origin_y);
    double move_unit = 1.0*axis_h/(0.2+num);
    chart->cairo_info.y_unit = move_unit;
    cairo_set_source_rgb(cr, 1, 1, 0);
    double actual_scale;
    char text[32] = {0};//刻度值显示所需字符串
    // sprintf(text, "%.1lf", start);
    // set_scale_tag(chart, text, origin_x, origin_y-5, 2, 0);
    cairo_move_to(cr, origin_x, origin_y);
    for(int i = 0; i < num; i++)
    {
        cairo_set_source_rgb(cr, 1, 1, 0);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x, temp_y-move_unit);
        actual_scale = scales[i];
        sprintf(text, "%.1lf", actual_scale);
        // if(if_show_grid == FALSE)
        // {
            cairo_line_to(cr, temp_x+5, temp_y-move_unit);
            set_scale_tag(chart, text, temp_x, temp_y-move_unit, 2, i+1);
            cairo_move_to(cr, temp_x, temp_y-move_unit);
            
        // }
        // else
        // {
            cairo_move_to(grid_cr, temp_x, temp_y-move_unit);
            cairo_line_to(grid_cr, temp_x+axis_w, temp_y-move_unit);
            cairo_set_line_width(grid_cr, 1);
            cairo_stroke(grid_cr);
        // }
        chart->cairo_info.scale_double_info.y_scale[i] = scales[i];
    }
    cairo_stroke(cr);
    chart->cairo_info.y_unit = move_unit;
    // chart_update(dest_vc, surface, cr, w, h);
}

static void set_y_scale_int(void *obj, int scales[], int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    //排序
    qsort(scales, num, sizeof(int), compare_int);
  
    chart->cairo_info.scale_int_info.y_start = scales[0];
    chart->cairo_info.scale_int_info.y_end = scales[num-1];

    chart->y_dc_num = 0;
    chart->cairo_info.y_scale_num = num;

    double temp_x, temp_y;
    double move_unit = 1.0*axis_h/(0.2+num);
    chart->cairo_info.y_unit = move_unit;
    cairo_set_source_rgb(cr, 1, 1, 0);
    int actual_scale;
    char text[32] = {0};//刻度值显示所需字符串
    // sprintf(text, "%d", start);
    // set_scale_tag(chart, text, origin_x, origin_y-5, 2, 0);
    cairo_move_to(cr, origin_x, origin_y);
    for(int i = 0; i < num; i++)
    {
        cairo_set_source_rgb(cr, 1, 1, 0);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x, temp_y-move_unit);
        actual_scale = scales[i];
        sprintf(text, "%d", actual_scale);
        // if(if_show_grid == FALSE)
        // {
            cairo_line_to(cr, temp_x+5, temp_y-move_unit);
            set_scale_tag(chart, text, temp_x, temp_y-move_unit, 2, i+1);
            cairo_move_to(cr, temp_x, temp_y-move_unit);
            
        // }
        // else
        // {
            cairo_move_to(grid_cr, temp_x, temp_y-move_unit);
            cairo_line_to(grid_cr, temp_x+axis_w, temp_y-move_unit);
            cairo_set_line_width(grid_cr, 1);
            cairo_stroke(grid_cr);
        // }
        chart->cairo_info.scale_int_info.y_scale[i] = scales[i];
    }
    cairo_stroke(cr);
    chart->cairo_info.y_unit = move_unit;
    // chart_update(dest_vc, surface, cr, w, h);
}

static void set_y2_scale_double(void *obj, double scales[], int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    chart->y2_dc_num = 0;
    chart->cairo_info.y2_scale_num = num;

    chart->cairo_info.scale_double_info.y2_start = scales[0];
    chart->cairo_info.scale_double_info.y2_end = scales[num-1];

    double actual_scale;
    double temp_x, temp_y;
    cairo_move_to(cr, origin_x, origin_y);
    double move_unit = 1.0*axis_h/(0.2+num);
    cairo_set_source_rgb(cr, 1, 1, 0);char text[32] = {0};//刻度值显示所需字符串
    // sprintf(text, "%.1lf", start);

    for(int i = 0; i < num; i++)
    {
        cairo_set_source_rgb(cr, 1, 1, 0);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x, temp_y-move_unit);
        actual_scale = scales[i];
        sprintf(text, "%d", actual_scale);

        // if(if_show_grid == FALSE)
        // {
            cairo_line_to(cr, temp_x-5, temp_y-move_unit);
            set_scale_tag(chart, text, temp_x, temp_y-move_unit, 3, i);
            cairo_move_to(cr, temp_x, temp_y-move_unit);
        // }
        // else
        // {
            cairo_move_to(grid_cr, temp_x, temp_y-move_unit);
            cairo_line_to(grid_cr, temp_x+axis_w, temp_y-move_unit);
            cairo_set_line_width(grid_cr, 1);
            cairo_stroke(grid_cr);
        // }
        chart->cairo_info.scale_double_info.y2_scale[i] = scales[i];
    }
    cairo_stroke(cr);
    chart->cairo_info.y2_unit = move_unit;
    // chart_update(dest_vc, surface, cr, w, h);
}

static void set_y2_scale_int(void *obj, int scales[], int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    //排序
    qsort(scales, num, sizeof(int), compare_int);
    chart->cairo_info.scale_int_info.y2_start = scales[0];
    chart->cairo_info.scale_int_info.y2_end = scales[num-1];

    chart->y2_dc_num = 0;
    chart->cairo_info.y2_scale_num = num;

    double temp_x, temp_y;
    // cairo_move_to(cr, origin_x, origin_y);
    double move_unit = 1.0*axis_h/(0.2+num);
    chart->cairo_info.y2_unit = move_unit;
    cairo_set_source_rgb(cr, 1, 1, 0);
    int actual_scale;
    char text[32] = {0};//刻度值显示所需字符串
    // sprintf(text, "%d", start);
    // set_scale_tag(chart, text, origin_x-15, origin_y, 3, 0);
    cairo_move_to(cr, origin_x, origin_y);
    for(int i = 0; i < num; i++)
    {
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x, temp_y-move_unit);
        actual_scale = scales[i];
        sprintf(text, "%d", actual_scale);
        // if(if_show_grid == FALSE)
        // {
            cairo_line_to(cr, temp_x-5, temp_y-move_unit);
            set_scale_tag(chart, text, temp_x, temp_y-move_unit, 3, i+1);
            cairo_move_to(cr, temp_x+1, temp_y-move_unit);
            
        // }
        // else
        // {
            cairo_move_to(grid_cr, temp_x, temp_y-move_unit);
            cairo_line_to(grid_cr, temp_x+axis_w, temp_y-move_unit);
            cairo_set_line_width(grid_cr, 1);
            cairo_stroke(grid_cr);
            // set_scale_tag(chart, text, temp_x, temp_y-move_unit, 3);
            
        // }
    }
    cairo_stroke(cr);
    chart->cairo_info.y2_unit = move_unit;
    // chart_update(dest_vc, surface, cr, w, h);
}

static void set_y2_scale_string(void *obj, char **content, int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->scale_cr;
    cairo_t *grid_cr = chart->grid_cr;
    cairo_t *scale_cr = chart->scale_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    chart->cairo_info.y2_scale_num = num;

    chart->y2_dc_num =0;
    chart->cairo_info.y2_scale_num = num;

    double temp_x, temp_y;
    double move_unit = 1.0*axis_h/(0.2+num);;
    chart->cairo_info.y2_unit = move_unit;
    char text[32] = {0};
    cairo_move_to(cr, origin_x, origin_y);
    for(int i = 0; i < num; i++)
    {
        sprintf(text, "%s", content[i]);
        cairo_set_source_rgba(cr, 1, 1, 0, 1);
        cairo_get_current_point(cr, &temp_x, &temp_y);
        cairo_move_to(cr, temp_x, temp_y - move_unit);
        // if(!if_show_grid)
            cairo_line_to(cr, temp_x + 5, temp_y - move_unit);
        // else 
            cairo_move_to(grid_cr, temp_x, temp_y-move_unit);
            cairo_line_to(grid_cr, temp_x+axis_w, temp_y-move_unit);
            cairo_set_line_width(grid_cr, 1);
            cairo_stroke(grid_cr);
        cairo_stroke(cr);
        set_scale_tag(chart, text, temp_x, temp_y - move_unit, 3, i);
        cairo_move_to(cr, temp_x, temp_y-move_unit);
    }
    chart->cairo_info.y2_unit = move_unit;
}


//创建轴
void _ChartCreateAxis(void *obj, int w, int h)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
	cairo_surface_t *axis_surface = chart->axis_surface;
	cairo_surface_t *data_surface = chart->data_surface;
	cairo_surface_t *tag_surface = chart->tag_surface;
	cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_t *cr = chart->cr;
    cairo_t *grid_cr = chart->grid_cr;
	cairo_t *axis_cr= chart->axis_cr;
	cairo_t *data_cr= chart->data_cr;
	cairo_t *tag_cr= chart->tag_cr;
	cairo_t *legend_cr= chart->legend_cr;

    cairo_set_operator(axis_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(axis_cr, 1, 1, 1, 0);
    cairo_paint(axis_cr);
    
    switch(chart->axis_type)
    {
        case TYPE_COORDINATES:
            paint_coordinates(chart);
            break;
        case TYPE_ECHARTS:
            paint_echarts(chart);
            break;
        case TYPE_PIE:
            paint_pie(chart);
            break;
        case TYPE_RADAR:
            paint_radar(chart);
        default: break;
    }
    merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
    chart_update(dest_vc, surface, cr, w, h);
    
    chart->axis_ok = TRUE;

    
    
    
}
//创建刻度double类型
void _ChartSetScaleDouble(void *obj, SCALE_TYPE scale_type, double scales[], double num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    if(!chart->axis_ok)
    {
        TLogE("Create scale error! No correct axis!\n");
    }
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
	cairo_surface_t *axis_surface = chart->axis_surface;
	cairo_surface_t *data_surface = chart->data_surface;
	cairo_surface_t *tag_surface = chart->tag_surface;
	cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->cr;
    cairo_t *grid_cr = chart->grid_cr;
	cairo_t *axis_cr= chart->axis_cr;
	cairo_t *data_cr= chart->data_cr;
	cairo_t *tag_cr= chart->tag_cr;
	cairo_t *legend_cr= chart->legend_cr;
    cairo_t *scale_cr= chart->scale_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;

    int if_show_grid;
    if_show_grid = chart->cairo_info.if_show_grid;

    switch(scale_type)
    {
        case (SCALE_TYPE_X):
            set_x_scale_double(chart, scales, num);
            chart->cairo_info.x_set_ok = 1;
            break;
        case (SCALE_TYPE_Y):
            set_y_scale_double(chart, scales, num);
            chart->cairo_info.y_set_ok = 1;
            break;
        case (SCALE_TYPE_Y2):
            set_y2_scale_double(chart, scales, num);
            chart->cairo_info.y2_set_ok = 1;
            break;
        default : break;

    }
    merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
    chart_update(dest_vc, surface, cr, w, h);
}
//创建刻度int类型
void _ChartSetScaleInt(void *obj, SCALE_TYPE scale_type, int scales[], int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    if(!chart->axis_ok)
    {
        TLogE("Create scale error! No correct axis!\n");
    }
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
	cairo_surface_t *axis_surface = chart->axis_surface;
	cairo_surface_t *data_surface = chart->data_surface;
	cairo_surface_t *tag_surface = chart->tag_surface;
	cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->cr;
    cairo_t *grid_cr = chart->grid_cr;
	cairo_t *axis_cr= chart->axis_cr;
	cairo_t *data_cr= chart->data_cr;
	cairo_t *tag_cr= chart->tag_cr;
	cairo_t *legend_cr= chart->legend_cr;
    cairo_t *scale_cr= chart->scale_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;

    int if_show_grid;
    if_show_grid = chart->cairo_info.if_show_grid;
    switch(scale_type)
    {
        case (SCALE_TYPE_X):
            set_x_scale_int(chart, scales, num);
            chart->cairo_info.x_set_ok = 1;
            break;
        case (SCALE_TYPE_Y):
            set_y_scale_int(chart, scales, num);
            chart->cairo_info.y_set_ok = 1;
            break;
        case (SCALE_TYPE_Y2):
            set_y2_scale_int(chart, scales, num);
            chart->cairo_info.y2_set_ok = 1;
            break;
        default: break;

    }
    merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
    chart_update(dest_vc, surface, cr, w, h);
}
//创建刻度string类型
void _ChartSetScaleString(void *obj, SCALE_TYPE scale_type, char **content, int num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    if(!chart->axis_ok)
    {
        TLogE("Create scale error! No correct axis!\n");
    }
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
	cairo_surface_t *axis_surface = chart->axis_surface;
	cairo_surface_t *data_surface = chart->data_surface;
	cairo_surface_t *tag_surface = chart->tag_surface;
	cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *scale_surface = chart->scale_surface;
    cairo_t *cr = chart->cr;
    cairo_t *grid_cr = chart->grid_cr;
	cairo_t *axis_cr= chart->axis_cr;
	cairo_t *data_cr= chart->data_cr;
	cairo_t *tag_cr= chart->tag_cr;
	cairo_t *legend_cr= chart->legend_cr;
    cairo_t *scale_cr= chart->scale_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;

    int if_show_grid;
    if_show_grid = chart->cairo_info.if_show_grid;
    switch(scale_type)
    {
        case (SCALE_TYPE_X):
            set_x_scale_string(chart, content, num);
            chart->cairo_info.x_set_ok = 1;
            break;
        case (SCALE_TYPE_Y):
            set_y_scale_string(chart, content, num);
            chart->cairo_info.y_set_ok = 1;
            break;
        case (SCALE_TYPE_Y2):
            set_y2_scale_string(chart, content, num);
            chart->cairo_info.y2_set_ok = 1;
            break;
        default: break;
    }

    // if(if_show_grid)
    // {
    //     merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
    // }
    merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
    chart_update(dest_vc, surface, cr, w, h);
    chart->cairo_info.if_show_scale = 1;
}





