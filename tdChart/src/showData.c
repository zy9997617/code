#include "showData.h"
#include <main.h>

/**
 *函数名:merge_surface 
 *参数:dest_cr:目标画笔
        source_sfc：源绘制表面
        op：操作方法
 *返回值：无
 *作用：将source_sfc上的图案通过op的方法，与dest_cr进行处理，由
 *      dest_cr进行绘制到其对应的表面
 **/
void merge_surface(cairo_t *dest_cr, cairo_surface_t *source_sfc, cairo_operator_t op)
{
    cairo_set_operator(dest_cr, op);
    cairo_set_source_surface(dest_cr, source_sfc, 0, 0);
    cairo_paint(dest_cr);
}
/**
 *函数名:showDataRadar 
 *参数:obj:对象；
        node：数据节点
        info_table：控件参数表
 *返回值：无
 *作用：绘制雷达图的实际操作
 **/
void showDataRadar(TWidget *obj, TwDataNode *node, TTable *info_table)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_t *cr = chart->axis_cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *tag_cr = chart->tag_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(data_cr, CAIRO_OPERATOR_SOURCE);

    double current_x, current_y;
    TwDataNode *datanode = TwDMGetChildNode(node, TRUE);
    int node_count = TwDMGetChildNodeCount(node);
    if(datanode == NULL)
        return;
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *nodetable = TTableGetTable(info_table, node_type);//配置文件节点信息表
    TTable *value_table = TwDMGetNodeValue(datanode, FALSE);//实际数据表

    double max, min, distance;
    double r, g, b, a;
    int if_fill;
    int if_show_tag;
    int level = 5;
    max = TTableGetFloat(nodetable, TStringID("max"));
    min = TTableGetFloat(nodetable, TStringID("min"));
    if_fill = TTableGetInt(nodetable, TStringID("if_fill"));
    if_show_tag = TTableGetInt(nodetable, TStringID("if_show_tag"));
    char *fill_color = TTableGetString(TTableGetTable(nodetable, TStringID("style")), TStringID("rgba"), NULL);
    int now_num = get_rank_of_rootnode((void *)chart, node);
    
    if(fill_color)
    {
        r = trans_rgba(fill_color, 1);
        g = trans_rgba(fill_color, 2);
        b = trans_rgba(fill_color, 3);
        a = trans_rgba(fill_color, 4);
    }
    else
    {
        char *rgba;
        rgba = chart->cairo_info.colors_for_pie[now_num];
        r = trans_rgba(rgba, 1);
        g = trans_rgba(rgba, 2);
        b = trans_rgba(rgba, 3);
        a = trans_rgba(rgba, 4);
    }
    if(if_fill)
    {
        a = 0.5;
    }
    
    if(TTableGetInt(nodetable, TStringID("level")) != 0)
    {
        level = TTableGetInt(nodetable, TStringID("level"));
    }
    distance = (max - min)/level;
    //根据节点个数，绘制对应的雷达图网格
    cairo_move_to(cr, origin_x, origin_y);//先移动到原点
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_set_line_width(cr, 1);
    if(node_count == 1)
    {
        for(int i = 0; i < level-1; i++)
        {
            cairo_move_to(cr, origin_x+pie_r/level*(i+1), origin_y);
            cairo_arc(cr, origin_x, origin_y, pie_r/level * (i+1), 0, 2*pi);
            cairo_stroke(cr);
        }
    }
    else if(node_count == 2)
    {
        for(int i = 0; i < level-1; i++)
        {
            cairo_move_to(cr, origin_x+pie_r/level*(i+1), origin_y);
            cairo_arc(cr, origin_x, origin_y, pie_r/level * (i+1), 0, 2*pi);
            cairo_stroke(cr);
        }
        cairo_move_to(cr, origin_x - pie_r, origin_y);
        cairo_line_to(cr, origin_x + pie_r, origin_y);
        cairo_stroke(cr);
    }
    else if(node_count == 3)
    {
        for(int i = 0; i < level; i++)
        {
            cairo_move_to(cr, origin_x, origin_y-pie_r/level*(i+1));
            cairo_line_to(cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
            cairo_line_to(cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
            cairo_line_to(cr, origin_x, origin_y-pie_r/level*(i+1));
        }
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x, origin_y - pie_r);
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x + pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x - pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
        cairo_stroke(cr);
    }
    else if(node_count == 4)
    {
        for(int i = 0; i < level; i++)
        {
            cairo_move_to(cr, origin_x, origin_y - pie_r/level*(i+1));
            cairo_line_to(cr, origin_x+pie_r/level*(i+1), origin_y);
            cairo_line_to(cr, origin_x, origin_y + pie_r/level*(i+1));
            cairo_line_to(cr, origin_x-pie_r/level*(i+1), origin_y);
            cairo_line_to(cr, origin_x, origin_y - pie_r/level*(i+1));
        }
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x, origin_y-pie_r);
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x + pie_r, origin_y);
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x, origin_y+pie_r);
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x - pie_r, origin_y);
        cairo_stroke(cr);
    }
    else if(node_count == 5)
    {
        for(int i = 0; i < level; i++)
        {
            cairo_move_to(cr, origin_x, origin_y - pie_r/level*(i+1));
            cairo_line_to(cr, origin_x + pie_r/level*(i+1)*cos(pi/10), origin_y-pie_r/level*(i+1)*sin(pi/10));
            cairo_line_to(cr, origin_x + pie_r/level*(i+1)*sin(pi/5), origin_y+pie_r/level*(i+1)*cos(pi/5));
            cairo_line_to(cr, origin_x - pie_r/level*(i+1)*sin(pi/5), origin_y+pie_r/level*(i+1)*cos(pi/5));
            cairo_line_to(cr, origin_x - pie_r/level*(i+1)*cos(pi/10), origin_y-pie_r/level*(i+1)*sin(pi/10));
            cairo_line_to(cr, origin_x, origin_y - pie_r/level*(i+1));
        }
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x, origin_y-pie_r);
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x + pie_r*cos(pi/10), origin_y - pie_r*sin(pi/10));
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x + pie_r*sin(pi/5), origin_y + pie_r*cos(pi/5));
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x - pie_r*sin(pi/5), origin_y + pie_r*cos(pi/5));
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x - pie_r*cos(pi/10), origin_y - pie_r*sin(pi/10));
        cairo_stroke(cr);
    }
    else if(node_count == 6)
    {
        for(int i = 0; i < level; i++)
        {
            cairo_move_to(cr, origin_x, origin_y - pie_r/level*(i+1));
            cairo_line_to(cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y - pie_r/level*(i+1)*sin(pi/6));
            cairo_line_to(cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
            cairo_line_to(cr, origin_x, origin_y + pie_r/level*(i+1));    
            cairo_line_to(cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
            cairo_line_to(cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y - pie_r/level*(i+1)*sin(pi/6));
            cairo_line_to(cr, origin_x, origin_y - pie_r/level*(i+1));    
        }
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x, origin_y - pie_r);
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x + pie_r*cos(pi/6), origin_y - pie_r*sin(pi/6));
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x + pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x, origin_y + pie_r);
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x - pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
        cairo_move_to(cr, origin_x, origin_y);
        cairo_line_to(cr, origin_x - pie_r*cos(pi/6), origin_y - pie_r*sin(pi/6));

        cairo_stroke(cr);
    }
    double length = 0;
    double first_x, first_y;
    cairo_move_to(cr, origin_x, origin_y);
    //创建用于绘制阴影面积的画笔
    
    //先绘制阴影
    //实际数据存储的字符串
    char value_2_str[32] = {0};
    char value_scale[32] = {0};
    //字符串长度
    int value_len, value_scale_len;
    //字体大小
    int font_size;
    font_size = chart->cairo_info.scale_font_size;
    T_ID data_type_x, data_type_y;
    data_type_x = TTableGetType(value_table, TStringID("x"));
    data_type_y = TTableGetType(value_table, TStringID("y"));
    for(int i = 0; i < node_count; i++)
    {
        if(data_type_y == T_VALUE_INT)
        {
            int value;
            value = TTableGetInt(value_table, TStringID("y"));
            sprintf(value_2_str, "%d", value);
            value_len = strlen(value_2_str);
            length = pie_r * value / (max-min);
        }
        else if(data_type_y == T_VALUE_FLOAT)
        {
            double value;
            value = TTableGetFloat(value_table, TStringID("y"));
            sprintf(value_2_str, "%.2lf", value);
            value_len = strlen(value_2_str);
            length = pie_r * value / (max-min);
        }
        
        //根据数据点数量分别绘制
        if(node_count == 6)
        {
            if(i == 0)
            {
                cairo_move_to(data_cr, origin_x, origin_y-length);
                cairo_get_current_point(data_cr, &first_x, &first_y);
                // if(if_show_tag)
                // {
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.0*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                // }
            }
            else
            {
                double delt_x, delt_y;
                delt_x = length * sin(pi/3*i);
                delt_y = length * cos(pi/3*i);
                cairo_line_to(data_cr, origin_x + delt_x, origin_y - delt_y);
                //数据注释
                // if(if_show_tag)
                {
                    // cairo_rotate(tag_cr, pi/3*i);
                    cairo_move_to(tag_cr, origin_x + delt_x-value_len/2.0, origin_y - delt_y-1*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    // cairo_rotate(tag_cr, -pi/3*i);
                    cairo_stroke(tag_cr);
                }
                if(i == node_count-1)
                {
                    cairo_line_to(data_cr, first_x, first_y);
                    cairo_set_source_rgba(data_cr, r, g, b, a);
                    cairo_set_source_rgba(tag_cr, 0, 0, 0, 255);
                    cairo_stroke(tag_cr);
                    if(if_fill)
                    {
                        cairo_fill_preserve(data_cr);
                        cairo_stroke(data_cr);
                    }
                    else 
                        cairo_stroke(data_cr);
                }
            }
        }
        else if(node_count == 5)
        {
            if(i == 0)
            {
                cairo_move_to(data_cr, origin_x, origin_y-length);
                cairo_get_current_point(data_cr, &first_x, &first_y);
                // if(if_show_tag)
                {
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    cairo_stroke(tag_cr);
                }
            }
            else
            {
                if(i == 1 || i == 4)
                {
                    double delt_x, delt_y;
                    delt_x = length*sin(i*2*pi/5);
                    delt_y = length*cos(i*2*pi/5);
                    cairo_line_to(data_cr, origin_x + delt_x, origin_y - delt_y);
                    //数据注释
                    // if(if_show_tag)
                    {
                        // cairo_rotate(tag_cr, i*2*pi/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        // cairo_rotate(tag_cr, -i*2*pi/5);
                        cairo_stroke(tag_cr);
                    }
                    if(i == 4)
                    {
                        cairo_line_to(data_cr, first_x, first_y);
                        cairo_set_source_rgba(data_cr, r, g, b, a);
                        if(if_fill)
                        {
                            cairo_fill_preserve(data_cr);
                            cairo_stroke(data_cr);
                        }
                        else 
                            cairo_stroke(data_cr);
                    }
                    
                }
                else if(i == 2)
                {
                    double delt_x, delt_y;
                    delt_x = length*sin(pi/5);
                    delt_y = length*cos(pi/5);
                    //数据注释
                    // if(if_show_tag)
                    {
                        // cairo_rotate(tag_cr, pi/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        // cairo_rotate(tag_cr, -pi/5);
                        cairo_stroke(tag_cr);
                    }
                    cairo_line_to(data_cr, origin_x + delt_x, origin_y + delt_y);
                }
                else if(i == 3)
                {
                    double delt_x, delt_y;
                    delt_x = length*sin(pi/5);
                    delt_y = length*cos(pi/5);
                    //数据注释
                    // if(if_show_tag)
                    {
                        // cairo_rotate(tag_cr, pi*4/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        // cairo_rotate(tag_cr, -pi*4/5);
                        cairo_stroke(tag_cr);
                    }
                    cairo_line_to(data_cr, origin_x - delt_x, origin_y + delt_y);
                }
            }
        }
        else if(node_count == 4)
        {
            if(i == 0)
            {
                cairo_move_to(data_cr, origin_x, origin_y-length);
                cairo_get_current_point(data_cr, &first_x, &first_y);
                // if(if_show_tag)
                {
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    cairo_stroke(tag_cr);
                }
            }
            else if(i == 1 || i == 2 || i == 3)
            {
                double delt_x, delt_y;
                delt_x = length*sin(i*pi/2);
                delt_y = length*cos(i*pi/2);
                cairo_line_to(data_cr, origin_x + delt_x, origin_y - delt_y);

                //数据注释
                // if(if_show_tag)
                {
                    // cairo_rotate(tag_cr, i*pi/2);
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    // cairo_rotate(tag_cr, -i*pi/2);
                    cairo_stroke(tag_cr);
                }
                if(i == 3)
                {
                    cairo_line_to(data_cr, first_x, first_y);
                    cairo_set_source_rgba(data_cr, r, g, b, a);
                    if(if_fill)
                    {
                        cairo_fill_preserve(data_cr);
                        cairo_stroke(data_cr);
                    }
                    else 
                        cairo_stroke(data_cr);
                }
               
            }
            
        }
        else if(node_count == 3)
        {
            if(i == 0)
            {
                cairo_move_to(data_cr, origin_x, origin_y-length);
                cairo_get_current_point(data_cr, &first_x, &first_y);

                //数据注释
                // if(if_show_tag)
                {
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    cairo_stroke(tag_cr);
                }
            }
            else 
            {
                double delt_x, delt_y;
                delt_x = length*sin(i*2*pi/3);
                delt_y = length*cos(i*2*pi/3);
                cairo_line_to(data_cr, origin_x + delt_x, origin_y - delt_y);

                //数据注释
                // if(if_show_tag)
                {
                    // cairo_rotate(tag_cr, i*2*pi/3);
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    // cairo_rotate(tag_cr, -i*2*pi/3);
                    cairo_stroke(tag_cr);
                }
                if(i == 2)
                {
                    cairo_line_to(data_cr, first_x, first_y);
                    cairo_set_source_rgba(data_cr, r, g, b, a);
                    if(if_fill)
                    {
                        cairo_fill_preserve(data_cr);
                        cairo_stroke(data_cr);
                    }
                    else 
                        cairo_stroke(data_cr);   
                }
            }
        }
        if(TwDMGetSiblingNode(datanode, TRUE))
        {
            datanode = TwDMGetSiblingNode(datanode, TRUE);
            value_table = TwDMGetNodeValue(datanode, FALSE);
        }
    }
    // merge_surface(data_cr, legend_sfc, CAIRO_OPERATOR_OVER);
    // merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);

    // cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    // cairo_set_source_surface(cr, data_sfc, 0, 0);
    // cairo_paint(cr);
    // cairo_set_source_surface(data_cr, surface, 0, 0);
    // cairo_paint(data_cr);

    // cairo_destroy(data_cr);
    // chart_update(dest_vc, surface, cr, w, h);
}


