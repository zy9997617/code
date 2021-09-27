#include "main.h"
#include "tdChart.h"
#include "struct.h"
#include "tdAxis.h"
#include "showData.h"
#include "tools.h"



extern void showDataPie(TWidget *obj, TwDataNode *node, TTable *info_table);


TTable *get_node_info_table(void *obj, T_ID show_root_type)
{
    TD_CHART *chart= (TD_CHART*)obj;

    TwDataNode *show_root = chart->root_nodes[0];
    if(show_root == NULL)
        return;
    T_ID dm_type = TwDMGetRootType(TwDMGetNodeDM(show_root));
    //***************第一步，首先解析配置文件*************************************//
	//获得config.ini
	TTable *config_table = TLoadIniFile(NULL, TGetExecutePath("../etc/config.ini"), NULL);
	if(config_table == NULL)
		return;
	//获得config.ini中的子表list_layout
	TTable *list_layout = TTableGetTable(config_table, TStringID("list_layout"));
	if(list_layout == NULL) 
		return;
	//去每个文件中寻找show_root_type对应的dm
	int list_layout_count = TTableGetCount(list_layout);
	char file_path[256] = {0};
	TTable *file_table, *info_table;
	for(int i = 0; i < list_layout_count; i++)
	{
		char *file_name = TTableGetIndexString(list_layout, i, NULL);
		sprintf(file_path, "%s/%s", TGetExecutePath("../etc"), file_name);
		file_table = TLoadIniFile(NULL, file_path, NULL);
		if(TTableGetTable(file_table, dm_type) != NULL)
		{
			info_table = TTableGetTable(file_table, dm_type);
			break;
		}
	}
	if(info_table == NULL)
		return;
    TTable *node_info_table;
    if(TTableGetTable(info_table, show_root_type) != NULL)
    {
        node_info_table = TTableGetTable(info_table, show_root_type);
    }
    return node_info_table;
}


void changenodetype(void *obj, T_ID show_root_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TwDataModel *dm;
    int node_counts;
    node_counts = TwDMGetChildNodeCount(chart->root_nodes[0]);
    for(int i = 0; i < chart->data_groups; i++)
    {
        dm = TwDMGetNodeDM(chart->root_nodes[i]);
        TwDMLock(dm);
        TTable *tables[node_counts];
        TwDataNode *last_node = TwDMGetChildNode(chart->root_nodes[i], TRUE);
        for(int j = 0; j < node_counts; j++)
        {
            tables[j] = TTableCreate();
            tables[j] = TTableCopy(TwDMGetNodeValue(last_node, FALSE), tables[j]);
            last_node = TwDMGetSiblingNode(last_node, TRUE);
        }
        TwDMDeleteNodeAllChild(chart->root_nodes[i]);
        for(int j = 0; j < node_counts; j++)
        {
            TwDataNode *new_node = TwDMAddNode(chart->root_nodes[i], show_root_type);
            TwDMSetNodeValue(new_node, tables[j]);
        }
        TwDMUnlock(dm);
    }
}