/**
 *函数名:showDataPie 
 *参数:obj:对象；
        node：数据节点
        info_table：控件参数表
 *返回值：无
 *作用：绘制饼图的实际操作
 **/
void showDataPie(TWidget *obj, TwDataNode *node, TTable *info_table)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->data_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_t *cr = chart->data_cr;
    cairo_t *tag_cr = chart->tag_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);

    double current_x, current_y;
    TwDataNode *datanode = TwDMGetChildNode(node, TRUE);
    int node_count = TwDMGetChildNodeCount(node);
    if(datanode == NULL)
        return;
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *nodetable = TTableGetTable(info_table, node_type);//配置文件节点信息表
    TTable *value_table = TwDMGetNodeValue(datanode, FALSE);//实际数据表

    T_ID data_type;
    data_type = TTableGetType(value_table, TStringID("y"));
    double percent = 0, value = 0, sum = 0, angle = 0;
    double last_x, last_y;
    last_x = origin_x+pie_r;
    last_y = origin_y;
    char *rgba;
    double r = 0, g = 0, b = 0, a = 0;
    int if_show_tag;
    if_show_tag = TTableGetInt(value_table, TStringID("if_show_tag"));
    TwDataNode *temp_node = datanode;
    TTable *temp_table = value_table;
    for(int i = 0; i < node_count; i++)
    {
        
        if(data_type == T_VALUE_INT)
            sum += TTableGetInt(temp_table, TStringID("y"));
        else if(data_type == T_VALUE_FLOAT)
        {
            sum += TTableGetFloat(temp_table, TStringID("y"));
        }
        temp_node = TwDMGetSiblingNode(temp_node, TRUE);
        temp_table = TwDMGetNodeValue(temp_node, FALSE);
    }
    //按比例绘制
    char value_2_str[32];
    memset(value_2_str, 0, 32);
    int value_len;
    int font_size;
    font_size = chart->cairo_info.scale_font_size;
    for(int i = 0; i < node_count; i++)
    {
        rgba = chart->cairo_info.colors_for_pie[i];
        r = trans_rgba(rgba, 1);
        g = trans_rgba(rgba, 2);
        b = trans_rgba(rgba, 3);
        a = trans_rgba(rgba, 4);
        if(data_type == T_VALUE_INT)
        {
            value = 1.0 * TTableGetInt(value_table, TStringID("y"));
            percent = value/sum;
            sprintf(value_2_str, "%.2lf%c", percent*100, '%');
            value_len = strlen(value_2_str);

            cairo_move_to(cr, origin_x, origin_y);
            cairo_line_to(cr, last_x, last_y);
            cairo_arc(cr, origin_x, origin_y, pie_r, angle, angle+percent*2*pi);
            cairo_get_current_point(cr, &last_x, &last_y);
            cairo_line_to(cr, origin_x, origin_y);
            cairo_set_source_rgba(cr, r, g, b, a);

            //标注数据
            // if(if_show_tag)
            {
                if(percent >= 0.05)
                {
                    cairo_move_to(tag_cr, origin_x, origin_y);
                    cairo_move_to(tag_cr, 1.0*pie_r*cos(angle+percent*pi)+origin_x,  \
                                    1.0*pie_r*sin(angle+percent*pi)+origin_y);
                    if(cos(angle+percent*pi) >= 0)
                    {
                        // cairo_line_to(tag_cr, 1.5*pie_r*cos(angle+percent*pi)+origin_x + 1.0*value_len*font_size/4+10,  \
                        //                 1.5*pie_r*sin(angle+percent*pi)+origin_y);
                        
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x+1.0*font_size/4*cos(angle+percent*pi), \
                                            current_y+1.0*font_size*sin(angle+percent*pi));
                        cairo_show_text(tag_cr, value_2_str);
                    }
                    else
                    {
                        // cairo_line_to(tag_cr, 1.5*pie_r*cos(angle+percent*pi)+origin_x - 1.0*value_len*font_size/4-10,  \
                        //                 1.5*pie_r*sin(angle+percent*pi)+origin_y);
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x+1.1*value_len*font_size/2*cos(angle+percent*pi), \
                                            current_y+1.1*font_size*sin(angle+percent*pi));
                        cairo_show_text(tag_cr, value_2_str);
                    }
                }
                cairo_stroke(tag_cr);
            }
            
            cairo_fill_preserve(cr);
            cairo_stroke(cr);

            angle+=percent*2*pi;

        }
        if(data_type == T_VALUE_FLOAT)
        {
            value = TTableGetFloat(value_table, TStringID("y"));
            percent = value/sum;
            sprintf(value_2_str, "%.2lf%c", percent*100, '%');
            value_len = strlen(value_2_str);

            cairo_move_to(cr, origin_x, origin_y);
            cairo_line_to(cr, last_x, last_y);
            cairo_arc(cr, origin_x, origin_y, pie_r, angle, angle+percent*2*pi);
            
            cairo_get_current_point(cr, &last_x, &last_y);
            cairo_line_to(cr, origin_x, origin_y);
            cairo_set_source_rgba(cr, r, g, b, a);

            //标注数据
            // if(if_show_tag)
            {
                if(percent >= 0.05)
                {
                    cairo_move_to(tag_cr, origin_x, origin_y);
                    cairo_move_to(tag_cr, 1.0*pie_r*cos(angle+percent*pi)+origin_x,  \
                                    1.0*pie_r*sin(angle+percent*pi)+origin_y);
                    if(cos(angle+percent*pi) >= 0)
                    {
                        // cairo_line_to(tag_cr, 1.5*pie_r*cos(angle+percent*pi)+origin_x + 1.0*value_len*font_size/4+10,  \
                        //                 1.5*pie_r*sin(angle+percent*pi)+origin_y);
                        
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x+1.0*font_size/4*cos(angle+percent*pi), \
                                            current_y+1.0*font_size*sin(angle+percent*pi));
                        cairo_show_text(tag_cr, value_2_str);
                    }
                    else
                    {
                        // cairo_line_to(tag_cr, 1.5*pie_r*cos(angle+percent*pi)+origin_x - 1.0*value_len*font_size/4-10,  \
                        //                 1.5*pie_r*sin(angle+percent*pi)+origin_y);
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x+1.1*value_len*font_size/2*cos(angle+percent*pi), \
                                            current_y+1.1*font_size*sin(angle+percent*pi));
                        cairo_show_text(tag_cr, value_2_str);
                    }
                }
                cairo_stroke(tag_cr);
            }

            cairo_fill_preserve(cr);
            cairo_stroke(cr);
            angle+=percent*2*pi;
        }
        datanode = TwDMGetSiblingNode(datanode, TRUE);
        value_table = TwDMGetNodeValue(datanode, FALSE);

    }
}


/**
 *函数名:showDataInt 
 *参数:obj:对象；
        node：数据节点
        info_table：控件参数表
 *返回值：无
 *作用：绘制平面直角坐标系相关图的实际操作，数据类型为整型
 **/
void showDataInt(TWidget *obj, TwDataNode *node, TTable *info_table)
{

    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->data_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    cairo_t *cr = chart->data_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *his_cr = chart->his_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;
    double x_unit, y_unit, y2_unit;
    x_unit = chart->cairo_info.x_unit;
    y_unit = chart->cairo_info.y_unit;
    y2_unit = chart->cairo_info.y2_unit;

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    
    int w = chart->surface_w;
    int h = chart->surface_h;

    int *x_scale = chart->cairo_info.scale_int_info.x_scale;

    int *y_scale_int = chart->cairo_info.scale_int_info.y_scale;
    int y_start_int = chart->cairo_info.scale_int_info.y_start;
    int y_end_int = chart->cairo_info.scale_int_info.y_end;
    int *y2_scale_int = chart->cairo_info.scale_int_info.y2_scale;
    int y2_start_int = chart->cairo_info.scale_int_info.y2_start;
    int y2_end_int = chart->cairo_info.scale_int_info.y2_end;

    double *y_scale_double = chart->cairo_info.scale_double_info.y_scale;
    double y_start_double = chart->cairo_info.scale_int_info.y_start;
    double y_end_double = chart->cairo_info.scale_int_info.y_end;
    double *y2_scale_double = chart->cairo_info.scale_int_info.y2_scale;
    double y2_start_double = chart->cairo_info.scale_int_info.y2_start;
    double y2_end_double = chart->cairo_info.scale_int_info.y2_end;

    char **x_scale_info = chart->cairo_info.scale_string_info.x_string;
    char **y_scale_info = chart->cairo_info.scale_string_info.y_string;
    char **y2_scale_info = chart->cairo_info.scale_string_info.y2_string;

    int x_scale_num = chart->cairo_info.x_scale_num;
    int y_scale_num = chart->cairo_info.y_scale_num;
    int y2_scale_num = chart->cairo_info.y2_scale_num;

    //保存当前点的位置信息
    double current_x, current_y;
    //绘制折线图时，保存上一点的位置信息；
    double last_x, last_y;

    TwDataNode *datanode = TwDMGetChildNode(node, TRUE);
    if(datanode == NULL)
        return;
    //记录显示了多少批次数据
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *nodetable = TTableGetTable(info_table, node_type);
    if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "histogram") == 0)
        chart->cairo_info.his_num++;    
    else if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "line") == 0)
        chart->cairo_info.line_num++;
    else if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "point") == 0)
        chart->cairo_info.point_num++;

    //是否显示数据标注
    int if_show_tag;
    if_show_tag = TTableGetInt(nodetable, TStringID("if_show_tag"));

    char value_2_str[32];
    memset(value_2_str, 0, 32);
    int value_len;
    int font_size;

    font_size = chart->cairo_info.scale_font_size;

    TTable *datavalue;
    //如果有多组数据均为柱状图，需要清空后重绘
    if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "histogram") == 0)
    {
        cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(his_cr);
        cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(his_cr, 1, 1, 1, 0);
        cairo_paint(his_cr);

        cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(his_tag_cr);
        cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(his_tag_cr, 1, 1, 1, 0);
        cairo_paint(his_tag_cr);
    }
    while(datanode)
    {
        datavalue = TwDMGetNodeValue(datanode, FALSE);
        // TTableDump(datavalue);
        //下面根据表中数据类型来进行不同的操作
        //x轴数据为整形时
        if(TTableGetType(datavalue, TStringID("x")) == T_VALUE_INT)
        {
            int target_x = TTableGetInt(datavalue, TStringID("x"));
            //x_target_scale可以标记当前正在处理同组数据中的第几个数据
            int x_target_scale = get_pos_of_scale_int(target_x, x_scale, x_scale_num);
            node_type = TwDMGetNodeType(datanode);
            TTable *node_info_table = TTableGetTable(info_table, node_type);//节点信息
            //柱状图
            if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "histogram") == 0)
            {
                int rect_w, rect_x; 
                double rect_r, rect_g, rect_b, rect_a;
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                rect_r = trans_rgba(rgba, 1);
                rect_g = trans_rgba(rgba, 2);
                rect_b = trans_rgba(rgba, 3);
                rect_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, rect_r, rect_g, rect_b, rect_a);
                //每组数据的矩形宽度
                // rect_w = ((axis_w-5)/(chart->cairo_info.x_scale_num)-5)/chart->cairo_info.his_num;
                rect_w = (x_unit-5)/chart->cairo_info.his_num;
                //计算矩形处于x轴的位置

                double move_step_x = 1.0*x_unit*(x_target_scale+1);
                cairo_move_to(cr, origin_x+move_step_x-x_unit/2, origin_y);
                TwDataNode *old_node;
                int cairo_his_num = 0;//标记绘制第机组矩形
                char *fill_rgba;
                double fill_r, fill_g, fill_b, fill_a;
                fill_r = 0.0;
                fill_g = 0.0;
                fill_b = 0.0;
                fill_a = 0.0;
                fill_rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);
                // int num = target_x/x_unit_int;
                for(int i = 0; i < chart->data_groups; i++)
                {
                    if(judge_showtype_if_is_his(chart->root_nodes[i]))
                    {
                        old_node = get_child_node(chart->root_nodes[i], x_target_scale);
                        datavalue = TwDMGetNodeValue(old_node, FALSE);
                        if(fill_rgba && chart->data_groups == 1)
                        {
                            fill_r = trans_rgba(fill_rgba, 1);
                            fill_g = trans_rgba(fill_rgba, 2);
                            fill_b = trans_rgba(fill_rgba, 3);
                            fill_a = trans_rgba(fill_rgba, 4);
                        }
                        else
                        {
                            fill_r = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 1);
                            fill_g = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 2);
                            fill_b = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 3);
                            fill_a = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 4);
                        }
                        
                    }
                    cairo_move_to(his_cr, origin_x+move_step_x-rect_w*chart->cairo_info.his_num/2+\
                                            cairo_his_num*rect_w, origin_y);
                    cairo_his_num++;
                    
                    cairo_get_current_point(his_cr, &current_x, &current_y);
                    if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                    {
                        double line_r, line_g, line_b, line_a;
                        
                        int target_y = TTableGetInt(datavalue, TStringID("y"));
                        // int scale_pos = get_pos_of_scale_int()
                        double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                        // printf("%d, %lf")
                        sprintf(value_2_str, "%d", target_y);
                        value_len = strlen(value_2_str);
                        //绘制矩形
                        cairo_line_to(his_cr, current_x, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-1);
                        cairo_line_to(his_cr, current_x, origin_y-1);
                        cairo_set_source_rgba(his_cr, fill_r, fill_g, fill_b, fill_a);
                        cairo_fill(his_cr); 
                        //显示数据标注
                        // if(if_show_tag)
                        {
                            cairo_move_to(his_tag_cr, current_x+rect_w/2-1.0*value_len*font_size/4,\
                                             origin_y-move_step_y-1);
                            // cairo_move_to(his_tag_cr, 0, 0);
                            cairo_set_source_rgba(his_tag_cr, 0, 0, 0, 1);
                            cairo_show_text(his_tag_cr, value_2_str);
                            cairo_stroke(his_tag_cr);
                            
                        } 
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                    }
                    else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                    {
                        double line_r, line_g, line_b, line_a;
                        double target_y = TTableGetFloat(datavalue, TStringID("y"));
                        double move_step_y = 1.0*((target_y-y_start_double) / ((y_end_double-y_start_double)/(y_scale_num-1)) + 1) * y_unit;
                        
                        sprintf(value_2_str, "%.2lf", target_y);
                        value_len = strlen(value_2_str);
                        //绘制矩形
                        cairo_line_to(his_cr, current_x, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-1);
                        cairo_line_to(his_cr, current_x, origin_y-1);
                        cairo_set_source_rgba(his_cr, fill_r, fill_g, fill_b, fill_a);
                        cairo_fill(his_cr); 
                        //显示数据标注
                        // if(if_show_tag)
                        {
                            cairo_move_to(his_tag_cr, current_x+rect_w/2-1.0*value_len*font_size/4,\
                                             origin_y-move_step_y-1);
                            // cairo_move_to(his_tag_cr, 0, 0);
                            cairo_set_source_rgba(his_tag_cr, 0, 0, 0, 1);
                            cairo_show_text(his_tag_cr, value_2_str);
                            cairo_stroke(his_tag_cr);
                            
                        } 
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                    }
                    else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                    {
                        char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                        int num = 0;
                        for(num; num < strlen(y_scale_info); num++)
                        {
                            if(strcmp(current_value, y_scale_info[num]) == 0)
                                break;
                        } 

                        double line_r, line_g, line_b, line_a;
                        double target_y = TTableGetFloat(datavalue, TStringID("y"));
                        double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                        //绘制矩形
                        cairo_line_to(cr, origin_x+move_step_x-chart->cairo_info.x_unit/2, origin_y-move_step_y);
                        cairo_line_to(cr, origin_x+move_step_x+chart->cairo_info.x_unit/2, origin_y-move_step_y);
                        cairo_line_to(cr, origin_x+move_step_x+chart->cairo_info.x_unit/2, origin_y);
                        cairo_line_to(cr, origin_x+move_step_x-chart->cairo_info.x_unit/2, origin_y);
                        //描线
                        // rgba = TTableGetString(style_table, TStringID("line_rgba"), NULL);
                        line_r = 0;
                        line_g = 0;
                        line_b = 0;
                        line_a = 255;
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr);  
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                        
                    }
                }
            }//柱状图

            //折线图
            else if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "line") == 0)
            {
                int point_r, if_show_value; 
                double circle_r, circle_g, circle_b, circle_a;
                point_r = TTableGetInt(node_info_table, TStringID("point_r"));
                if_show_value = TTableGetInt(node_info_table, TStringID("if_show_value"));
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                circle_r = trans_rgba(rgba, 1);
                circle_g = trans_rgba(rgba, 2);
                circle_b = trans_rgba(rgba, 3);
                circle_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, circle_r, circle_g, circle_b, circle_a);
                //计算点处于x轴的位置
                double move_step_x = 1.0*x_unit*(x_target_scale+1);
                cairo_move_to(cr, origin_x+move_step_x, origin_y);
                 
                if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                {
                    double line_r, line_g, line_b, line_a;
                    int target_y = TTableGetInt(datavalue, TStringID("y"));
                    double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;

                    sprintf(value_2_str, "%d", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);

                    //接下来获取上一个点的位置并连线
                    if(x_target_scale > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);
                    
                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                {
                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = 1.0*((target_y-y_start_double) / ((y_end_double-y_start_double)/(y_scale_num-1)) + 1) * y_unit;
                    
                    sprintf(value_2_str, "%.2lf", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(x_target_scale > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);
                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    }
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                {
                    char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                    int num = 0;
                    for(num; num < strlen(y_scale_info); num++)
                    {
                        if(strcmp(current_value, y_scale_info[num]) == 0)
                            break;
                    } 

                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                                        //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(x_target_scale > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);
                    /*
                    //显示数据标注
                    if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(cr, value_2_str);
                        cairo_set_source_rgba(cr, 0, 0, 0, 1);
                        cairo_stroke(cr);
                    }
                    */
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                    
                }
            }//折线图

            //散点图
            else if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "point") == 0)
            {
                int point_r, if_show_value; 
                double circle_r, circle_g, circle_b, circle_a;
                point_r = TTableGetInt(node_info_table, TStringID("point_r"));
                if_show_value = TTableGetInt(node_info_table, TStringID("if_show_value"));
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                circle_r = trans_rgba(rgba, 1);
                circle_g = trans_rgba(rgba, 2);
                circle_b = trans_rgba(rgba, 3);
                circle_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, circle_r, circle_g, circle_b, circle_a);
                //计算点处于x轴的位置
                double move_step_x = 1.0*x_unit*(x_target_scale+1);
                cairo_move_to(cr, origin_x+move_step_x, origin_y);
                if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                {
                    int target_y = TTableGetInt(datavalue, TStringID("y"));
                    double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%d", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  
                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    }
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                {
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = 1.0*((target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%.2lf", target_y); 
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  
                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    }
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                {
                    char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                    int num = 0;
                    for(num; num < strlen(y_scale_info); num++)
                    {
                        if(strcmp(current_value, y_scale_info[num]) == 0)
                            break;
                    } 

                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  
                    /*
                    //显示数据标注
                    if(if_show_tag)
                    {
                        cairo_move_to(cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(cr, value_2_str);
                        cairo_set_source_rgba(cr, 0, 0, 0, 1);
                        cairo_stroke(cr);
                    }
                    */
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
            }//散点图
        }//x轴数据为整形时

        datanode = TwDMGetSiblingNode(datanode, TRUE);
        
    }
}