void ChartChangeFromRadar2PieChangeLegend(void *obj, T_ID show_root_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    cairo_t *his_cr = chart->his_cr;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    //清空之前的surface
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_set_operator(data_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(data_cr);

    cairo_set_operator(tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(tag_cr);

    cairo_set_operator(legend_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(legend_cr);

    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_tag_cr);

    cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(data_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(legend_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);

    changenodetype(obj, show_root_type);

    TwDataNode *temp_node;
    TTable *temp_table;
    temp_node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
    temp_table = TwDMGetNodeValue(temp_node, FALSE);
    char temp_str[32] = {0};

    TTable *node_info_table;
    T_ID node_type;
    int if_show_legend;
    char *legend_text;
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

    node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);
    for(int i = 0; i < chart->data_groups; i++)
    {
        DcShowHide(chart->legend_dc[i], FALSE);
    }
    
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
            if(chart->pie_legend_dc[i])
                DcShowHide(chart->pie_legend_dc[i], TRUE);
            else
            {
                chart->pie_legend_dc[i] = DcCreate(DC_INPUT_WINDOW, chart->vc, (int)current_x+6, (int)current_y-6, 40, 20, (TWidget *)obj);
                DcSetTextAline(chart->pie_legend_dc[i], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
                DcSetText(chart->pie_legend_dc[i], TTableGetString(str_table, TStringID("str"), NULL), -1, 0);
            }
        }
        else
        {
            cairo_move_to(legend_cr, 1.0*(w-50*(node_count-node_count/2))/2+50*(i-node_count/2), \
                            origin_y+pie_r+font_size*2+25);
            cairo_get_current_point(legend_cr, &current_x, &current_y);
            cairo_arc(legend_cr, current_x, current_y+4, 4, 0, 2*pi);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_fill(legend_cr);
            if(chart->pie_legend_dc[i])
                DcShowHide(chart->pie_legend_dc[i], TRUE);
            else
            {
                chart->pie_legend_dc[i] = DcCreate(DC_INPUT_WINDOW, chart->vc, (int)current_x+6, (int)current_y-6, 40, 20, (TWidget *)obj);
                DcSetTextAline(chart->pie_legend_dc[i], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
                DcSetText(chart->pie_legend_dc[i], TTableGetString(str_table, TStringID("str"), NULL), -1, 0);
            }
        }
        temp_node = TwDMGetSiblingNode(temp_node, TRUE);
        temp_table =  TwDMGetNodeValue(temp_node, FALSE);
    }
}

void ChartChangeFromPie2RadarChangeLegend(void *obj, T_ID show_root_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    cairo_t *his_cr = chart->his_cr;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    //清空之前的surface
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_set_operator(data_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(data_cr);

    cairo_set_operator(tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(tag_cr);

    cairo_set_operator(legend_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(legend_cr);

    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_tag_cr);

    cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(data_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(legend_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);

    changenodetype(obj, show_root_type);

    int node_count;
    node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);
    for(int i= 0; i < node_count; i++)
    {
        DcShowHide(chart->pie_legend_dc[i], FALSE);
    }
    //绘制雷达图图例
    double temp_x, temp_y;
    TTable *node_info_table;
    char *rgba;
    double r, g, b, a;
    char *legend_text;
    node_info_table = get_node_info_table(obj, show_root_type);
    cairo_move_to(legend_cr, w-50, 10.0+chart->cairo_info.legend_num * 20.0);
    cairo_get_current_point(legend_cr, &temp_x, &temp_y);
    int if_fill;
    if_fill = TTableGetInt(node_info_table, TStringID("if_fill"));
    for(int i = 0; i < chart->data_groups; i++)
    {
        cairo_move_to(legend_cr, w-50, 10.0+i * 20.0);
        cairo_get_current_point(legend_cr, &temp_x, &temp_y);
        if(chart->data_groups == 1 && TTableGetString(TTableGetTable(node_info_table, TStringID("style")),\
                         TStringID("rgba"), NULL))
        {
            rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);

        }
        else
        {
            rgba = chart->cairo_info.colors_for_pie[i];
        }
        if_fill = TTableGetInt(node_info_table, TStringID("if_fill"));
        r = trans_rgba(rgba, 1);
        g = trans_rgba(rgba, 2);
        b = trans_rgba(rgba, 3);
        a = trans_rgba(rgba, 4);
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
        if(!chart->legend_dc[i])
        {
            chart->legend_dc[i] = DcCreate(DC_INPUT_WINDOW, chart->vc, temp_x+18, temp_y-6, 30, 20, (TWidget *)obj);
            // DcShowHide(chart->legend_dc[chart->cairo_info.legend_num], FALSE);
            DcSetTextAline(chart->legend_dc[i], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
            legend_text = TTableGetString(TwDMGetNodeValue(chart->root_nodes[i], FALSE), TStringID("legend_text"), NULL);
            DcSetText(chart->legend_dc[i], legend_text, -1, 0);
        }
       
        
    }
    chart->cairo_info.legend_num = chart->data_groups;
}

void ChartChangeFromPie2CoorChangeLegend(void *obj, T_ID show_root_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    cairo_t *his_cr = chart->his_cr;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    //清空之前的surface
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_set_operator(data_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(data_cr);

    cairo_set_operator(tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(tag_cr);

    cairo_set_operator(legend_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(legend_cr);

    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_tag_cr);

    cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(data_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(legend_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);

    changenodetype(obj, show_root_type);
    int node_count;
    node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);
    for(int i= 0; i < node_count; i++)
    {
        DcShowHide(chart->pie_legend_dc[i], FALSE);
    }
    //绘制直角坐标的图例
    TTable *node_info_table;
    T_ID node_type;
    int if_show_legend;
    char *legend_text;
    char *show_type;
    char *rgba;
    double r, g, b, a;
    int font_size;
    double origin_x, origin_y;
    double pie_r;
    double temp_x, temp_y;

    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    pie_r = chart->cairo_info.pie_r;
    font_size = chart->cairo_info.scale_font_size;

    node_info_table = get_node_info_table(obj, show_root_type);
    show_type = TTableGetString(node_info_table, TStringID("show_type"), NULL);
    for(int i = 0; i < chart->data_groups; i++)
    {
        if(chart->data_groups == 1 && TTableGetString(TTableGetTable(node_info_table, TStringID("style")),\
                            TStringID("rgba"), NULL))
        {
            rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);

        }
        else
        {
            rgba = chart->cairo_info.colors_for_pie[i];
        }
        r = trans_rgba(rgba, 1);
        g = trans_rgba(rgba, 2);
        b = trans_rgba(rgba, 3);
        a = trans_rgba(rgba, 4);
        legend_text = TTableGetString(TwDMGetNodeValue(chart->root_nodes[i], FALSE), TStringID("legend_text"), NULL);
        if(strcmp(show_type, "histogram") == 0)
        {
            cairo_move_to(legend_cr, w-50.0, 5+chart->cairo_info.legend_num * 20.0);
            cairo_get_current_point(legend_cr, &temp_x, &temp_y);
            cairo_rectangle(legend_cr, temp_x, temp_y, 15, 8);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_fill(legend_cr);

            chart->legend_dc[chart->cairo_info.legend_num] = DcCreate(DC_INPUT_WINDOW, chart->vc, temp_x+16, temp_y-6, 30, 20, (TWidget *)obj);
            // DcShowHide(chart->legend_dc[chart->cairo_info.legend_num], FALSE);
            DcSetTextAline(chart->legend_dc[chart->cairo_info.legend_num], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
            DcSetText(chart->legend_dc[chart->cairo_info.legend_num], legend_text, -1, 0);
            chart->cairo_info.legend_num++;
        }
        else if(strcmp(show_type, "line") == 0)
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
        else if(strcmp(show_type, "point") == 0)
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
    }
}

void ChartChangeFromRadar2CoorChangeLegend(void *obj, T_ID show_root_type)
{
	TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    cairo_t *his_cr = chart->his_cr;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;
    //清空之前的surface
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_set_operator(data_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(data_cr);

    cairo_set_operator(tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(tag_cr);

    cairo_set_operator(legend_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(legend_cr);

    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_tag_cr);

    cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(data_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(legend_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);
    
    //重新绘制图例
    double temp_x, temp_y;
    char *legend_text;

    TTable *node_info_table;
    node_info_table = get_node_info_table(obj, show_root_type);

    char *rgba;
    double r, g, b, a;
    char *show_type;
    show_type = TTableGetString(node_info_table, TStringID("show_type"), NULL);
    for(int i = 0; i < chart->data_groups; i++)
    {
        if(chart->data_groups == 1 && TTableGetString(TTableGetTable(node_info_table, TStringID("style")),\
                         TStringID("rgba"), NULL))
        {
            rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);

        }
        else
        {
            rgba = chart->cairo_info.colors_for_pie[i];
        }
        r = trans_rgba(rgba, 1);
        g = trans_rgba(rgba, 2);
        b = trans_rgba(rgba, 3);
        a = trans_rgba(rgba, 4);
        if(strcmp(show_type, "histogram") == 0)
        {
            cairo_move_to(legend_cr, w-50.0, 5+i*20.0);
            cairo_get_current_point(legend_cr, &temp_x, &temp_y);
            cairo_rectangle(legend_cr, temp_x, temp_y, 15, 8);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_fill(legend_cr);
        }
        else if(strcmp(show_type, "line") == 0)
        {
            cairo_move_to(legend_cr, w-50.0, 10+chart->cairo_info.legend_num * 20.0);
            cairo_get_current_point(legend_cr, &temp_x, &temp_y);
            cairo_line_to(legend_cr, temp_x+15, temp_y);
            cairo_set_line_width(legend_cr, 2);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_stroke(legend_cr);

        }
        else if(strcmp(show_type, "point") == 0)
        {
            cairo_move_to(legend_cr, w-50.0, 10+chart->cairo_info.legend_num * 20.0);
            cairo_get_current_point(legend_cr, &temp_x, &temp_y);
            cairo_move_to(legend_cr, temp_x+7, temp_y+2);
            cairo_arc(legend_cr, temp_x+9, temp_y+2, 2, 0, 2*pi);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_fill(legend_cr);
        }
        DcShowHide(chart->legend_dc[i], TRUE);
        char *text = TTableGetString(TwDMGetNodeValue(chart->root_nodes[i], FALSE), TStringID("legend_text"), NULL);
        DcSetText(chart->legend_dc[i], text, -1, 0);
    }


}

void ChartChangeFromCoor2RadarChangeLegend(void *obj, T_ID show_root_type)
{
	TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_surface_t *his_surface = chart->his_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    cairo_t *his_cr = chart->his_cr;
    cairo_t *his_tag_cr = chart->his_tag_cr;
    int w = chart->surface_w;
    int h = chart->surface_h;

    //清空之前的surface
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_set_operator(data_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(data_cr);

    cairo_set_operator(tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(tag_cr);

    cairo_set_operator(legend_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(legend_cr);

    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_tag_cr);

    cairo_set_operator(his_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(his_cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(data_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(legend_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_operator(his_tag_cr, CAIRO_OPERATOR_SOURCE);
    //重新绘制图例
    double temp_x, temp_y;
    char *legend_text;
    TTable *node_info_table;
    node_info_table = get_node_info_table(obj, show_root_type);
    char *rgba;
    double r, g, b, a;
    int if_fill;
    for(int i = 0; i < chart->data_groups; i++)
    {
        cairo_move_to(legend_cr, w-50, 10.0+i * 20.0);
        cairo_get_current_point(legend_cr, &temp_x, &temp_y);
        if(chart->data_groups == 1 && TTableGetString(TTableGetTable(node_info_table, TStringID("style")),\
                         TStringID("rgba"), NULL))
        {
            rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);

        }
        else
        {
            rgba = chart->cairo_info.colors_for_pie[i];
        }
        if_fill = TTableGetInt(node_info_table, TStringID("if_fill"));
        r = trans_rgba(rgba, 1);
        g = trans_rgba(rgba, 2);
        b = trans_rgba(rgba, 3);
        a = trans_rgba(rgba, 4);
        if(!if_fill)
        {
            cairo_move_to(legend_cr, temp_x, temp_y+5);
            cairo_line_to(legend_cr, temp_x+15, temp_y+5);
            cairo_set_line_width(legend_cr, 2);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_stroke(legend_cr);
        }
        else
        {
            cairo_move_to(legend_cr, w-50, 10.0+chart->cairo_info.legend_num * 20.0);
            cairo_get_current_point(legend_cr, &temp_x, &temp_y);
            cairo_rectangle(legend_cr, temp_x, temp_y, 15, 8);
            cairo_set_source_rgba(legend_cr, r, g, b, a);
            cairo_fill(legend_cr);
        }
        DcShowHide(chart->legend_dc[i], TRUE);
    }
}


void ChartChangeFromRadar2CoorChangeScale(void *obj)
{
	TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;
    int w = chart->surface_w;
    int h = chart->surface_h;

	TwDataNode *root_node = chart->root_nodes[0];
    TwDataNode *node = TwDMGetChildNode(root_node, TRUE);
	TTable *value_table;
	int node_count;
	int data_type_x;
	int font_size;
	font_size = chart->cairo_info.scale_font_size;
	node_count = TwDMGetChildNodeCount(root_node);
	value_table = TwDMGetNodeValue(node, FALSE);
	data_type_x = TTableGetType(value_table, TStringID("x"));

    char value_scale[32] = {0};
	int value_scale_len;
    DcShowHide(chart->dc_x[0], FALSE);
    double pos_x, pos_y;
	for(int i = 1; i <= node_count; i++)
	{
        DcShowHide(chart->dc_x[i], FALSE);
        pos_x = origin_x + chart->cairo_info.x_unit*i;
        pos_y = origin_y;
        DcMove(chart->dc_x[i], pos_x, pos_y);
	}
    for(int i = 0; i < 16; i++)
    {
        if(chart->dc_y[i] != NULL)
            DcShowHide(chart->dc_y[i], FALSE);
    }
    
}


void ChartChangeFromPie2RadarChangeScale(void *obj)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;
    int w = chart->surface_w;
    int h = chart->surface_h;

	TwDataNode *root_node = chart->root_nodes[0];
    TwDataNode *node = TwDMGetChildNode(root_node, TRUE);
	TTable *value_table;
	int node_count;
	int data_type_x;
	int font_size;
	font_size = chart->cairo_info.scale_font_size;
	node_count = TwDMGetChildNodeCount(root_node);
	value_table = TwDMGetNodeValue(node, FALSE);
	data_type_x = TTableGetType(value_table, TStringID("x"));

    char value_scale[32] = {0};
	int value_scale_len;
	for(int i = 1; i <= node_count; i++)
	{
        if(data_type_x == T_VALUE_INT)
        {
            sprintf(value_scale, "%d", TTableGetInt(value_table, TStringID("x")));
            // my_itoa(TTableGetInt(value_table, TStringID("x")), value_scale);
            value_scale_len = strlen(value_scale);
        }
        else if(data_type_x == T_VALUE_FLOAT)
        {
            sprintf(value_scale, "%.2lf", TTableGetFloat(value_table, TStringID("x")));
            value_scale_len = strlen(value_scale);
        }
        else if(data_type_x == T_VALUE_STRING)
        {
            char *temp_str = TTableGetString(value_table, TStringID("x"), NULL);
            int zh_num = 0, ch_num = 0;
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
            sprintf(value_scale, "%s", temp_str);
            value_scale_len = zh_num + ch_num;
        }
        TTable *temp_table = TTableCreate();
        TTableAddString(temp_table, TStringID("str"), value_scale, -1);
        int pos_x, pos_y;
        if(cos(pi*i*2/node_count) >= 0)
        {
            pos_y = origin_y - (pie_r + font_size+7) * cos(pi*i*2/node_count);
        }
        else
        {
            pos_y = origin_y - (pie_r + 3) * cos(pi*i*2/node_count);
        }
        if(sin(pi*i*2/node_count) > 0 && (node_count == 6 && i!=3))
        {
            pos_x = origin_x + (pie_r + font_size*value_scale_len) * sin(pi*i*2/node_count);
        }
        else
        {
            pos_x = origin_x + (pie_r + font_size*value_scale_len+10) * sin(pi*i*2/node_count) - (font_size*value_scale_len+10)/2;
        }
        if(chart->dc_x[i])
            DcMove(chart->dc_x[i], pos_x, pos_y);
        else 
        {
            chart->dc_x[i-1] = DcCreate(DC_INPUT_WINDOW, chart->vc, pos_x, pos_y, \
                        value_scale_len*font_size, font_size+5, (TWidget *)obj);
            DcSetTextAline(chart->dc_x[i-1], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
            DcSetText(chart->dc_x[i-1], TTableGetString(temp_table, TStringID("str"), NULL), -1, 0);
            // DcSetText(chart->dc_x[i-1], "a", -1, 0);
        }
        node = TwDMGetSiblingNode(node, TRUE); 
        value_table = TwDMGetNodeValue(node, FALSE);
	}
    chart->x_dc_num = node_count;
}

void ChartChangeFromCoor2RadarChangeScale(void *obj)
{
	TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;
    int w = chart->surface_w;
    int h = chart->surface_h;

	TwDataNode *root_node = chart->root_nodes[0];
    TwDataNode *node = TwDMGetChildNode(root_node, TRUE);
	TTable *value_table;
	int node_count;
	int data_type_x;
	int font_size;
	font_size = chart->cairo_info.scale_font_size;
	node_count = TwDMGetChildNodeCount(root_node);
	value_table = TwDMGetNodeValue(node, FALSE);
	data_type_x = TTableGetType(value_table, TStringID("x"));

    char value_scale[32] = {0};
	int value_scale_len;
	for(int i = 1; i <= node_count; i++)
	{
        
        if(data_type_x == T_VALUE_INT)
        {
            sprintf(value_scale, "%d", TTableGetInt(value_table, TStringID("x")));
            // my_itoa(TTableGetInt(value_table, TStringID("x")), value_scale);
            value_scale_len = strlen(value_scale);
        }
        else if(data_type_x == T_VALUE_FLOAT)
        {
            sprintf(value_scale, "%.2lf", TTableGetFloat(value_table, TStringID("x")));
            value_scale_len = strlen(value_scale);
        }
        else if(data_type_x == T_VALUE_STRING)
        {
            char *temp_str = TTableGetString(value_table, TStringID("x"), NULL);
            int zh_num = 0, ch_num = 0;
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
            sprintf(value_scale, "%s", temp_str);
            value_scale_len = zh_num + ch_num;
        }
        int pos_x, pos_y;
        if(cos(pi*i*2/node_count) >= 0)
        {
            pos_y = origin_y - (pie_r + font_size+7) * cos(pi*i*2/node_count);
        }
        else
        {
            pos_y = origin_y - (pie_r + 3) * cos(pi*i*2/node_count);
        }
        if(sin(pi*i*2/node_count) > 0 && (node_count == 6 && i!=3))
        {
            pos_x = origin_x + (pie_r + font_size*value_scale_len) * sin(pi*i*2/node_count);
        }
        else
        {
            pos_x = origin_x + (pie_r + font_size*value_scale_len+10) * sin(pi*i*2/node_count) - (font_size*value_scale_len+10)/2;
        }
        DcMove(chart->dc_x[i], pos_x, pos_y);
	}
    
}
/*
void ChartChangeFromPie2CoorChangeScale(void *obj)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;
    int w = chart->surface_w;
    int h = chart->surface_h;

    TwDataNode *root_node = chart->root_nodes[0];
    TwDataNode *node = TwDMGetChildNode(root_node, TRUE);
	TTable *value_table;
	int node_count;
	int data_type_x;
	int font_size;
	font_size = chart->cairo_info.scale_font_size;
	node_count = TwDMGetChildNodeCount(root_node);
	value_table = TwDMGetNodeValue(node, FALSE);
	data_type_x = TTableGetType(value_table, TStringID("x"));

    char value_scale[32] = {0};
	int value_scale_len;
	for(int i = 1; i <= node_count; i++)
	{
        
        if(data_type_x == T_VALUE_INT)
        {
            sprintf(value_scale, "%d", TTableGetInt(value_table, TStringID("x")));
            // my_itoa(TTableGetInt(value_table, TStringID("x")), value_scale);
            value_scale_len = strlen(value_scale);
        }
        else if(data_type_x == T_VALUE_FLOAT)
        {
            sprintf(value_scale, "%.2lf", TTableGetFloat(value_table, TStringID("x")));
            value_scale_len = strlen(value_scale);
        }
        else if(data_type_x == T_VALUE_STRING)
        {
            char *temp_str = TTableGetString(value_table, TStringID("x"), NULL);
            int zh_num = 0, ch_num = 0;
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
            sprintf(value_scale, "%s", temp_str);
            value_scale_len = zh_num + ch_num;
        }
        int pos_x, pos_y;
        
	}
}*/


void _ChartChangeFromPie2Radar(void *obj, TwDataNode *show_root, T_ID show_node_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;

    T_ID dm_type = TwDMGetRootType(TwDMGetNodeDM(show_root));
    //***************第一步，首先解析配置文件*************************************//
	//获得config.ini
	TTable *config_table = TLoadIniFile(NULL, TGetExecutePath("../etc/config.ini"), NULL);
	if(config_table == NULL)
		return;
	//获得config.ini中的子表list_layout
	TTable *list_layout = TTableGetTable(config_table, TStringID("list_layout"));
	if(list_layout == NULL) 
		return;
	//去每个文件中寻找show_root_type对应的dm
	int list_layout_count = TTableGetCount(list_layout);
	char file_path[256] = {0};
	TTable *file_table, *info_table;
	for(int i = 0; i < list_layout_count; i++)
	{
		char *file_name = TTableGetIndexString(list_layout, i, NULL);
		sprintf(file_path, "%s/%s", TGetExecutePath("../etc"), file_name);
		file_table = TLoadIniFile(NULL, file_path, NULL);
		if(TTableGetTable(file_table, dm_type) != NULL)
		{
			info_table = TTableGetTable(file_table, dm_type);
			break;
		}
	}
	if(info_table == NULL)
		return;
    TTable *node_info_table;
    if(TTableGetTable(info_table, show_node_type) != NULL)
    {
        node_info_table = TTableGetTable(info_table, show_node_type);
    }
    int now_num = get_rank_of_rootnode((void *)chart, show_root);
    // printf("num is :%d\n", now_num);
    //**********************************************************************//

    double current_x, current_y;
    TwDataNode *datanode = TwDMGetChildNode(show_root, TRUE);
    int node_count = TwDMGetChildNodeCount(show_root);
    if(datanode == NULL)
        return;
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *value_table = TwDMGetNodeValue(datanode, FALSE);//实际数据表

    
    double max, min, distance;
    double r, g, b, a;
    int if_fill;
    int if_show_tag;
    int level = 5;
    max = TTableGetFloat(node_info_table, TStringID("max"));
    min = TTableGetFloat(node_info_table, TStringID("min"));
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
  
    if(TTableGetInt(node_info_table, TStringID("level")) != 0)
    {
        level = TTableGetInt(node_info_table, TStringID("level"));
    }
    distance = (max - min)/level;

    if(now_num == 0)
    {

        //根据节点个数，绘制对应的雷达图网格
        cairo_move_to(axis_cr, origin_x, origin_y);//先移动到原点
        cairo_set_source_rgba(axis_cr, 0, 0, 0, 1);
        cairo_set_line_width(axis_cr, 1);
        if(node_count == 1)
        {
            for(int i = 0; i < level-1; i++)
            {
                cairo_move_to(axis_cr, origin_x+pie_r/level*(i+1), origin_y);
                cairo_arc(axis_cr, origin_x, origin_y, pie_r/level * (i+1), 0, 2*pi);
                cairo_stroke(axis_cr);
            }
        }
        else if(node_count == 2)
        {
            for(int i = 0; i < level-1; i++)
            {
                cairo_move_to(axis_cr, origin_x+pie_r/level*(i+1), origin_y);
                cairo_arc(axis_cr, origin_x, origin_y, pie_r/level * (i+1), 0, 2*pi);
                cairo_stroke(axis_cr);
            }
            cairo_move_to(axis_cr, origin_x - pie_r, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r, origin_y);
            cairo_stroke(axis_cr);
        }
        else if(node_count == 3)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y-pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x, origin_y-pie_r/level*(i+1));
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y - pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_stroke(axis_cr);
        }
        else if(node_count == 4)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x+pie_r/level*(i+1), origin_y);
                cairo_line_to(axis_cr, origin_x, origin_y + pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x-pie_r/level*(i+1), origin_y);
                cairo_line_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y-pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r, origin_y);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y+pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r, origin_y);
            cairo_stroke(axis_cr);
        }
        else if(node_count == 5)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/10), origin_y-pie_r/level*(i+1)*sin(pi/10));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*sin(pi/5), origin_y+pie_r/level*(i+1)*cos(pi/5));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*sin(pi/5), origin_y+pie_r/level*(i+1)*cos(pi/5));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/10), origin_y-pie_r/level*(i+1)*sin(pi/10));
                cairo_line_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y-pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/10), origin_y - pie_r*sin(pi/10));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*sin(pi/5), origin_y + pie_r*cos(pi/5));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*sin(pi/5), origin_y + pie_r*cos(pi/5));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/10), origin_y - pie_r*sin(pi/10));
            cairo_stroke(axis_cr);
        }
        else if(node_count == 6)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y - pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x, origin_y + pie_r/level*(i+1));    
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y - pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));    
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y - pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/6), origin_y - pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y + pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/6), origin_y - pie_r*sin(pi/6));

            cairo_stroke(axis_cr);
        }
    }
    double length = 0;
    double first_x, first_y;
    cairo_move_to(axis_cr, origin_x, origin_y);
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
    char *rgba;
    if(chart->data_groups == 1 && TTableGetString(TTableGetTable(node_info_table, TStringID("style")),\
                         TStringID("rgba"), NULL))
    {
        rgba = TTableGetString(TTableGetTable(node_info_table, TStringID("style")), TStringID("rgba"), NULL);

    }
    else
    {
        rgba = chart->cairo_info.colors_for_pie[now_num];
    }
    r = trans_rgba(rgba, 1);
    g = trans_rgba(rgba, 2);
    b = trans_rgba(rgba, 3);
    a = trans_rgba(rgba, 4);
    
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
                    
                    cairo_move_to(tag_cr, origin_x + delt_x-value_len/2.0, origin_y - delt_y-1*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    
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
                        
                        cairo_rotate(tag_cr, i*2*pi/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_rotate(tag_cr, -i*2*pi/5);
                        
                    }
                    if(i == 4)
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
                else if(i == 2)
                {
                    double delt_x, delt_y;
                    delt_x = length*sin(pi/5);
                    delt_y = length*cos(pi/5);
                    //数据注释
                    // if(if_show_tag)
                    {
                        cairo_rotate(tag_cr, pi/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_rotate(tag_cr, -pi/5);
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
                        cairo_rotate(tag_cr, pi*4/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_rotate(tag_cr, -pi*4/5);
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
                    cairo_rotate(tag_cr, i*pi/2);
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    cairo_rotate(tag_cr, -i*pi/2);
                    cairo_stroke(tag_cr);
                }
                if(i == 3)
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
                    cairo_rotate(tag_cr, i*2*pi/3);
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    cairo_rotate(tag_cr, -i*2*pi/3);
                    cairo_stroke(tag_cr);
                }
                if(i == 2)
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
        
        if(TwDMGetSiblingNode(datanode, TRUE))
        {
            datanode = TwDMGetSiblingNode(datanode, TRUE);
            value_table = TwDMGetNodeValue(datanode, FALSE);
        }
    }

    // int if_show_tag;
    int if_show_legend;
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
    if_show_legend = TTableGetInt(node_info_table, TStringID("if_show_legend"));
    int if_show_scale = TTableGetInt(node_info_table, TStringID("if_show_scale"));
    chart->cairo_info.if_show_tag = if_show_tag;
    chart->cairo_info.if_show_legend = if_show_legend;
    chart->cairo_info.if_show_grid = 0;
    chart->cairo_info.if_show_scale = if_show_scale;
}

void _ChartChangeFromRadar2Pie(void *obj, T_ID show_node_type)
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
    TwDataNode *datanode = TwDMGetChildNode(chart->root_nodes[0], TRUE);
    int node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);
    if(datanode == NULL)
        return;
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *nodetable = get_node_info_table(obj, node_type);//配置文件节点信息表
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