/**
 *函数名:showDataDouble 
 *参数:obj:对象；
        node：数据节点
        info_table：控件参数表
 *返回值：无
 *作用：绘制平面直角坐标系相关图的实际操作，数据类型为浮点型
 **/
void showDataDouble(TWidget *obj, TwDataNode *node, TTable *info_table)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->data_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_t *cr = chart->data_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *his_cr = chart->his_cr;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);

    double x_unit, y_unit, y2_unit;
    x_unit = chart->cairo_info.x_unit;
    y_unit = chart->cairo_info.y_unit;
    y2_unit = chart->cairo_info.y2_unit;

    double *x_scale = chart->cairo_info.scale_double_info.x_scale;

    int *y_scale_int = chart->cairo_info.scale_int_info.y_scale;
    int y_start_int = chart->cairo_info.scale_int_info.y_start;
    int y_end_int = chart->cairo_info.scale_int_info.y_end;
    int *y2_scale_int = chart->cairo_info.scale_int_info.y2_scale;
    int y2_start_int = chart->cairo_info.scale_int_info.y2_start;
    int y2_end_int = chart->cairo_info.scale_int_info.y2_end;

    double *y_scale_double = chart->cairo_info.scale_double_info.y_scale;
    double y_start_double = chart->cairo_info.scale_int_info.y_start;
    double y_end_double = chart->cairo_info.scale_int_info.y_end;
    double *y2_scale_double = chart->cairo_info.scale_int_info.y2_scale;
    double y2_start_double = chart->cairo_info.scale_int_info.y2_start;
    double y2_end_double = chart->cairo_info.scale_int_info.y2_end;

    char **x_scale_info = chart->cairo_info.scale_string_info.x_string;
    char **y_scale_info = chart->cairo_info.scale_string_info.y_string;
    char **y2_scale_info = chart->cairo_info.scale_string_info.y2_string;

    int x_scale_num = chart->cairo_info.x_scale_num;
    int y_scale_num = chart->cairo_info.y_scale_num;
    int y2_scale_num = chart->cairo_info.y2_scale_num;

    //保存当前点的位置信息
    double current_x, current_y;
    //绘制折线图时，保存上一点的位置信息；
    double last_x, last_y;

    TwDataNode *datanode = TwDMGetChildNode(node, TRUE);
    if(datanode == NULL)
        return;
    //记录显示了多少批次数据
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *nodetable = TTableGetTable(info_table, node_type);
    if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "histogram") == 0)
        chart->cairo_info.his_num++;    
    else if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "line") == 0)
        chart->cairo_info.line_num++;
    else if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "point") == 0)
        chart->cairo_info.point_num++;
    
    //是否显示数据标注
    int if_show_tag;
    if_show_tag = TTableGetInt(nodetable, TStringID("if_show_tag"));

    char value_2_str[32];
    memset(value_2_str, 0, 32);
    int value_len; 
    int font_size;
    font_size = chart->cairo_info.scale_font_size;

    TTable *datavalue;

    if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "histogram") == 0)
    {
        cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(his_cr);
        cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(his_cr, 1, 1, 1, 0);
        cairo_paint(his_cr);

        cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(his_tag_cr);
        cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(his_tag_cr, 1, 1, 1, 0);
        cairo_paint(his_tag_cr);
    }
    while(datanode)
    {
        datavalue = TwDMGetNodeValue(datanode, FALSE);
        //下面根据表中数据类型来进行不同的操作
                //x轴数据为浮点形时
        if(TTableGetType(datavalue, TStringID("x")) == T_VALUE_FLOAT)
        {
            double target_x = TTableGetFloat(datavalue, TStringID("x"));
            //x_target_scale可以标记当前正在处理同组数据中的第几个数据
            int x_target_scale = get_pos_of_scale_double(target_x, x_scale, x_scale_num);
            T_ID node_type = TwDMGetNodeType(datanode);
            TTable *node_info_table = TTableGetTable(info_table, node_type);//节点信息
            //柱状图
            if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "histogram") == 0)
            {
                int rect_w; 
                double rect_r, rect_g, rect_b, rect_a;
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                rect_r = trans_rgba(rgba, 1);
                rect_g = trans_rgba(rgba, 2);
                rect_b = trans_rgba(rgba, 3);
                rect_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, rect_r, rect_g, rect_b, rect_a);
                //每组数据的矩形宽度
                // rect_w = ((axis_w-5)/(chart->cairo_info.x_scale_num)-5)/chart->cairo_info.his_num;
                rect_w = (x_unit-5)/chart->cairo_info.his_num;
                //计算矩形处于x轴的位置
                double move_step_x = 1.0*x_unit*(x_target_scale+1);
                cairo_move_to(cr, origin_x+move_step_x-chart->cairo_info.x_unit/2, origin_y);

                TwDataNode *old_node;
                int cairo_his_num = 0;//标记绘制第机组矩形
                char *fill_rgba;
                double fill_r, fill_g, fill_b, fill_a;
                fill_r = 0.0;
                fill_g = 0.0;
                fill_b = 0.0;
                fill_a = 0.0;
                fill_rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);

                // int num = (int)(target_x/x_unit);
                for(int i = 0; i < chart->data_groups; i++)
                {
                    if(judge_showtype_if_is_his(chart->root_nodes[i]))
                    {
                        old_node = get_child_node(chart->root_nodes[i], x_target_scale-1);
                        datavalue = TwDMGetNodeValue(old_node, FALSE);
                        if(fill_rgba && chart->data_groups == 1)
                        {
                            fill_r = trans_rgba(fill_rgba, 1);
                            fill_g = trans_rgba(fill_rgba, 2);
                            fill_b = trans_rgba(fill_rgba, 3);
                            fill_a = trans_rgba(fill_rgba, 4);
                        }
                        else
                        {
                            fill_r = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 1);
                            fill_g = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 2);
                            fill_b = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 3);
                            fill_a = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 4);
                        }
                    }
                    cairo_move_to(his_cr, origin_x+move_step_x-rect_w*chart->cairo_info.his_num/2+\
                                            cairo_his_num*rect_w, origin_y);
                    cairo_his_num++;
                    
                    cairo_get_current_point(his_cr, &current_x, &current_y);
                    if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                    {
                        double line_r, line_g, line_b, line_a;
                        int target_y = TTableGetInt(datavalue, TStringID("y"));
                        double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                        
                        sprintf(value_2_str, "%d", target_y);
                        value_len = strlen(value_2_str);
                        //绘制矩形
                        cairo_line_to(his_cr, current_x, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-1);
                        cairo_line_to(his_cr, current_x, origin_y-1);
                        cairo_set_source_rgba(his_cr, fill_r, fill_g, fill_b, fill_a);
                        cairo_fill(his_cr); 
                        //显示数据标注
                        // if(if_show_tag)
                        {
                            cairo_move_to(his_tag_cr, current_x+rect_w/2-1.0*value_len*font_size/4,\
                                             origin_y-move_step_y-1);
                            // cairo_move_to(his_tag_cr, 0, 0);
                            cairo_set_source_rgba(his_tag_cr, 0, 0, 0, 1);
                            cairo_show_text(his_tag_cr, value_2_str);
                            cairo_stroke(his_tag_cr);
                            
                        } 
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                    }
                    else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                    {
                        double line_r, line_g, line_b, line_a;
                        double target_y = TTableGetFloat(datavalue, TStringID("y"));
                        double move_step_y = 1.0*((target_y-y_start_double) / ((y_end_double-y_start_double)/(y_scale_num-1)) + 1) * y_unit;
                        
                        sprintf(value_2_str, "%.2lf", target_y);
                        value_len = strlen(value_2_str);
                        //绘制矩形
                        cairo_line_to(his_cr, current_x, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-1);
                        cairo_line_to(his_cr, current_x, origin_y-1);
                        cairo_set_source_rgba(his_cr, fill_r, fill_g, fill_b, fill_a);
                        cairo_fill(his_cr); 
                        //显示数据标注
                        // if(if_show_tag)
                        {
                            cairo_move_to(his_tag_cr, current_x+rect_w/2-1.0*value_len*font_size/4,\
                                             origin_y-move_step_y-1);
                            // cairo_move_to(his_tag_cr, 0, 0);
                            cairo_set_source_rgba(his_tag_cr, 0, 0, 0, 1);
                            cairo_show_text(his_tag_cr, value_2_str);
                            cairo_stroke(his_tag_cr);
                            
                        } 
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                    }
                    else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                    {
                        char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                        int num = 0;
                        for(num; num < strlen(y_scale_info); num++)
                        {
                            if(strcmp(current_value, y_scale_info[num]) == 0)
                                break;
                        } 

                        double line_r, line_g, line_b, line_a;
                        double target_y = TTableGetFloat(datavalue, TStringID("y"));
                        double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                        //绘制矩形
                        cairo_line_to(cr, origin_x+move_step_x-chart->cairo_info.x_unit/2, origin_y-move_step_y);
                        cairo_line_to(cr, origin_x+move_step_x+chart->cairo_info.x_unit/2, origin_y-move_step_y);
                        cairo_line_to(cr, origin_x+move_step_x+chart->cairo_info.x_unit/2, origin_y);
                        cairo_line_to(cr, origin_x+move_step_x-chart->cairo_info.x_unit/2, origin_y);
                        //描线
                        // rgba = TTableGetString(style_table, TStringID("line_rgba"), NULL);
                        line_r = 0;
                        line_g = 0;
                        line_b = 0;
                        line_a = 255;;
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr);  
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                        
                    }
                }
            }//柱状图

            //折线图
            else if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "line") == 0)
            {
                int point_r, if_show_value; 
                double circle_r, circle_g, circle_b, circle_a;
                point_r = TTableGetInt(node_info_table, TStringID("point_r"));
                if_show_value = TTableGetInt(node_info_table, TStringID("if_show_value"));
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                circle_r = trans_rgba(rgba, 1);
                circle_g = trans_rgba(rgba, 2);
                circle_b = trans_rgba(rgba, 3);
                circle_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, circle_r, circle_g, circle_b, circle_a);
                //计算点处于x轴的位置
                double move_step_x = 1.0*x_unit*(x_target_scale+1);
                cairo_move_to(cr, origin_x+move_step_x, origin_y);
                if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                {
                    double line_r, line_g, line_b, line_a;
                    int target_y = TTableGetInt(datavalue, TStringID("y"));
                    double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%d", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(x_target_scale > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                {
                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = 1.0*((target_y-y_start_double) / ((y_end_double-y_start_double)/(y_scale_num-1)) + 1) * y_unit;
                    
                    sprintf(value_2_str, "%.2lf", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(x_target_scale > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    }  
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                {
                    char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                    int num = 0;
                    for(num; num < strlen(y_scale_info); num++)
                    {
                        if(strcmp(current_value, y_scale_info[num]) == 0)
                            break;
                    } 

                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                                        //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(x_target_scale > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                    
                }
            }//折线图

            //散点图
            else if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "point") == 0)
            {
                int point_r, if_show_value; 
                double circle_r, circle_g, circle_b, circle_a;
                point_r = TTableGetInt(node_info_table, TStringID("point_r"));
                if_show_value = TTableGetInt(node_info_table, TStringID("if_show_value"));
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                circle_r = trans_rgba(rgba, 1);
                circle_g = trans_rgba(rgba, 2);
                circle_b = trans_rgba(rgba, 3);
                circle_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, circle_r, circle_g, circle_b, circle_a);
                //计算点处于x轴的位置
                double move_step_x = 1.0*x_unit*(x_target_scale+1);
                cairo_move_to(cr, origin_x+move_step_x, origin_y);
                if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                {
                    double line_r, line_g, line_b, line_a;
                    int target_y = TTableGetInt(datavalue, TStringID("y"));
                    double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%d", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                {
                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = 1.0*((target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%.2lf", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                {
                    char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                    int num = 0;
                    for(num; num < strlen(y_scale_info); num++)
                    {
                        if(strcmp(current_value, y_scale_info[num]) == 0)
                            break;
                    } 

                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
            }//散点图
        }//x轴数据为整形时

        datanode = TwDMGetSiblingNode(datanode, TRUE);
    }
}