void _ChartChangeFromCoor2Pie(void *obj, T_ID show_node_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;


    T_ID dm_type = TwDMGetRootType(TwDMGetNodeDM(chart->root_nodes[0]));

    //第一步，首先解析配置文件
	//获得config.ini
	TTable *config_table = TLoadIniFile(NULL, TGetExecutePath("../etc/config.ini"), NULL);
	if(config_table == NULL)
		return;
	//获得config.ini中的子表list_layout
	TTable *list_layout = TTableGetTable(config_table, TStringID("list_layout"));
	if(list_layout == NULL) 
		return;
	//去每个文件中寻找show_root_type对应的dm
	int list_layout_count = TTableGetCount(list_layout);
	char file_path[256] = {0};
	TTable *file_table, *info_table;
	for(int i = 0; i < list_layout_count; i++)
	{
		char *file_name = TTableGetIndexString(list_layout, i, NULL);
		sprintf(file_path, "%s/%s", TGetExecutePath("../etc"), file_name);
		file_table = TLoadIniFile(NULL, file_path, NULL);
		if(TTableGetTable(file_table, dm_type) != NULL)
		{
			info_table = TTableGetTable(file_table, dm_type);
			break;
		}
	}
	if(info_table == NULL)
		return;
    TTable *node_info_table;
    if(TTableGetTable(info_table, show_node_type) != NULL)
    {
        node_info_table = TTableGetTable(info_table, show_node_type);
    }

    cairo_set_operator(data_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(data_cr, 1, 1, 1, 0);
    cairo_paint(data_cr);

    cairo_set_operator(tag_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(tag_cr, 1, 1, 1, 0);
    cairo_paint(tag_cr);

    cairo_set_operator(legend_cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(legend_cr, 1, 1, 1, 0);
    cairo_paint(legend_cr);
    for(int i = chart->cairo_info.legend_num-1; i >= 0; i--)
    {
        DcDestroy(chart->legend_dc[i]);
    }
    chart->cairo_info.legend_num = 0;
    //showDataPie(obj, root_nodes[0], info_table);
    //绘图操作
    double current_x, current_y;
    TwDataNode *datanode = TwDMGetChildNode(chart->root_nodes[0], TRUE);
    int node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);
    if(datanode == NULL)
        return;
    
    TTable *value_table = TwDMGetNodeValue(datanode, FALSE);//实际数据表
    T_ID data_type;
    data_type = TTableGetType(value_table, TStringID("y"));
    double percent = 0, value = 0, sum = 0, angle = 0;
    double last_x, last_y;
    last_x = origin_x+pie_r;
    last_y = origin_y;
    char *rgba;
    double r = 0, g = 0, b = 0, a = 0;

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

            cairo_move_to(data_cr, origin_x, origin_y);
            cairo_line_to(data_cr, last_x, last_y);
            cairo_arc(data_cr, origin_x, origin_y, pie_r, angle, angle+percent*2*pi);
            cairo_get_current_point(data_cr, &last_x, &last_y);
            cairo_line_to(data_cr, origin_x, origin_y);
            cairo_set_source_rgba(data_cr, r, g, b, a);

            //标注数据
            // if(if_show_tag)
            {
                if(percent >= 0.02)
                {
                    cairo_set_source_rgba(tag_cr, 0,0, 0, 1);
                    cairo_move_to(tag_cr, origin_x, origin_y);
                    cairo_move_to(tag_cr, pie_r*cos(angle+percent*pi)+origin_x,  \
                                    pie_r*sin(angle+percent*pi)+origin_y);
                    cairo_line_to(tag_cr, 1.1*pie_r*cos(angle+percent*pi)+origin_x,  \
                                    1.1*pie_r*sin(angle+percent*pi)+origin_y);
                    if(cos(angle+percent*pi) >= 0)
                    {
                        cairo_line_to(tag_cr, 1.1*pie_r*cos(angle+percent*pi)+origin_x + 1.0*value_len*font_size/4+10,  \
                                        1.1*pie_r*sin(angle+percent*pi)+origin_y);
                        
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x-1.0*value_len*font_size/4-5, current_y - font_size-2);
                        cairo_show_text(tag_cr, value_2_str);
                    }
                    else
                    {
                        cairo_line_to(tag_cr, 1.1*pie_r*cos(angle+percent*pi)+origin_x - 1.0*value_len*font_size/4-10,  \
                                        1.1*pie_r*sin(angle+percent*pi)+origin_y);
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x-1.0*value_len*font_size/4-5, current_y - font_size-2);
                        cairo_show_text(tag_cr, value_2_str);
                    }
                }
                
                cairo_stroke(tag_cr);
            }
            
            cairo_fill_preserve(data_cr);
            cairo_stroke(data_cr);

            angle+=percent*2*pi;

        }
        if(data_type == T_VALUE_FLOAT)
        {
            value = TTableGetFloat(value_table, TStringID("y"));
            percent = value/sum;
            sprintf(value_2_str, "%.2lf%c", percent*100, '%');
            value_len = strlen(value_2_str);

            cairo_move_to(data_cr, origin_x, origin_y);
            cairo_line_to(data_cr, last_x, last_y);
            cairo_arc(data_cr, origin_x, origin_y, pie_r, angle, angle+percent*2*pi);
            
            cairo_get_current_point(data_cr, &last_x, &last_y);
            cairo_line_to(data_cr, origin_x, origin_y);
            cairo_set_source_rgba(data_cr, r, g, b, a);

            //标注数据
            // if(if_show_tag)
            {
                if(percent >= 0.02)
                {
                    cairo_set_source_rgba(tag_cr, 0,0, 0, 1);
                    cairo_move_to(tag_cr, origin_x, origin_y);
                    cairo_move_to(tag_cr, pie_r*cos(angle+percent*pi)+origin_x,  \
                                    pie_r*sin(angle+percent*pi)+origin_y);
                    cairo_line_to(tag_cr, 1.1*pie_r*cos(angle+percent*pi)+origin_x,  \
                                    1.1*pie_r*sin(angle+percent*pi)+origin_y);
                    if(cos(angle+percent*pi) >= 0)
                    {
                        cairo_line_to(tag_cr, 1.1*pie_r*cos(angle+percent*pi)+origin_x + 1.0*value_len*font_size/4+10,  \
                                        1.1*pie_r*sin(angle+percent*pi)+origin_y);
                        
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x-1.0*value_len*font_size/4-5, current_y - font_size-2);
                        cairo_show_text(tag_cr, value_2_str);
                    }
                    else
                    {
                        cairo_line_to(tag_cr, 1.1*pie_r*cos(angle+percent*pi)+origin_x - 1.0*value_len*font_size/4-10,  \
                                        1.1*pie_r*sin(angle+percent*pi)+origin_y);
                        cairo_get_current_point(tag_cr, &current_x, &current_y);
                        cairo_move_to(tag_cr, current_x-1.0*value_len*font_size/4-5, current_y - font_size-2);
                        cairo_show_text(tag_cr, value_2_str);
                    }
                }
                cairo_stroke(tag_cr);
            }

            cairo_fill_preserve(data_cr);
            cairo_stroke(data_cr);
            angle+=percent*2*pi;
        }
        datanode = TwDMGetSiblingNode(datanode, TRUE);
        value_table = TwDMGetNodeValue(datanode, FALSE);

    }
    
    //重新绘制图例
    double temp_x, temp_y;
    char *legend_text;
    datanode = TwDMGetChildNode(chart->root_nodes[0], TRUE);
    value_table = TwDMGetNodeValue(datanode, FALSE);
    for(int i = 0; i < node_count; i++)
    {
        rgba = chart->cairo_info.colors_for_pie[i];
        r = trans_rgba(rgba, 1);
        g = trans_rgba(rgba, 2);
        b = trans_rgba(rgba, 3);
        a = trans_rgba(rgba, 4);

        cairo_move_to(legend_cr, w-50.0, chart->cairo_info.legend_num * 20.0+5);
        cairo_get_current_point(legend_cr, &temp_x, &temp_y);
        cairo_move_to(legend_cr, temp_x+7, temp_y+2);
        cairo_arc(legend_cr, temp_x+9, temp_y+3, 3, 0, 2*pi);
        cairo_set_source_rgba(legend_cr, r, g, b, a);
        cairo_fill(legend_cr);

        chart->legend_dc[chart->cairo_info.legend_num] = DcCreate(DC_INPUT_WINDOW, chart->vc, temp_x+18, temp_y-6, 30, 20, (TWidget *)obj);
        DcShowHide(chart->legend_dc[chart->cairo_info.legend_num], FALSE);
        DcSetTextAline(chart->legend_dc[chart->cairo_info.legend_num], DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
        legend_text = TTableGetString(value_table, TStringID("item"), NULL);
        DcSetText(chart->legend_dc[chart->cairo_info.legend_num], legend_text, -1, 0);
        chart->cairo_info.legend_num++;

        datanode = TwDMGetSiblingNode(datanode, TRUE);
        value_table = TwDMGetNodeValue(datanode, FALSE);
    }

    int if_show_tag;
    int if_show_legend;
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
    if_show_legend = TTableGetInt(node_info_table, TStringID("if_show_legend"));
    chart->cairo_info.if_show_tag = if_show_tag;
    chart->cairo_info.if_show_legend = if_show_legend;
    chart->cairo_info.if_show_grid = 0;
    chart->cairo_info.if_show_scale = 0;

}

void _ChartChangeFromCoor2Radar(void *obj, TwDataNode *show_root, T_ID show_node_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;

    T_ID dm_type = TwDMGetRootType(TwDMGetNodeDM(show_root));
    //***************第一步，首先解析配置文件*************************************//
	//获得config.ini
	TTable *config_table = TLoadIniFile(NULL, TGetExecutePath("../etc/config.ini"), NULL);
	if(config_table == NULL)
		return;
	//获得config.ini中的子表list_layout
	TTable *list_layout = TTableGetTable(config_table, TStringID("list_layout"));
	if(list_layout == NULL) 
		return;
	//去每个文件中寻找show_root_type对应的dm
	int list_layout_count = TTableGetCount(list_layout);
	char file_path[256] = {0};
	TTable *file_table, *info_table;
	for(int i = 0; i < list_layout_count; i++)
	{
		char *file_name = TTableGetIndexString(list_layout, i, NULL);
		sprintf(file_path, "%s/%s", TGetExecutePath("../etc"), file_name);
		file_table = TLoadIniFile(NULL, file_path, NULL);
		if(TTableGetTable(file_table, dm_type) != NULL)
		{
			info_table = TTableGetTable(file_table, dm_type);
			break;
		}
	}
	if(info_table == NULL)
		return;
    TTable *node_info_table;
    if(TTableGetTable(info_table, show_node_type) != NULL)
    {
        node_info_table = TTableGetTable(info_table, show_node_type);
    }
    int now_num = get_rank_of_rootnode((void *)chart, show_root);
    // printf("num is :%d\n", now_num);
    //**********************************************************************//

    double current_x, current_y;
    TwDataNode *datanode = TwDMGetChildNode(show_root, TRUE);
    int node_count = TwDMGetChildNodeCount(show_root);
    if(datanode == NULL)
        return;
    T_ID node_type = TwDMGetNodeType(datanode);
    TTable *value_table = TwDMGetNodeValue(datanode, FALSE);//实际数据表

    
    double max, min, distance;
    double r, g, b, a;
    int if_fill;
    int if_show_tag;
    int level = 5;
    max = TTableGetFloat(node_info_table, TStringID("max"));
    min = TTableGetFloat(node_info_table, TStringID("min"));
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
  
    if(TTableGetInt(node_info_table, TStringID("level")) != 0)
    {
        level = TTableGetInt(node_info_table, TStringID("level"));
    }
    distance = (max - min)/level;

    if(now_num == 0)
    {

        //根据节点个数，绘制对应的雷达图网格
        cairo_move_to(axis_cr, origin_x, origin_y);//先移动到原点
        cairo_set_source_rgba(axis_cr, 0, 0, 0, 1);
        cairo_set_line_width(axis_cr, 1);
        if(node_count == 1)
        {
            for(int i = 0; i < level-1; i++)
            {
                cairo_move_to(axis_cr, origin_x+pie_r/level*(i+1), origin_y);
                cairo_arc(axis_cr, origin_x, origin_y, pie_r/level * (i+1), 0, 2*pi);
                cairo_stroke(axis_cr);
            }
        }
        else if(node_count == 2)
        {
            for(int i = 0; i < level-1; i++)
            {
                cairo_move_to(axis_cr, origin_x+pie_r/level*(i+1), origin_y);
                cairo_arc(axis_cr, origin_x, origin_y, pie_r/level * (i+1), 0, 2*pi);
                cairo_stroke(axis_cr);
            }
            cairo_move_to(axis_cr, origin_x - pie_r, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r, origin_y);
            cairo_stroke(axis_cr);
        }
        else if(node_count == 3)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y-pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x, origin_y-pie_r/level*(i+1));
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y - pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_stroke(axis_cr);
        }
        else if(node_count == 4)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x+pie_r/level*(i+1), origin_y);
                cairo_line_to(axis_cr, origin_x, origin_y + pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x-pie_r/level*(i+1), origin_y);
                cairo_line_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y-pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r, origin_y);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y+pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r, origin_y);
            cairo_stroke(axis_cr);
        }
        else if(node_count == 5)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/10), origin_y-pie_r/level*(i+1)*sin(pi/10));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*sin(pi/5), origin_y+pie_r/level*(i+1)*cos(pi/5));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*sin(pi/5), origin_y+pie_r/level*(i+1)*cos(pi/5));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/10), origin_y-pie_r/level*(i+1)*sin(pi/10));
                cairo_line_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y-pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/10), origin_y - pie_r*sin(pi/10));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*sin(pi/5), origin_y + pie_r*cos(pi/5));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*sin(pi/5), origin_y + pie_r*cos(pi/5));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/10), origin_y - pie_r*sin(pi/10));
            cairo_stroke(axis_cr);
        }
        else if(node_count == 6)
        {
            for(int i = 0; i < level; i++)
            {
                cairo_move_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y - pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x + pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x, origin_y + pie_r/level*(i+1));    
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y + pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x - pie_r/level*(i+1)*cos(pi/6), origin_y - pie_r/level*(i+1)*sin(pi/6));
                cairo_line_to(axis_cr, origin_x, origin_y - pie_r/level*(i+1));    
            }
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y - pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/6), origin_y - pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x + pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x, origin_y + pie_r);
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/6), origin_y + pie_r*sin(pi/6));
            cairo_move_to(axis_cr, origin_x, origin_y);
            cairo_line_to(axis_cr, origin_x - pie_r*cos(pi/6), origin_y - pie_r*sin(pi/6));

            cairo_stroke(axis_cr);
        }
    }
    double length = 0;
    double first_x, first_y;
    cairo_move_to(axis_cr, origin_x, origin_y);
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
    char *rgba;
    rgba = chart->cairo_info.colors_for_pie[now_num];
    r = trans_rgba(rgba, 1);
    g = trans_rgba(rgba, 2);
    b = trans_rgba(rgba, 3);
    a = trans_rgba(rgba, 4);
    
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
                    
                    cairo_move_to(tag_cr, origin_x + delt_x-value_len/2.0, origin_y - delt_y-1*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    
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
                        
                        cairo_rotate(tag_cr, i*2*pi/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_rotate(tag_cr, -i*2*pi/5);
                        
                    }
                    if(i == 4)
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
                else if(i == 2)
                {
                    double delt_x, delt_y;
                    delt_x = length*sin(pi/5);
                    delt_y = length*cos(pi/5);
                    //数据注释
                    // if(if_show_tag)
                    {
                        cairo_rotate(tag_cr, pi/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_rotate(tag_cr, -pi/5);
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
                        cairo_rotate(tag_cr, pi*4/5);
                        cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                        cairo_show_text(tag_cr, value_2_str);
                        cairo_rotate(tag_cr, -pi*4/5);
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
                    cairo_rotate(tag_cr, i*pi/2);
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    cairo_rotate(tag_cr, -i*pi/2);
                    cairo_stroke(tag_cr);
                }
                if(i == 3)
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
                    cairo_rotate(tag_cr, i*2*pi/3);
                    cairo_move_to(tag_cr, first_x-value_len/2.0, first_y-1.3*font_size);
                    cairo_show_text(tag_cr, value_2_str);
                    cairo_rotate(tag_cr, -i*2*pi/3);
                    cairo_stroke(tag_cr);
                }
                if(i == 2)
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
        
        if(TwDMGetSiblingNode(datanode, TRUE))
        {
            datanode = TwDMGetSiblingNode(datanode, TRUE);
            value_table = TwDMGetNodeValue(datanode, FALSE);
        }
    }

    // int if_show_tag;
    int if_show_legend;
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
    if_show_legend = TTableGetInt(node_info_table, TStringID("if_show_legend"));
    int if_show_scale = TTableGetInt(node_info_table, TStringID("if_show_scale"));
    chart->cairo_info.if_show_tag = if_show_tag;
    chart->cairo_info.if_show_legend = if_show_legend;
    chart->cairo_info.if_show_grid = 0;
    chart->cairo_info.if_show_scale = if_show_scale;
}