/**
 *函数名:showDataString 
 *参数:obj:对象；
        node：数据节点
        info_table：控件参数表
 *返回值：无
 *作用：绘制平面直角坐标系相关图的实际操作，数据类型为字符串
 **/
void showDataString(TWidget *obj, TwDataNode *node, TTable *info_table)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->data_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    cairo_t *cr = chart->data_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *his_cr = chart->his_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;

    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double axis_w, axis_h;
    axis_w = chart->cairo_info.axis_w;
    axis_h = chart->cairo_info.axis_h;

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);

    double x_unit, y_unit, y2_unit;
    x_unit = chart->cairo_info.x_unit;
    y_unit = chart->cairo_info.y_unit;
    y2_unit = chart->cairo_info.y2_unit;

    int *x_scale = chart->cairo_info.scale_int_info.x_scale;

    int *y_scale_int = chart->cairo_info.scale_int_info.y_scale;
    int y_start_int = chart->cairo_info.scale_int_info.y_start;
    int y_end_int = chart->cairo_info.scale_int_info.y_end;
    int *y2_scale_int = chart->cairo_info.scale_int_info.y2_scale;
    int y2_start_int = chart->cairo_info.scale_int_info.y2_start;
    int y2_end_int = chart->cairo_info.scale_int_info.y2_end;

    double *y_scale_double = chart->cairo_info.scale_double_info.y_scale;
    double y_start_double = chart->cairo_info.scale_int_info.y_start;
    double y_end_double = chart->cairo_info.scale_int_info.y_end;
    double *y2_scale_double = chart->cairo_info.scale_int_info.y2_scale;
    double y2_start_double = chart->cairo_info.scale_int_info.y2_start;
    double y2_end_double = chart->cairo_info.scale_int_info.y2_end;

    char **x_scale_info = chart->cairo_info.scale_string_info.x_string;
    char **y_scale_info = chart->cairo_info.scale_string_info.y_string;
    char **y2_scale_info = chart->cairo_info.scale_string_info.y2_string;

    int x_scale_num = chart->cairo_info.x_scale_num;
    int y_scale_num = chart->cairo_info.y_scale_num;
    int y2_scale_num = chart->cairo_info.y2_scale_num;

    //保存当前点的位置信息
    double current_x, current_y;
    //绘制折线图时，保存上一点的位置信息；
    double last_x, last_y;

    TwDataNode *datanode = TwDMGetChildNode(node, TRUE);
    if(datanode == NULL)
        return;
    //记录显示了多少批次数据
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *nodetable = TTableGetTable(info_table, node_type);
    if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "histogram") == 0)
        chart->cairo_info.his_num++;    
    else if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "line") == 0)
        chart->cairo_info.line_num++;
    else if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "point") == 0)
        chart->cairo_info.point_num++;
    
    int if_show_tag;
    char value_2_str[32];
    memset(value_2_str, 0, 32);
    int value_len = 0;
    int font_size = 25;
    font_size = chart->cairo_info.scale_font_size;
    TTable *datavalue;
    char *current_value;
    //如果有多组数据均为柱状图，需要清空后重绘
    if(strcmp(TTableGetString(nodetable, TStringID("show_type"), NULL), "histogram") == 0)
    {
        cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(his_cr);
        cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(his_cr, 1, 1, 1, 0);
        cairo_paint(his_cr);

        cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(his_tag_cr);
        cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(his_tag_cr, 1, 1, 1, 0);
        cairo_paint(his_tag_cr);
    }

    //按照加入节点的顺序依次绘制
    while(datanode)
    {
        datavalue = TwDMGetNodeValue(datanode, FALSE);
        current_value = TTableGetString(datavalue, TStringID("x"), NULL);
        int num = 0;
        for(num; num < chart->cairo_info.x_scale_num; num++)
        {
            if(strcmp(current_value, chart->cairo_info.scale_string_info.x_string[num]) == 0)
                break;
        } 
        //下面根据表中数据类型来进行不同的操作
        if(TTableGetType(datavalue, TStringID("x")) == T_VALUE_STRING)
        {
            double target_x = TTableGetFloat(datavalue, TStringID("y"));
            T_ID node_type = TwDMGetNodeType(datanode);
            TTable *node_info_table = TTableGetTable(info_table, node_type);//节点信息
            //柱状图
            if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "histogram") == 0)
            {
                double rect_w, rect_x;
                double rect_r, rect_g, rect_b, rect_a;
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                rect_r = trans_rgba(rgba, 1);
                rect_g = trans_rgba(rgba, 2);
                rect_b = trans_rgba(rgba, 3);
                rect_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, rect_r, rect_g, rect_b, rect_a);
                //每组数据的矩形宽度
                // if(chart->cairo_info.his_num > 1)
                // {
                    // rect_w = ((axis_w-5)/(chart->cairo_info.x_scale_num)-5)/chart->cairo_info.his_num;
                rect_w = (x_unit-5)/chart->cairo_info.his_num;
                // }


                //计算矩形处于x轴的位置
                double move_step_x = 1.0*x_unit*(num+1);
                TwDataNode *old_node;
                int cairo_his_num = 0;//标记绘制第机组矩形
                char *fill_rgba;
                double fill_r, fill_g, fill_b, fill_a;
                fill_r = 0.0;
                fill_g = 0.0;
                fill_b = 0.0;
                fill_a = 0.0;
                fill_rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);
                for(int i = 0; i < chart->data_groups; i++)
                {
                    if(judge_showtype_if_is_his(chart->root_nodes[i]))
                    {
                        old_node = get_child_node(chart->root_nodes[i], num);
                        datavalue = TwDMGetNodeValue(old_node, FALSE);
                        if(fill_rgba && chart->data_groups == 1)
                        {
                            fill_r = trans_rgba(fill_rgba, 1);
                            fill_g = trans_rgba(fill_rgba, 2);
                            fill_b = trans_rgba(fill_rgba, 3);
                            fill_a = trans_rgba(fill_rgba, 4);
                        }
                        else
                        {
                            fill_r = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 1);
                            fill_g = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 2);
                            fill_b = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 3);
                            fill_a = trans_rgba(chart->cairo_info.colors_for_pie[cairo_his_num], 4);
                        }
                    }
                    cairo_move_to(his_cr, origin_x+move_step_x-rect_w*chart->cairo_info.his_num/2+\
                                            cairo_his_num*rect_w, origin_y);
                    cairo_his_num++;
                    
                    cairo_get_current_point(his_cr, &current_x, &current_y);
                    if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                    {
                        double line_r, line_g, line_b, line_a;
                        int target_y = TTableGetInt(datavalue, TStringID("y"));
                        double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                        
                        sprintf(value_2_str, "%d", target_y);
                        value_len = strlen(value_2_str);
                        //绘制矩形
                        cairo_line_to(his_cr, current_x, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-1);
                        cairo_line_to(his_cr, current_x, origin_y-1);
                        cairo_set_source_rgba(his_cr, fill_r, fill_g, fill_b, fill_a);
                        cairo_fill(his_cr);
                        // cairo_stroke(his_cr);

                        //显示数据标注
                        // if(if_show_tag)
                        {
                            cairo_move_to(his_tag_cr, current_x+rect_w/2-1.0*value_len*font_size/4,\
                                             origin_y-move_step_y-1);
                            // cairo_move_to(his_tag_cr, 0, 0);
                            cairo_set_source_rgba(his_tag_cr, 0, 0, 0, 1);
                            cairo_show_text(his_tag_cr, value_2_str);
                            cairo_stroke(his_tag_cr);
                            
                        } 
                        
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                    }
                    else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                    {
                        double line_r, line_g, line_b, line_a;
                        double target_y = TTableGetFloat(datavalue, TStringID("y"));
                        double move_step_y = 1.0*((target_y-y_start_double) / ((y_end_double-y_start_double)/(y_scale_num-1)) + 1) * y_unit;
                        sprintf(value_2_str, "%.1lf", target_y);
                        value_len = strlen(value_2_str);
                        //绘制矩形
                        cairo_line_to(his_cr, current_x, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-1);
                        cairo_line_to(his_cr, current_x, origin_y-1);
                        cairo_set_source_rgba(his_cr, fill_r, fill_g, fill_b, fill_a);
                        cairo_fill(his_cr);
                        // cairo_stroke(his_cr);

                        //显示数据标注
                        // if(if_show_tag)
                        {
                            cairo_move_to(his_tag_cr, current_x+rect_w/2-1.0*value_len*font_size/4,\
                                             origin_y-move_step_y-1);
                            // cairo_move_to(his_tag_cr, 0, 0);
                            cairo_set_source_rgba(his_tag_cr, 0, 0, 0, 1);
                            cairo_show_text(his_tag_cr, value_2_str);
                            cairo_stroke(his_tag_cr);
                            
                        } 
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                    }
                    else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                    {
                        char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                        int num = 0;
                        for(num; num < strlen(y_scale_info); num++)
                        {
                            if(strcmp(current_value, y_scale_info[num]) == 0)
                                break;
                        } 

                        double line_r, line_g, line_b, line_a;
                        double target_y = TTableGetFloat(datavalue, TStringID("y"));
                        double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                        //绘制矩形
                        cairo_line_to(his_cr, current_x, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-move_step_y-1);
                        cairo_line_to(his_cr, current_x+rect_w, origin_y-1);
                        cairo_line_to(his_cr, current_x, origin_y-1);
                        cairo_set_source_rgba(his_cr, fill_r, fill_g, fill_b, fill_a);
                        cairo_fill(his_cr);
                        // cairo_stroke(his_cr);
                        //显示数据标注
                        // if(if_show_tag)
                        {
                            cairo_move_to(his_tag_cr, current_x+rect_w/2-1.0*value_len*font_size/4,\
                                             origin_y-move_step_y-1);
                            // cairo_move_to(his_tag_cr, 0, 0);
                            cairo_set_source_rgba(his_tag_cr, 0, 0, 0, 1);
                            cairo_show_text(his_tag_cr, value_2_str);
                            cairo_stroke(his_tag_cr);
                            
                        } 
                        //更新画布
                        // chart_update(dest_vc, surface, cr, w, h);
                        
                    }
                }
            }//柱状图
            

            //折线图
            else if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "line") == 0)
            {
                int point_r, if_show_value; 
                double circle_r, circle_g, circle_b, circle_a;
                point_r = TTableGetInt(node_info_table, TStringID("point_r"));
                if_show_value = TTableGetInt(node_info_table, TStringID("if_show_value"));
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                circle_r = trans_rgba(rgba, 1);
                circle_g = trans_rgba(rgba, 2);
                circle_b = trans_rgba(rgba, 3);
                circle_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, circle_r, circle_g, circle_b, circle_a);
                //计算点处于x轴的位置
                double move_step_x = 1.0*x_unit*(num+1);
                cairo_move_to(cr, origin_x+move_step_x, origin_y);
                if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                {
                    double line_r, line_g, line_b, line_a;
                    int target_y = TTableGetInt(datavalue, TStringID("y"));
                    double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%d", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(num > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                {
                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = 1.0*((target_y-y_start_double) / ((y_end_double-y_start_double)/(y_scale_num-1)) + 1) * y_unit;
                    
                    sprintf(value_2_str, "%.2lf", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(num > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                {
                    char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                    int num = 0;
                    for(num; num < strlen(y_scale_info); num++)
                    {
                        if(strcmp(current_value, y_scale_info[num]) == 0)
                            break;
                    } 

                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                                        //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    //接下来获取上一个点的位置并连线
                    if(num > 0)
                    {
                        cairo_move_to(cr, origin_x+move_step_x-point_r, origin_y-move_step_y);
                        cairo_line_to(cr, last_x, last_y);
                        rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                        line_r = trans_rgba(rgba, 1);
                        line_g = trans_rgba(rgba, 2);
                        line_b = trans_rgba(rgba, 3);
                        line_a = trans_rgba(rgba, 4);
                        cairo_set_source_rgba(cr, line_r, line_g, line_b, line_a);
                        cairo_set_line_width(cr, TTableGetInt(style_table, TStringID("line_width")));
                        cairo_stroke(cr); 
                    }
                    cairo_get_current_point(cr, &last_x, &last_y);
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                    
                }
            }//折线图

            //散点图
            else if(strcmp(TTableGetString(node_info_table, TStringID("show_type"), NULL), "point") == 0)
            {
                int point_r, if_show_value; 
                double circle_r, circle_g, circle_b, circle_a;
                point_r = TTableGetInt(node_info_table, TStringID("point_r"));
                if_show_value = TTableGetInt(node_info_table, TStringID("if_show_value"));
                TTable *style_table = TTableGetTable(node_info_table, TStringID("style"));
                char *rgba = TTableGetString(style_table, TStringID("rgba"), NULL);
                circle_r = trans_rgba(rgba, 1);
                circle_g = trans_rgba(rgba, 2);
                circle_b = trans_rgba(rgba, 3);
                circle_a = trans_rgba(rgba, 4);
                cairo_set_source_rgba(cr, circle_r, circle_g, circle_b, circle_a);
                //计算点处于x轴的位置
                double move_step_x = 1.0*x_unit*(num+1);
                cairo_move_to(cr, origin_x+move_step_x, origin_y);
                if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_INT)
                {
                    double line_r, line_g, line_b, line_a;
                    int target_y = TTableGetInt(datavalue, TStringID("y"));
                    double move_step_y = (1.0*(target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)*1.0) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%d", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_FLOAT)
                {
                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = 1.0*((target_y-y_start_int) / ((y_end_int-y_start_int)/(y_scale_num-1)) + 1)*y_unit;
                    
                    sprintf(value_2_str, "%.2lf", target_y);
                    value_len = strlen(value_2_str);
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  

                    //显示数据标注
                    // if(if_show_tag)
                    {
                        cairo_move_to(tag_cr, origin_x+move_step_x - 1.0*value_len*font_size/4, \
                                        origin_y-move_step_y-1);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_set_source_rgba(tag_cr, 0, 0, 0, 1);
                        cairo_stroke(tag_cr);
                    } 
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
                else if(TTableGetType(datavalue, TStringID("y")) == T_VALUE_STRING)
                {
                    char *current_value = TTableGetString(datavalue, TStringID("y"), NULL);
                    int num = 0;
                    for(num; num < strlen(y_scale_info); num++)
                    {
                        if(strcmp(current_value, y_scale_info[num]) == 0)
                            break;
                    } 

                    double line_r, line_g, line_b, line_a;
                    double target_y = TTableGetFloat(datavalue, TStringID("y"));
                    double move_step_y = (axis_h-5)/(chart->cairo_info.y_scale_num)*num;
                    //绘制点
                    cairo_move_to(cr, origin_x+move_step_x+point_r, origin_y-move_step_y);
                    cairo_arc(cr, origin_x+move_step_x, origin_y-move_step_y, point_r, 0, 2*pi);
                    cairo_fill_preserve(cr);
                    
                    cairo_stroke(cr);  
                    //更新画布
                    // chart_update(dest_vc, surface, cr, w, h);
                }
            }//散点图

        }//x轴数据为整形时
        datanode = TwDMGetSiblingNode(datanode, TRUE);
    }
}