void _ChartChangeFromRadar2Coor(void *obj, T_ID show_node_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;

    T_ID x_type, y_type;
    TwDataNode *node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
    TTable *value = TwDMGetNodeValue(node, FALSE);
    if(value == NULL)
        return;
    x_type = TTableGetType(value, TStringID("x"));
    y_type = TTableGetType(value, TStringID("y"));

    //***************第一步，首先解析配置文件*************************************//
    T_ID dm_type = TwDMGetRootType(TwDMGetNodeDM(chart->root_nodes[0]));
	//获得config.ini
	TTable *config_table = TLoadIniFile(NULL, TGetExecutePath("../etc/config.ini"), NULL);
	if(config_table == NULL)
		return;
	//获得config.ini中的子表list_layout
	TTable *list_layout = TTableGetTable(config_table, TStringID("list_layout"));
	if(list_layout == NULL) 
		return;
	//去每个文件中寻找show_root_type对应的dm
	int list_layout_count = TTableGetCount(list_layout);
	char file_path[256] = {0};
	TTable *file_table, *info_table, *node_info_table;
	for(int i = 0; i < list_layout_count; i++)
	{
		char *file_name = TTableGetIndexString(list_layout, i, NULL);
		sprintf(file_path, "%s/%s", TGetExecutePath("../etc"), file_name);
		file_table = TLoadIniFile(NULL, file_path, NULL);
		if(TTableGetTable(file_table, dm_type) != NULL)
		{
			info_table = TTableGetTable(file_table, dm_type);
			break;
		}
	}
	if(info_table == NULL)
		return;
    node_info_table = TTableGetTable(info_table, show_node_type);
    TwDataModel *dm;
    int node_counts;
    node_counts = TwDMGetChildNodeCount(chart->root_nodes[0]);
    for(int i = 0; i < chart->data_groups; i++)
    {
        dm = TwDMGetNodeDM(chart->root_nodes[i]);
        TwDMLock(dm);
        TTable *tables[node_counts];
        TwDataNode *last_node = TwDMGetChildNode(chart->root_nodes[i], TRUE);
        for(int j = 0; j < node_counts; j++)
        {
            tables[j] = TTableCreate();
            tables[j] = TTableCopy(TwDMGetNodeValue(last_node, FALSE), tables[j]);
            last_node = TwDMGetSiblingNode(last_node, TRUE);
        }
        TwDMDeleteNodeAllChild(chart->root_nodes[i]);
        for(int j = 0; j < node_counts; j++)
        {
            TwDataNode *new_node = TwDMAddNode(chart->root_nodes[i], show_node_type);
            TwDMSetNodeValue(new_node, tables[j]);
        }
        TwDMUnlock(dm);
        if(x_type == T_VALUE_INT )
            showDataInt((TWidget*)obj, chart->root_nodes[i], info_table);
        else if(x_type == T_VALUE_FLOAT)
            showDataDouble((TWidget*)obj, chart->root_nodes[i], info_table);
        else if(x_type == T_VALUE_STRING)
            showDataString((TWidget*)obj, chart->root_nodes[i], info_table);
    }
    int if_show_tag, if_show_grid, if_show_legend;
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
    if_show_grid = TTableGetInt(node_info_table, TStringID("if_show_grid"));
    if_show_legend = TTableGetInt(node_info_table, TStringID("if_show_legend"));
    chart->cairo_info.if_show_grid = if_show_grid;
    chart->cairo_info.if_show_tag = if_show_tag;
    chart->cairo_info.if_show_legend = if_show_legend;
}