/**
 *函数名:createLegend 
 *参数:obj:对象；
        node：数据节点
        info_table：控件参数表
 *返回值：无
 *作用：绘制图例的实际操作
 **/
void createLegend(TWidget *obj, TwDataNode *show_root, TTable *info_table)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    

    int w = chart->surface_w;
    int h = chart->surface_h;
    double temp_x = 0, temp_y = 0;

    TTable *node_info_table;
    T_ID node_type;
    int if_show_legend;
    char *legend_text;
    char *show_type;
    char *rgba;
    double r, g, b, a;
    int node_count;
    int font_size;
    double origin_x, origin_y;
    double pie_r;
    double current_x, current_y;

    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    pie_r = chart->cairo_info.pie_r;
    font_size = chart->cairo_info.scale_font_size;

    node_type = TwDMGetNodeType(TwDMGetChildNode(show_root, TRUE));
    node_info_table = TTableGetTable(info_table, node_type);
    // legend_text = TTableGetString(node_info_table, TStringID("legend_text"), NULL);
    show_type = TTableGetString(node_info_table, TStringID("show_type"), NULL);
    rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);
    // legend_text = TTableGetString(TwDMGetNodeValue(show_root, FALSE), TStringID("legend_text"), NULL);
    node_count = TwDMGetChildNodeCount(show_root);
    r = trans_rgba(rgba, 1);
    g = trans_rgba(rgba, 2);
    b = trans_rgba(rgba, 3);
    a = trans_rgba(rgba, 4);
    if(strcmp(show_type, "histogram") == 0 && chart->axis_type == TYPE_COORDINATES)
    {
        cairo_set_operator(legend_cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(legend_cr);
        cairo_set_operator(legend_cr, CAIRO_OPERATOR_SOURCE);
        for(int i = 0; i < chart->data_groups; i++)
        {
            if(rgba && chart->data_groups == 1)
            {
                r = trans_rgba(rgba, 1);
                g = trans_rgba(rgba, 2);
                b = trans_rgba(rgba, 3);
                a = trans_rgba(rgba, 4);
            }
            else
            {
                r = trans_rgba(chart->cairo_info.colors_for_pie[i], 1);
                g = trans_rgba(chart->cairo_info.colors_for_pie[i], 2);
                b = trans_rgba(chart->cairo_info.colors_for_pie[i], 3);
                a = trans_rgba(chart->cairo_info.colors_for_pie[i], 4);
            }
            cairo_move_to(legend_cr, w-50.0, 5+i * 20.0);
            cairo_get_current_point(legend_cr, &temp_x, &temp_y);
            cairo_rectangle(legend_cr, temp_x, temp_y, 15, 8);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_fill(legend_cr);

            legend_text = TTableGetString(TwDMGetNodeValue(chart->root_nodes[i], FALSE), TStringID("legend_text"), NULL);
            if(!chart->legend_dc[i])
            {
                chart->legend_dc[i] = DcCreate(DC_INPUT_WINDOW, chart->vc, temp_x+16, temp_y-6, 30, 20, (TWidget *)obj);
                // DcShowHide(chart->legend_dc[chart->cairo_info.legend_num], FALSE);
                DcSetTextAline(chart->legend_dc[i], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
                DcSetText(chart->legend_dc[i], legend_text, -1, 0);
                chart->cairo_info.legend_num = i+1;
            }
            else
            {
                DcShowHide(chart->legend_dc[i], TRUE);
                DcMove(chart->legend_dc[i], temp_x+16, temp_y-6);
                DcSetText(chart->legend_dc[i], legend_text, -1, 0);
                chart->cairo_info.legend_num = i+1;
            }
        }

        
    }
    else if(strcmp(show_type, "line") == 0 && chart->axis_type == TYPE_COORDINATES)
    {
        cairo_move_to(legend_cr, w-50.0, 10+chart->cairo_info.legend_num * 20.0);
        cairo_get_current_point(legend_cr, &temp_x, &temp_y);
        cairo_line_to(legend_cr, temp_x+15, temp_y);
        cairo_set_line_width(legend_cr, 2);
        cairo_set_source_rgba(legend_cr, r, g, b, a);
        cairo_stroke(legend_cr);

        chart->legend_dc[chart->cairo_info.legend_num] = DcCreate(DC_INPUT_WINDOW, chart->vc, temp_x+16, temp_y-10, 30, 20, (TWidget *)obj);
        // DcShowHide(chart->legend_dc[chart->cairo_info.legend_num], FALSE);
        DcSetTextAline(chart->legend_dc[chart->cairo_info.legend_num], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
        DcSetText(chart->legend_dc[chart->cairo_info.legend_num], legend_text, -1, 0);
        chart->cairo_info.legend_num++;

    }
    else if(strcmp(show_type, "point") == 0 && chart->axis_type == TYPE_COORDINATES)
    {
        cairo_move_to(legend_cr, w-50.0, 10+chart->cairo_info.legend_num * 20.0);
        cairo_get_current_point(legend_cr, &temp_x, &temp_y);
        cairo_move_to(legend_cr, temp_x+7, temp_y+2);
        cairo_arc(legend_cr, temp_x+9, temp_y+2, 2, 0, 2*pi);
        cairo_set_source_rgba(legend_cr, r, g, b, a);
        cairo_fill(legend_cr);

        chart->legend_dc[chart->cairo_info.legend_num] = DcCreate(DC_INPUT_WINDOW, chart->vc, temp_x+16, temp_y-6, 30, 20, (TWidget *)obj);
        // DcShowHide(chart->legend_dc[chart->cairo_info.legend_num], FALSE);
        DcSetTextAline(chart->legend_dc[chart->cairo_info.legend_num], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
        DcSetText(chart->legend_dc[chart->cairo_info.legend_num], legend_text, -1, 0);
        chart->cairo_info.legend_num++;
    }
    else if(strcmp(show_type, "pie") == 0 && chart->axis_type == TYPE_PIE)
    {
        TwDataNode *temp_node;
        TTable *temp_table;
        temp_node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
        temp_table = TwDMGetNodeValue(temp_node, FALSE);
        char temp_str[32] = {0};
        
        for(int i = 0; i < node_count; i++)
        {
            if(TTableGetType(temp_table, TStringID("x")) == T_VALUE_INT)
            {
                sprintf(temp_str, "%d", TTableGetInt(temp_table, TStringID("x")));
            }
            else if(TTableGetType(temp_table, TStringID("x")) == T_VALUE_FLOAT)
            {
                sprintf(temp_str, "%.2lf", TTableGetFloat(temp_table, TStringID("x")));
            }
            else if(TTableGetType(temp_table, TStringID("x")) == T_VALUE_STRING)
            {
                sprintf(temp_str, "%s", TTableGetString(temp_table, TStringID("x"), NULL));
            }
            TTable *str_table = TTableCreate();
            TTableAddString(str_table, TStringID("str"), temp_str, -1);
            
            r = trans_rgba(chart->cairo_info.colors_for_pie[i], 1);
            g = trans_rgba(chart->cairo_info.colors_for_pie[i], 2);
            b = trans_rgba(chart->cairo_info.colors_for_pie[i], 3);
            a = trans_rgba(chart->cairo_info.colors_for_pie[i], 4);
            if(i < node_count/2)
            {
                cairo_move_to(legend_cr, 1.0*(w-50*(node_count/2))/2+50*i, \
                                origin_y+pie_r+font_size*2);
                cairo_get_current_point(legend_cr, &current_x, &current_y);
                cairo_arc(legend_cr, current_x, current_y+4, 4, 0, 2*pi);
                cairo_set_source_rgba(legend_cr, r, g, b, a);
                cairo_fill(legend_cr);
                chart->pie_legend_dc[i] = DcCreate(DC_INPUT_WINDOW, chart->vc, (int)current_x+6, (int)current_y-6, 40, 20, (TWidget *)obj);
                DcSetTextAline(chart->pie_legend_dc[i], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
                DcSetText(chart->pie_legend_dc[i], TTableGetString(str_table, TStringID("str"), NULL), -1, 0);

            }
            else
            {
                cairo_move_to(legend_cr, 1.0*(w-50*(node_count-node_count/2))/2+50*(i-node_count/2), \
                                origin_y+pie_r+font_size*2+25);
                cairo_get_current_point(legend_cr, &current_x, &current_y);
                cairo_arc(legend_cr, current_x, current_y+4, 4, 0, 2*pi);
                cairo_set_source_rgba(legend_cr, r, g, b, a);
                cairo_fill(legend_cr);
                chart->pie_legend_dc[i] = DcCreate(DC_INPUT_WINDOW, chart->vc, (int)current_x+6, (int)current_y-6, 40, 20, (TWidget *)obj);
                DcSetTextAline(chart->pie_legend_dc[i], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
                DcSetText(chart->pie_legend_dc[i], TTableGetString(str_table, TStringID("str"), NULL), -1, 0);

            }

            temp_node = TwDMGetSiblingNode(temp_node, TRUE);
            temp_table =  TwDMGetNodeValue(temp_node, FALSE);
        }
    }
    else if(strcmp(show_type, "radar") == 0 && chart->axis_type == TYPE_RADAR)
    {
        cairo_move_to(legend_cr, w-50, 10.0+chart->cairo_info.legend_num * 20.0);
        cairo_get_current_point(legend_cr, &temp_x, &temp_y);
        int if_fill;
        if_fill = TTableGetInt(node_info_table, TStringID("if_fill"));
        
        if(if_fill)
        {
            cairo_rectangle(legend_cr, temp_x, temp_y, 15, 8);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_fill(legend_cr);
           
        }
        else 
        {
            cairo_move_to(legend_cr, temp_x, temp_y+5);
            cairo_line_to(legend_cr, temp_x+15, temp_y+5);
            cairo_set_line_width(legend_cr, 2);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_stroke(legend_cr);
        }
       
        chart->legend_dc[chart->cairo_info.legend_num] = DcCreate(DC_INPUT_WINDOW, chart->vc, temp_x+18, temp_y-6, 30, 20, (TWidget *)obj);
        // DcShowHide(chart->legend_dc[chart->cairo_info.legend_num], FALSE);
        DcSetTextAline(chart->legend_dc[chart->cairo_info.legend_num], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
        DcSetText(chart->legend_dc[chart->cairo_info.legend_num], legend_text, -1, 0);
        chart->cairo_info.legend_num++;
        
    }
}


/**
 *函数名:_chartShowData 
 *参数:obj:对象；
        node：数据节点
        info_table：控件参数表
 *返回值：无
 *作用：绘制数据相关流程
 **/
void _chartShowData(TWidget *obj, TwDataNode *show_root, TTable *info_table)
{
	TD_CHART *chart= (TD_CHART*)obj;
    if(!chart->axis_ok)
    {
        TLogE("Show data error! No correct axis!\n");
    }
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *his_cr = chart->his_cr;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    cairo_t *axis_cr= chart->axis_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    TwDataNode *node = TwDMGetChildNode(show_root, TRUE);
    if(node == NULL)
        return;
    TTable *value = TwDMGetNodeValue(node, FALSE);
    if(value == NULL)
        return;
    //第一步按照图的显示类型绘制数据
    if(chart->axis_type == TYPE_COORDINATES)
    {
        T_ID x_type;
        x_type = TTableGetType(value, TStringID("x"));
        if(x_type == 0)
            return;
        chart->root_nodes[chart->data_groups++] = show_root;
        if(x_type == T_VALUE_INT )
            showDataInt(obj, show_root, info_table);
        else if(x_type == T_VALUE_FLOAT)
            showDataDouble(obj, show_root, info_table);
        else if(x_type == T_VALUE_STRING)
            showDataString(obj, show_root, info_table);
    }
    else if(chart->axis_type == TYPE_PIE)
    {
        chart->root_nodes[chart->data_groups++] = show_root;
        showDataPie(obj, show_root, info_table);
    }
    else if(chart->axis_type == TYPE_RADAR)
    {
        chart->root_nodes[chart->data_groups++] = show_root;
        showDataRadar(obj, show_root, info_table);
    }

    //绘制完数据以后，根据是否显示图例参数来绘制图例
    //首先获取节点类型以及该类型节点的参数
    TTable *node_info_table;
    T_ID node_type;
    //是否显示图例
    int if_show_legend;
    //是否显示数据标注
    int if_show_tag;
    node_type = TwDMGetNodeType(TwDMGetChildNode(show_root, TRUE));
    node_info_table = TTableGetTable(info_table, node_type);
    if_show_legend= TTableGetInt(node_info_table, TStringID("if_show_legend"));
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
    createLegend(obj, show_root, info_table);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
   
    if(if_show_tag == 1)
    {
        merge_surface(cr, his_surface, CAIRO_OPERATOR_OVER);
        merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
        merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
        merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
        merge_surface(cr, his_tag_surface, CAIRO_OPERATOR_OVER);
    }
    else
    {
        merge_surface(cr, his_surface, CAIRO_OPERATOR_OVER);
        merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
        merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
        
    }
    chart->cairo_info.if_show_tag = if_show_tag;
    chart->cairo_info.if_show_legend = if_show_legend;
    // cairo_surface_write_to_png(tag_surface, "pp.png");
    chart_update(dest_vc, surface, cr, w, h);
}