void _ChartChangeFromPie2Coor(void *obj, T_ID show_root_type)
{
    TD_CHART *chart= (TD_CHART*)obj;
    TDisplayCell *dest_vc = chart->axis_vc;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *data_surface = chart->data_surface;
    cairo_surface_t *legend_surface = chart->legend_surface;
    cairo_surface_t *tag_surface = chart->tag_surface;
    cairo_surface_t *axis_surface = chart->axis_surface;
    cairo_t *cr = chart->cr;
    cairo_t *data_cr = chart->data_cr;
    cairo_t *legend_cr = chart->legend_cr;
    cairo_t *tag_cr = chart->tag_cr;
    cairo_t *axis_cr = chart->axis_cr;
    double origin_x, origin_y;
    origin_x = chart->cairo_info.origin_x;
    origin_y = chart->cairo_info.origin_y;
    int w = chart->surface_w;
    int h = chart->surface_h;
    double pie_r;
    pie_r = chart->cairo_info.pie_r;

    T_ID x_type, y_type;
    TwDataNode *node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
    TTable *value = TwDMGetNodeValue(node, FALSE);
    if(value == NULL)
        return;
    x_type = TTableGetType(value, TStringID("x"));
    y_type = TTableGetType(value, TStringID("y"));

    //***************第一步，首先解析配置文件*************************************//
    T_ID dm_type = TwDMGetRootType(TwDMGetNodeDM(chart->root_nodes[0]));
	//获得config.ini
	TTable *config_table = TLoadIniFile(NULL, TGetExecutePath("../etc/config.ini"), NULL);
	if(config_table == NULL)
		return;
	//获得config.ini中的子表list_layout
	TTable *list_layout = TTableGetTable(config_table, TStringID("list_layout"));
	if(list_layout == NULL) 
		return;
	//去每个文件中寻找show_root_type对应的dm
	int list_layout_count = TTableGetCount(list_layout);
	char file_path[256] = {0};
	TTable *file_table, *info_table, *node_info_table;
	for(int i = 0; i < list_layout_count; i++)
	{
		char *file_name = TTableGetIndexString(list_layout, i, NULL);
		sprintf(file_path, "%s/%s", TGetExecutePath("../etc"), file_name);
		file_table = TLoadIniFile(NULL, file_path, NULL);
		if(TTableGetTable(file_table, dm_type) != NULL)
		{
			info_table = TTableGetTable(file_table, dm_type);
			break;
		}
	}
	if(info_table == NULL)
		return;
    node_info_table = TTableGetTable(info_table, show_root_type);
    TwDataModel *dm;
    int node_counts;
    node_counts = TwDMGetChildNodeCount(chart->root_nodes[0]);

    for(int i = 0; i < chart->data_groups; i++)
    {
        dm = TwDMGetNodeDM(chart->root_nodes[i]);
        TwDMLock(dm);
        TTable *tables[node_counts];
        TwDataNode *last_node = TwDMGetChildNode(chart->root_nodes[i], TRUE);
        for(int j = 0; j < node_counts; j++)
        {
            tables[j] = TTableCreate();
            tables[j] = TTableCopy(TwDMGetNodeValue(last_node, FALSE), tables[j]);
            last_node = TwDMGetSiblingNode(last_node, TRUE);
        }
        TwDMDeleteNodeAllChild(chart->root_nodes[i]);
        for(int j = 0; j < node_counts; j++)
        {
            TwDataNode *new_node = TwDMAddNode(chart->root_nodes[i], show_root_type);
            TwDMSetNodeValue(new_node, tables[j]);
        }
        TwDMUnlock(dm);
        if(x_type == T_VALUE_INT )
            showDataInt((TWidget*)obj, chart->root_nodes[i], info_table);
        else if(x_type == T_VALUE_FLOAT)
            showDataDouble((TWidget*)obj, chart->root_nodes[i], info_table);
        else if(x_type == T_VALUE_STRING)
            showDataString((TWidget*)obj, chart->root_nodes[i], info_table);
    }
    int if_show_tag, if_show_grid, if_show_legend;
    if_show_tag = TTableGetInt(node_info_table, TStringID("if_show_tag"));
    if_show_grid = TTableGetInt(node_info_table, TStringID("if_show_grid"));
    if_show_legend = TTableGetInt(node_info_table, TStringID("if_show_legend"));
    chart->cairo_info.if_show_grid = if_show_grid;
    chart->cairo_info.if_show_tag = if_show_tag;
    chart->cairo_info.if_show_legend = if_show_legend;
}