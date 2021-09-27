#include "tdChart.h"
#include "main.h"


extern void merge_surface(cairo_t *dest_cr, cairo_surface_t *source_sfc, cairo_operator_t op);




/**
 *函数名:load_info_layout 
 *参数:obj:控件对象结构体；
		widget_info:配置文件中的该控件的table
 *返回值：无
 *作用：获取当前控件的在配置文件中配置的信息
 **/
void load_info_layout(void *obj, TTable *widget_info)
{
	TD_CHART *chart= (TD_CHART*)obj;	
	char *axis_type = TTableGetString(widget_info, TStringID("axis"), NULL);
	if(!axis_type)
	{
		chart->axis_type = TYPE_COORDINATES;
	}
	else
	{
		if(strcmp(axis_type, "pie") == 0)
			chart->axis_type = TYPE_PIE;
		else if(strcmp(axis_type, "radar") == 0)
			chart->axis_type = TYPE_RADAR;
		else if(strcmp(axis_type, "coordinates") == 0)
			chart->axis_type = TYPE_COORDINATES;
		else if(strcmp(axis_type, "echarts") == 0)
			chart->axis_type = TYPE_ECHARTS;
		else
			TLogW("Unknown axis type %s!\n", axis_type);
	}
}


void TwChartSetXCaption(TWidget *widget, char *caption, Tintp len)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	// int cap_len = 0;
	// cap_len =  
	if(chart->x_cap_dc)
	{
		DcSetText(chart->x_cap_dc, caption, len, 0);
		DcShowHide(chart->x_cap_dc, TRUE);
	}
	else
	{
		chart->x_cap_dc = DcCreate(DC_INPUT_WINDOW, chart->vc, \
			chart->cairo_info.origin_x + chart->cairo_info.axis_w + 5, \
			chart->cairo_info.origin_y + 2, 50, 25, widget);
		DcSetTextAline(chart->x_cap_dc, DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
		DcSetText(chart->x_cap_dc, caption, len, 0);
		DcShowHide(chart->x_cap_dc, TRUE);
	}
}

void TwChartSetYCaption(TWidget *widget, char *caption, Tintp len)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	// int cap_len = 0;
	// cap_len =  
	if(chart->y_cap_dc)
	{
		DcSetText(chart->y_cap_dc, caption, len, 0);
		DcShowHide(chart->y_cap_dc, TRUE);
	}
	else
	{
		chart->y_cap_dc = DcCreate(DC_INPUT_WINDOW, chart->vc, \
			chart->cairo_info.origin_x - 52, \
			chart->cairo_info.origin_y + chart->cairo_info.axis_h + 2, 50, 25, widget);
		DcSetTextAline(chart->y_cap_dc, DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
		DcSetText(chart->y_cap_dc, caption, len, 0);
		DcShowHide(chart->y_cap_dc, TRUE);
	}
}

/**
 *函数名:TwChartSetScaleDouble 
 *参数:widget:控件对象；
        scale_type：x轴或者y轴
		start：起始值
		end：结束值
		unit：每份值
 *返回值：无
 *作用：平面直角坐标设置范围
 **/
void TwChartSetScaleDouble(TWidget *widget, SCALE_TYPE scale_type, double scales[], double num)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	_ChartSetScaleDouble(chart, scale_type, scales, num);
}

/**
 *函数名:TwChartSetScaleInt 
 *参数:widget:控件对象；
        scale_type：x轴或者y轴
		start：起始值
		end：结束值
		unit：每份值
 *返回值：无
 *作用：平面直角坐标设置范围
 **/
void TwChartSetScaleInt(TWidget *widget, SCALE_TYPE scale_type, int scales[], int num)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	if(scale_type == SCALE_TYPE_X)
	{
		chart->cairo_info.x_data_type = 0;
		chart->cairo_info.x_set_ok = 1;
	}
	else if(scale_type == SCALE_TYPE_Y)
	{
		chart->cairo_info.y_data_type = 0;
		chart->cairo_info.y_set_ok = 1;
	}
	else if(scale_type == SCALE_TYPE_Y2)
	{
		chart->cairo_info.y2_data_type = 0;
		chart->cairo_info.y2_set_ok = 1;
	}
	
	_ChartSetScaleInt(chart, scale_type, scales, num);
}

/**
 *函数名:TwChartSetScaleString 
 *参数:widget:控件对象；
        scale_type：x轴或者y轴
		content：字符串数组
		num：刻度数量
 *返回值：无
 *作用：平面直角坐标设置范围
 **/
void TwChartSetScaleString(TWidget *widget, SCALE_TYPE scale_type, char **content, int num)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	_ChartSetScaleString(chart, scale_type, content, num);
}

/**
 *函数名:TwChartSetLegendText 
 *参数:widget:控件对象；
        root_node：根结点
		text：图例文本
 *返回值：无
 *作用：设置图例文本
 **/
void TwChartSetLegendText(TWidget *widget, TwDataNode *root_node, char *text)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	//_ChartCreateLegend(chart, node_type, text);
	TwDataModel *dm;
	TTable *value_table;
	dm = TwDMGetNodeDM(root_node);
	TwDMLock(dm);
	value_table = TwDMGetNodeValue(root_node, TRUE);
	TTableAddString(value_table, TStringID("legend_text"), text, -1);
	TwDMUnlock(dm);


}

/**
 *函数名:TwChartSetType 
 *参数:widget:控件对象；
        type：图表类型
 *返回值：无
 *作用：初始化图表类型
 **/
void TwChartSetType(TWidget *widget, CHART_TYPE type)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	
	TD_CHART *chart= (TD_CHART *)widget; 
	chart->axis_type = type;
	// destroy_extra_dc();
	_ChartCreateAxis(chart, chart->surface_w, chart->surface_h);

}

/**
 *函数名:TwChartShowLegend 
 *参数:widget:控件对象；
        show_hide：显示/隐藏
 *返回值：无
 *作用：显示或者隐藏图例
 **/
void TwChartShowLegend(TWidget *widget, Tbool show_hide) 
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
    
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

	TDisplayCell *dest_vc = chart->axis_vc;
    int w = chart->surface_w;
    int h = chart->surface_h;


	if(show_hide)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 1 ,1, 0, 0);
		cairo_paint(cr);
		
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_scale)
			merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
		if(chart->axis_type == TYPE_COORDINATES && chart->cairo_info.if_show_grid)
			merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0)
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_tag)
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		for(int i = 0; i < chart->data_groups; i++)
		{
			DcShowHide(chart->legend_dc[i], TRUE);
		}
		chart->cairo_info.if_show_legend = 1;
	}
	else
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 1 ,1, 0, 0);
		cairo_paint(cr);
		
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_scale)
			merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
		if(chart->axis_type == TYPE_COORDINATES && chart->cairo_info.if_show_grid)
			merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0)
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_tag)
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		// merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		for(int i = 0; i < chart->data_groups; i++)
		{
			DcShowHide(chart->legend_dc[i], FALSE);
		}
		chart->cairo_info.if_show_legend = 0;
	}
	chart_update(dest_vc, surface, cr, w, h);
}

/**
 *函数名:TwChartShowTag 
 *参数:widget:控件对象；
        show_hide：显示/隐藏
 *返回值：无
 *作用：显示或者隐藏数据标注
 **/
void TwChartShowTag(TWidget *widget, Tbool show_hide)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
    
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

	TDisplayCell *dest_vc = chart->axis_vc;
    int w = chart->surface_w;
    int h = chart->surface_h;

	if(show_hide)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 1 ,1, 0, 0);
		cairo_paint(cr);
		
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_scale)
			merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
		if(chart->axis_type == TYPE_COORDINATES && chart->cairo_info.if_show_grid)
			merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0)
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		
		merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		chart->cairo_info.if_show_tag = 1;
	}
	else
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 1 ,1, 0, 0);
		cairo_paint(cr);
		
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_scale)
			merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
		if(chart->axis_type == TYPE_COORDINATES && chart->cairo_info.if_show_grid)
			merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0)
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		// merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		chart->cairo_info.if_show_tag = 0;
	}
	chart_update(dest_vc, surface, cr, w, h);
}


/**
 *函数名:TwChartSetDM 
 *参数:chart:控件对象；
        show_root:需要显示的数据组对应的根节点；
		show_root_type：节点显示类型
 *返回值：无
 *作用：将数据与图表控件绑定，在该控件中显示数据
 **/
void TwChartSetDM(TWidget *widget, TwDataNode *show_root, T_ID show_node_type)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
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
		if(TTableGetTable(file_table, show_node_type) != NULL)
		{
			info_table = TTableGetTable(file_table, show_node_type);
			break;
		}
	}
	if(info_table == NULL)
		return;
	_chartShowData(widget, show_root, info_table);
}


/**
 *函数名:TwChartShowGrid 
 *参数:widget:控件对象；
        show_hide：显示/隐藏
 *返回值：无
 *作用：显示或者隐藏网格
 **/
void TwChartShowGrid(TWidget *widget, Tbool if_show)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
    if(chart->axis_type != TYPE_COORDINATES)
		return;
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

	TDisplayCell *dest_vc = chart->axis_vc;
    int w = chart->surface_w;
    int h = chart->surface_h;

	if(if_show)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 1 ,1, 0, 0);
		cairo_paint(cr);

		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_scale)
			merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0)
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		
		merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_tag)
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		chart->cairo_info.if_show_grid = 1;
	}
	else
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 0, 0, 0, 0);
		cairo_paint(cr);
		
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_scale)
			merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0);
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_tag)
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		chart->cairo_info.if_show_grid = 0;
	}
	chart_update(dest_vc, surface, cr, w, h);
}


/**
 *函数名:TwChartShowScale 
 *参数:widget:控件对象；
        show_hide：显示/隐藏
 *返回值：无
 *作用：显示或者隐藏刻度
 **/
void TwChartShowScale(TWidget *widget, Tbool if_show)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
    
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

	TDisplayCell *dest_vc = chart->axis_vc;
    int w = chart->surface_w;
    int h = chart->surface_h;

	if(if_show)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 1 ,1, 0, 0);
		cairo_paint(cr);

		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		merge_surface(cr, scale_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0)
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		if(chart->axis_type == TYPE_COORDINATES && chart->cairo_info.if_show_grid)
			merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_tag)
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		chart->cairo_info.if_show_scale = 1;
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			DcShowHide(chart->dc_x[i], TRUE);
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			DcShowHide(chart->dc_y[i], TRUE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			DcShowHide(chart->dc_y2[i], TRUE);
		}
	}
	else
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgba(cr, 1 ,1, 0, 0);
		cairo_paint(cr);
		
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
		if(chart->data_groups > 0)
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_grid)
			merge_surface(cr, grid_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_tag)
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		chart->cairo_info.if_show_scale = 0;
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			DcShowHide(chart->dc_x[i], FALSE);
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			DcShowHide(chart->dc_y[i], FALSE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			DcShowHide(chart->dc_y2[i], FALSE);
		}
	}
	chart_update(dest_vc, surface, cr, w, h);
}


/**
 *函数名:TwChartChangeFrame 
 *参数:widget:控件对象；
		type:需要将当前图表更换为该类型
		show_root_type：根结点类型
 *返回值：无
 *作用：更改当前图表的类型
 **/
void TwChartChangeFrame(TWidget *widget, CHART_TYPE type, T_ID show_root_type)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
    if(chart->data_groups == 0)
		return;
    cairo_surface_t *surface = chart->surface;
    cairo_surface_t *grid_surface= chart->grid_surface;
	cairo_surface_t *axis_surface = chart->axis_surface;
	cairo_surface_t *data_surface = chart->data_surface;
	cairo_surface_t *tag_surface = chart->tag_surface;
	cairo_surface_t *legend_surface = chart->legend_surface;
	cairo_surface_t *scale_surface = chart->scale_surface;
	cairo_surface_t *his_surface = chart->his_surface;
	cairo_surface_t *his_tag_surface = chart->his_tag_surface;
    cairo_t *cr = chart->cr;
    cairo_t *grid_cr = chart->grid_cr;
	cairo_t *axis_cr= chart->axis_cr;
	cairo_t *data_cr= chart->data_cr;
	cairo_t *tag_cr= chart->tag_cr;
	cairo_t *legend_cr= chart->legend_cr;
	cairo_t *scale_cr= chart->scale_cr;
	cairo_t *his_cr= chart->his_cr;
	cairo_t *his_tag_cr= chart->his_tag_cr;

	TDisplayCell *dest_vc = chart->axis_vc;
    int w = chart->surface_w;
    int h = chart->surface_h;
	int old_type;
	int node_count;
	old_type = chart->axis_type;
	chart->axis_type = type;
	node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);

	_ChartCreateAxis(chart, chart->axis_vc->w, chart->axis_vc->h);

	if(old_type == TYPE_COORDINATES && type == TYPE_PIE)
	{
		if(chart->data_groups > 1)
		{
			TLogW("Too many datanode groups! Pie Graph can only support one serial of data!");
		}
		_ChartChangeFromCoor2Pie((void *)widget, show_root_type);

		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
		{
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
			for(int i = 0; i < chart->cairo_info.legend_num; i++)
			{
				DcShowHide(chart->legend_dc[i], TRUE);
			}
		}
		if(chart->data_groups > 0)
		{
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		}
		if(chart->cairo_info.if_show_tag)
		{
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		}
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			DcShowHide(chart->dc_x[i], FALSE);
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			DcShowHide(chart->dc_y[i], FALSE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			DcShowHide(chart->dc_y2[i], FALSE);
		}
		// TwChartShowScale(chart, FALSE);
	}
	else if(old_type == TYPE_COORDINATES && type == TYPE_RADAR)
	{
		for(int i = 0; i < chart->data_groups; i++)
		{
			if(TwDMGetChildNodeCount(chart->root_nodes[i]) > 6)
			{
				TLogW("Number of data in one serial is out of range(max:6)!\n");
				break;
			}
		}
		ChartChangeFromCoor2RadarChangeLegend((void *)chart, show_root_type);
		ChartChangeFromCoor2RadarChangeScale((void *)chart);
		
		for(int i = 0; i < chart->data_groups; i++)
		{
			_ChartChangeFromCoor2Radar((void *)widget, chart->root_nodes[i], show_root_type);
		}

		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
		{
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
			for(int i = 0; i < chart->cairo_info.legend_num; i++)
			{
				DcShowHide(chart->legend_dc[i], TRUE);
			}
		}
		if(chart->data_groups > 0)
		{
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		}
		if(chart->cairo_info.if_show_tag)
		{
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
		}
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			if(i == 0)
				DcShowHide(chart->dc_x[i], FALSE);
			else 
				DcShowHide(chart->dc_x[i], TRUE);
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			DcShowHide(chart->dc_y[i], FALSE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			DcShowHide(chart->dc_y2[i], FALSE);
		}
	}
	//从雷达图转为直角坐标图
	else if(old_type == TYPE_RADAR && type == TYPE_COORDINATES)
	{

		TwDataNode *node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
		TTable *value = TwDMGetNodeValue(node, FALSE);
		int node_count;
		node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);
		if(!chart->cairo_info.x_set_ok)
		{
			if(TTableGetType(value, TStringID("x")) == T_VALUE_INT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_int_info.x_scale[i] = TTableGetInt(value, TStringID("x"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_int_info.x_scale, node_count, sizeof(int), compare_int);
				TwChartSetScaleInt(widget, SCALE_TYPE_X, chart->cairo_info.scale_int_info.x_scale, node_count);
			}
			else if(TTableGetType(value, TStringID("x")) == T_VALUE_FLOAT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_double_info.x_scale[i] = TTableGetFloat(value, TStringID("x"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_double_info.x_scale, node_count, sizeof(double), compare_double);
				TwChartSetScaleDouble(widget, SCALE_TYPE_X, chart->cairo_info.scale_double_info.x_scale, node_count);
			}
			else if(TTableGetType(value, TStringID("x")) == T_VALUE_STRING)
			{
				for(int i = 0; i < node_count; i++)
				{
					sprintf(chart->cairo_info.scale_string_info.x_string[i], "%s", TTableGetString(value, TStringID("x"), NULL));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				TwChartSetScaleString(widget, SCALE_TYPE_X, chart->cairo_info.scale_string_info.x_string, node_count);
			}
            chart->cairo_info.x_set_ok = 1;
		}
		node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
		value = TwDMGetNodeValue(node, FALSE);
		if(!chart->cairo_info.y_set_ok)
		{
			if(TTableGetType(value, TStringID("y")) == T_VALUE_INT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_int_info.y_scale[i] = TTableGetInt(value, TStringID("y"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_int_info.y_scale, node_count, sizeof(int), compare_int);
				deal_y_scale_int((void *)chart, node_count);
				TwChartSetScaleInt(widget, SCALE_TYPE_Y, chart->cairo_info.scale_int_info.y_scale, 10);
			}
			else if(TTableGetType(value, TStringID("y")) == T_VALUE_FLOAT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_double_info.y_scale[i] = TTableGetFloat(value, TStringID("y"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_double_info.y_scale, node_count, sizeof(double), compare_double);
				deal_y_scale_double((void *)chart, node_count);
				TwChartSetScaleDouble(widget, SCALE_TYPE_Y, chart->cairo_info.scale_double_info.y_scale, 10);
			}
			else if(TTableGetType(value, TStringID("y")) == T_VALUE_STRING)
			{
				for(int i = 0; i < node_count; i++)
				{
					sprintf(chart->cairo_info.scale_string_info.y_string[i], "%s", TTableGetString(value, TStringID("x"), NULL));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				TwChartSetScaleString(widget, SCALE_TYPE_Y, chart->cairo_info.scale_string_info.y_string, node_count);
			}
            chart->cairo_info.y_set_ok = 1;
		}

		ChartChangeFromRadar2CoorChangeLegend((void *)chart, show_root_type);
		// ChartChangeFromRadar2CoorChangeScale((void *)chart);

		_ChartChangeFromRadar2Coor((void *)widget, show_root_type);
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
		{
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
			for(int i = 0; i < chart->cairo_info.legend_num; i++)
			{
				DcShowHide(chart->legend_dc[i], TRUE);
			}
		}
		if(chart->data_groups > 0)
		{
			merge_surface(cr, his_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		}
		if(chart->cairo_info.if_show_tag)
		{
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, his_tag_surface, CAIRO_OPERATOR_OVER);
		}
		
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			if(chart->dc_x[i] != NULL)
				DcShowHide(chart->dc_x[i], TRUE);
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			if(chart->dc_y[i] != NULL)
				DcShowHide(chart->dc_y[i], TRUE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			if(chart->dc_y2[i] != NULL)
				DcShowHide(chart->dc_y2[i], TRUE);
		}
	}
	//从饼图转为直角坐标图
	else if(old_type == TYPE_PIE && type == TYPE_COORDINATES)
	{
		TwDataNode *node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
		TTable *value = TwDMGetNodeValue(node, FALSE);
		int node_count;
		node_count = TwDMGetChildNodeCount(chart->root_nodes[0]);
		if(!chart->cairo_info.x_set_ok)
		{
			if(TTableGetType(value, TStringID("x")) == T_VALUE_INT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_int_info.x_scale[i] = TTableGetInt(value, TStringID("x"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_int_info.x_scale, node_count, sizeof(int), compare_int);
				TwChartSetScaleInt(widget, SCALE_TYPE_X, chart->cairo_info.scale_int_info.x_scale, node_count);
			}
			else if(TTableGetType(value, TStringID("x")) == T_VALUE_FLOAT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_double_info.x_scale[i] = TTableGetFloat(value, TStringID("x"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_double_info.x_scale, node_count, sizeof(double), compare_double);
				TwChartSetScaleDouble(widget, SCALE_TYPE_X, chart->cairo_info.scale_double_info.x_scale, node_count);
			}
			else if(TTableGetType(value, TStringID("x")) == T_VALUE_STRING)
			{
				for(int i = 0; i < node_count; i++)
				{
					sprintf(chart->cairo_info.scale_string_info.x_string[i], "%s", TTableGetString(value, TStringID("x"), NULL));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				TwChartSetScaleString(widget, SCALE_TYPE_X, chart->cairo_info.scale_string_info.x_string, node_count);
			}
            chart->cairo_info.x_set_ok = 1;
		}
		node = TwDMGetChildNode(chart->root_nodes[0], TRUE);
		value = TwDMGetNodeValue(node, FALSE);
		if(!chart->cairo_info.y_set_ok)
		{
			if(TTableGetType(value, TStringID("y")) == T_VALUE_INT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_int_info.y_scale[i] = TTableGetInt(value, TStringID("y"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_int_info.y_scale, node_count, sizeof(int), compare_int);
				deal_y_scale_int((void *)chart, node_count);
				TwChartSetScaleInt(widget, SCALE_TYPE_Y, chart->cairo_info.scale_int_info.y_scale, 10);
			}
			else if(TTableGetType(value, TStringID("y")) == T_VALUE_FLOAT)
			{
				for(int i = 0; i < node_count; i++)
				{
					chart->cairo_info.scale_double_info.y_scale[i] = TTableGetFloat(value, TStringID("y"));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				qsort(chart->cairo_info.scale_double_info.y_scale, node_count, sizeof(double), compare_double);
				deal_y_scale_double((void *)chart, node_count);
				TwChartSetScaleDouble(widget, SCALE_TYPE_Y, chart->cairo_info.scale_double_info.y_scale, 10);
			}
			else if(TTableGetType(value, TStringID("y")) == T_VALUE_STRING)
			{
				for(int i = 0; i < node_count; i++)
				{
					sprintf(chart->cairo_info.scale_string_info.y_string[i], "%s", TTableGetString(value, TStringID("x"), NULL));
					node = TwDMGetSiblingNode(node, TRUE);
					if(node == NULL)
						break;
					value = TwDMGetNodeValue(node, FALSE);
				}
				TwChartSetScaleString(widget, SCALE_TYPE_Y, chart->cairo_info.scale_string_info.y_string, node_count);
			}
            chart->cairo_info.y_set_ok = 1;
		}

		ChartChangeFromPie2CoorChangeLegend((void *)chart, show_root_type);
		// ChartChangeFromPie2CoorChangeScale((void *)chart);

		_ChartChangeFromPie2Coor((void *)widget, show_root_type);
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
		{
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
			for(int i = 0; i < chart->cairo_info.legend_num; i++)
			{
				DcShowHide(chart->legend_dc[i], TRUE);
			}
		}
		if(chart->data_groups > 0)
		{
			merge_surface(cr, his_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		}
		if(chart->cairo_info.if_show_tag)
		{
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, his_tag_surface, CAIRO_OPERATOR_OVER);
		}
		
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			if(chart->dc_x[i] != NULL)
				DcShowHide(chart->dc_x[i], TRUE);
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			if(chart->dc_y[i] != NULL)
				DcShowHide(chart->dc_y[i], TRUE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			if(chart->dc_y2[i] != NULL)
				DcShowHide(chart->dc_y2[i], TRUE);
		}
	}
	//从饼图转为雷达图
	else if(old_type == TYPE_PIE && type == TYPE_RADAR)
	{

		for(int i = 0; i < chart->data_groups; i++)
		{
			_ChartChangeFromPie2Radar((void *)widget, chart->root_nodes[i], show_root_type);
		}
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
		{
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
			for(int i = 0; i < chart->cairo_info.legend_num; i++)
			{
				DcShowHide(chart->legend_dc[i], TRUE);
			}
		}
		if(chart->data_groups > 0)
		{
			merge_surface(cr, his_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		}
		if(chart->cairo_info.if_show_tag)
		{
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, his_tag_surface, CAIRO_OPERATOR_OVER);
		}
		
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			if(chart->dc_x[i] != NULL)
			{
				DcShowHide(chart->dc_x[i], TRUE);
			}
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			if(chart->dc_y[i] != NULL)
				DcShowHide(chart->dc_y[i], TRUE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			if(chart->dc_y2[i] != NULL)
				DcShowHide(chart->dc_y2[i], TRUE);
		}
	}
	//从雷达图转为饼图
	else if(old_type == TYPE_RADAR && type == TYPE_PIE)
	{
		if(chart->data_groups > 1)
		{
			TLogW("Too many datanode groups! Pie Graph can only support one serial of data!");
		}
		ChartChangeFromRadar2PieChangeLegend((void *)chart, show_root_type);
		// ChartChangeFromRadar2PieChangeScale((void *)chart);

		_ChartChangeFromRadar2Pie((void *)widget, show_root_type);
		
		merge_surface(cr, axis_surface, CAIRO_OPERATOR_OVER);
		if(chart->cairo_info.if_show_legend)
		{
			merge_surface(cr, legend_surface, CAIRO_OPERATOR_OVER);
			for(int i = 0; i < chart->cairo_info.legend_num; i++)
			{
				DcShowHide(chart->legend_dc[i], FALSE);
			}
			for(int i = 0; i < node_count; i++)
			{
				DcShowHide(chart->pie_legend_dc[i], TRUE);
			}
		}
		if(chart->data_groups > 0)
		{
			merge_surface(cr, his_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, data_surface, CAIRO_OPERATOR_OVER);
		}
		if(chart->cairo_info.if_show_tag)
		{
			merge_surface(cr, tag_surface, CAIRO_OPERATOR_OVER);
			merge_surface(cr, his_tag_surface, CAIRO_OPERATOR_OVER);
		}
		
		for(int i = 0; i < chart->x_dc_num; i++)
		{
			if(chart->dc_x[i] != NULL)
				DcShowHide(chart->dc_x[i], TRUE);
		}
		for(int i = 0; i < chart->y_dc_num; i++)
		{
			if(chart->dc_y[i] != NULL)
				DcShowHide(chart->dc_y[i], TRUE);
		}
		for(int i = 0; i < chart->y2_dc_num; i++)
		{
			if(chart->dc_y2[i] != NULL)
				DcShowHide(chart->dc_y2[i], TRUE);
		}
	}




	chart_update(dest_vc, surface, cr, w, h);
	chart->axis_type = type;
	TwUpdateShow();
}


/**
 *函数名:TwChartGetFrameType 
 *参数:widget:控件对象；
		img：需要设置为背景的颜色
 *返回值：无
 *作用：设置当前控件的背景颜色
 **/
int TwChartGetFrameType(TWidget *widget)
{
	if(TObjectGetType((void *)widget) == TW_CHART)
	{
		TD_CHART *chart= (TD_CHART*)widget;
		return chart->axis_type;
	}
	else
	{
		TLogE("Widget is not type chart!\n");
	}
}

/**
 *函数名:TwChartSetBackgroundColor 
 *参数:widget:控件对象；
		img：需要设置为背景的颜色
 *返回值：无
 *作用：设置当前控件的背景颜色
 **/
void TwChartSetBackgroundColor(TWidget *widget, TColor color)
{
	if(TObjectGetType((void *)widget) == TW_CHART)
	{
		TD_CHART *chart= (TD_CHART*)widget;
		DcSetBackgroundColor(chart->axis_vc, color);
		DcUpdateShow();
	}
	else
	{
		TLogE("Widget is not type chart!\n");
	}
}

/**
 *函数名:TwChartSetBackgroundImage 
 *参数:widget:控件对象；
		img：需要设置为背景的图片
 *返回值：无
 *作用：设置当前控件的背景图片
 **/
void TwChartSetBackgroundImage(TWidget *widget, Pixmap img)
{
	if(TObjectGetType((void *)widget) == TW_CHART)
	{
		TD_CHART *chart= (TD_CHART*)widget;
		
		DcSetBackgroundPixmap(chart->axis_vc, img, chart->axis_vc->w, chart->axis_vc->h, DC_STRETCH_NORMAL);
		DcUpdateShow();
		return;
	}
	else
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
}

/**
 *函数名:TwChartSetDefaultColors 
 *参数:widget:控件对象；
		colors:颜色字符串数组
		num：需要替换多少个颜色
 *返回值：无
 *作用：将默认的颜色替换为自定义颜色
 **/
void TwChartSetDefaultColors(TWidget *widget, char **colors, int num)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	int i = 0;
	for(int i = 0; i < num; i++)
	{
		chart->cairo_info.colors_for_pie[i] = colors[i];
	}

}

/**
 *函数名:TwChartGetValue 
 *参数:widget:控件对象；
 *返回值：Tintp
 *作用：返回图表控件的值value
 **/
Tintp TwChartGetValue(TWidget *widget)
{
	if(TObjectGetType((void *)widget) != TW_CHART)
	{
		TLogE("Widget is not type chart!\n");
		return -1;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	return chart->value;
}

/**
 *函数名:chart_set_caption 
 *参数:widget:控件对象；
		str：需要显示的字符串文本
		len:需要显示的长度
 *返回值：无
 *作用：设置当前控件的caption文本
 **/
void chart_set_caption(TWidget *widget, const char *str, int len)
{
	TD_CHART *chart= (TD_CHART*)widget;
	DcSetTextAline(chart->caption_vc, DC_TEXT_X_MIDDLE | DC_TEXT_Y_MIDDLE);
	DcSetText(chart->caption_vc, str, -1, 0);
	DcUpdateShow();
	chart->caption = str;
}

/**
 *函数名:chart_get_caption 
 *参数:widget:控件对象；
		plen:返回字符串的长度
 *返回值：字符串
 *作用：获取当前控件的caption文本
 **/
const char *chart_get_caption(TWidget *widget, int *plen)
{
	TD_CHART *chart= (TD_CHART*)widget;	
	if(chart->caption != NULL)
	{
		*plen = strlen(chart->caption);
		return chart->caption;
	}
	else
	{
		*plen = 0;
		return NULL;
	}
}

/**
 *函数名:chart_change_style 
 *参数:widget:控件对象；
		plen:返回字符串的长度
 *返回值：字符串
 *作用：获取当前控件的caption文本
 **/
const char *chart_change_style(TWidget *widget, T_ID style)
{
	TD_CHART *chart= (TD_CHART*)widget;	
	TwStyle *new_style = _TwFindStyle(TStringID("chart"), style);
	if(chart->axis_vc_style != new_style) {
		
        _TwSetStyle(chart->axis_vc, new_style, TW_STATUS_NONE);
	}
	return;
}


Tbool chart_set_value(TWidget *widget, Tintp value)
{
	TD_CHART *chart= (TD_CHART*)widget;
	if(value < 0)
	{
		TLogE("value can't be set to %d!\n", value);
		return FALSE;
	}
	else
	{
		chart->value = value;
		return TRUE;
	}
}

void chart_set_enable(TWidget *widget, Tbool if_enable)
{
	if(if_enable)
		_tnEnable(widget);
	else
		_tnDisable(widget);
}

void chart_move(TWidget *widget, Tint cx, Tint cy)
{
	TD_CHART *chart= (TD_CHART*)widget;
	DcMove(chart->vc, cx, cy);
	return;
}

void chart_show_hide(TWidget *widget, Tbool if_show)
{
	TD_CHART *chart= (TD_CHART*)widget;
	DcShowHide(chart->vc, if_show);
	return;
}

void chart_event_handler(TwEvent *event, TWidget *widget, TDisplayCell *vc)
{
	TD_CHART *chart= (TD_CHART*)widget;
	int current_x, current_y;
	switch (event->type)
	{
	case TW_EVENT_MOUSE_ENTER:
		if(TwGetValue(widget) == 1)
		{
			_TwSetStyle(chart->axis_vc, chart->style, TW_STATUS_ACTIVE);
			TwSetValue(widget, TW_STATUS_ACTIVE);
		}
		break;
	case TW_EVENT_MOUSE_LEAVE:
		if(TwGetValue(widget) == 2)
		{
			_TwSetStyle(chart->axis_vc, chart->style, TW_STATUS_NORMAL);
			TwSetValue(widget, TW_STATUS_NORMAL);
		}
		break;
	case TW_EVENT_BUTTON_DOWN:
		_TwSetStyle(chart->axis_vc, chart->style, TW_STATUS_SELECT);
		TwSetValue(widget, TW_STATUS_SELECT);
		current_x = event->x;
		current_y = event->y;
		// printf("down%d, %d\n", current_x, current_y);
		// TW_BUTTON_CLICK
		break;
	default:
		break;
	}
}
/*
void chart_resize(TWidget *widget, Tint w, Tint h)
{
	if(w <= 0 || h <= 0)
	{
		if(w <= 0)
			TLogE("Reset width error! %d <= 0", w);
		if(w <= 0)
			TLogE("Reset height error! %d <= 0", h);
		return;
	}
	TD_CHART *chart= (TD_CHART*)widget;
	DcResize(chart->axis_vc, w, h);
	DcResize(chart->caption_vc, w, 30);
	DcResize(chart->vc, w, h);
	chart->surface_w = chart->axis_vc->w;
	chart->surface_h = chart->axis_vc->h;

	DcUpdateShow();
}*/

static TwMethod chart_method = {
	// .event_handler = chart_event_handler,
	.set_value = chart_set_value,
	.change_style = chart_change_style,
	.set_caption = chart_set_caption,
	.get_caption = chart_get_caption,
	.set_enable = chart_set_enable,
	.move = chart_move,
	.show_hide = chart_show_hide,
	.event_handler = chart_event_handler,
	// .resize = chart_resize,
};


void _set_default_cairo_info(void *obj){
	TD_CHART *chart= (TD_CHART*)obj;
	chart->cairo_info.scale_font_size = 10;
	chart->cairo_info.if_show_grid = FALSE;
}

static int chart_obj_create(void *obj, TTable *in)
{
	TDisplayCell *parent_vc;
	TD_CHART *chart= (TD_CHART*)obj;
	TWidget *widget = (TWidget*)obj;

	TwLock();
	parent_vc = _TwInit(widget, &chart_method, in);
	if(parent_vc == NULL) {
		TwUnlock();
		return T_FAIL;
	}

	chart->vc = DcCreate(DC_WINDOW, parent_vc, widget->cx, widget->cy, widget->w, widget->h, widget);
	//chart->axis_vc = DcCreate(DC_INPUT_WINDOW, chart->vc, widget->w/20, widget->h/20, widget->w*18/20, widget->h*18/20, widget);
	chart->axis_vc = DcCreate(DC_INPUT_WINDOW, chart->vc, 0, 0, widget->w, widget->h, widget);
	chart->caption_vc = DcCreate(DC_INPUT_WINDOW, chart->vc, 0, 0, widget->w, 30, widget);
	chart->test = DcCreate(DC_INPUT_WINDOW, chart->vc, 100, 100, 300, 300, widget);
	chart->style = _TwFindStyle(TW_CHART, widget->style_name);
	_TwSetStyle(chart->vc, chart->style, TW_STATUS_NORMAL);
	DcSetBackgroundColor(chart->axis_vc, T_RGB(255, 255, 255));
	DcSetBackgroundColor(chart->test, T_RGB(255, 255, 255));
	
	//设置图表基本参数
	_set_default_cairo_info(chart);
	//chart->axis_type = TYPE_COORDINATES;//
	// chart->axis_type = TYPE_PIE;
	load_info_layout(obj, in);
	int surface_w, surface_h;
	chart->surface_w = chart->axis_vc->w;
	chart->surface_h = chart->axis_vc->h;
	chart->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, chart->surface_w, chart->surface_h);
	chart->cr = cairo_create(chart->surface);
	chart->data_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->data_cr = cairo_create(chart->data_surface);
	chart->legend_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->legend_cr = cairo_create(chart->legend_surface);
	chart->tag_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->tag_cr = cairo_create(chart->tag_surface);
	chart->grid_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->grid_cr = cairo_create(chart->grid_surface);
	chart->axis_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->axis_cr = cairo_create(chart->axis_surface);
	chart->scale_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->scale_cr = cairo_create(chart->scale_surface);
	chart->his_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->his_cr = cairo_create(chart->his_surface);
	chart->his_tag_surface = cairo_surface_create_similar(chart->surface, \
				CAIRO_CONTENT_COLOR_ALPHA, chart->surface_w, chart->surface_h);
	chart->his_tag_cr = cairo_create(chart->his_tag_surface);
	chart->data_groups = 0;

	chart->cairo_info.if_show_grid = 0;
	chart->cairo_info.if_show_tag = 0;
	chart->cairo_info.if_show_legend = 0;
	chart->cairo_info.if_show_scale = 0;
	
	//end
	chart->cairo_info.his_num = 0;
	chart->cairo_info.line_num = 0;
	chart->cairo_info.point_num = 0;

	chart->cairo_info.colors_for_pie[0] = "233,0,0,255";
	chart->cairo_info.colors_for_pie[1] = "233,233,0,255";
	chart->cairo_info.colors_for_pie[2] = "233,0,233,255";
	chart->cairo_info.colors_for_pie[3] = "0,233,233,255";
	chart->cairo_info.colors_for_pie[4] = "245,137,14,255";
	chart->cairo_info.colors_for_pie[5] = "137,137,14,255";
	chart->cairo_info.colors_for_pie[6] = "137,14,14,255";
	chart->cairo_info.colors_for_pie[7] = "137,245,14,255";
	chart->cairo_info.colors_for_pie[8] = "14,245,68,255";
	chart->cairo_info.colors_for_pie[9] = "79,137,14,255";
	chart->cairo_info.colors_for_pie[10] = "148,52,14,255";
	chart->cairo_info.colors_for_pie[11] = "178,137,66,255";
	chart->cairo_info.colors_for_pie[12] = "211,25,144,255";
	chart->cairo_info.colors_for_pie[13] = "123,137,14,255";
	chart->cairo_info.colors_for_pie[14] = "245,123,114,255";
	chart->cairo_info.colors_for_pie[15] = "21,22,28,255";
	// _ChartCreateAxis(obj, surface_w, surface_h);
	// DcSetExposeFunc(chart->vc, slider_text_expose);
	
	_TwSetFocusVC(widget, chart->vc);

	_TwInit_Finish(widget);
	TwUnlock();
	// chart->btn = TwCreate(TW_BUTTON, TwGetParent(widget), 0, 0, 200, 100, NULL, None);
	
	// tdCairo_GetPixmap()
	if(TTableGetString(in, TStringID("axis"), NULL) != NULL)
	{
		char *type_str = TTableGetString(in, TStringID("axis"), NULL);
		if(strcmp(type_str, "coordinates") == 0 || strcmp(type_str, "pie") == 0 || \
					strcmp(type_str, "radar") == 0 || strcmp(type_str, "echarts") == 0)
			TwChartSetType((TWidget *)widget, chart->axis_type);
		else
			TLogE("Create axis error! Unknown axis type %s!\n", type_str);
	}
	
	return T_SUCCESS;
}
static void chart_obj_destroy(void *obj)
{
	TD_CHART *chart = (TD_CHART*)obj;
	TwLock();
	_TwUnInit((TWidget*)obj);
	DcDestroy(chart->vc);
	TwUnlock();
	return ;
}



static TObjectType chart_obj_info = {
	.create_func = chart_obj_create,
	.destroy_func = chart_obj_destroy,
	.obj_size = sizeof(TD_CHART),
	.interface_num = 1,
	.interfaces = {None},
};
static TwStyleNode chart_style[] = {
	{
	.status_mask = TW_STATUS_NORMAL|TW_STATUS_DISABLE|TW_STATUS_ACTIVE|TW_STATUS_SELECT,
	.flags_mask = HAS_TEXT_COLOR,
	.text_color = T_BGR(255, 255, 255),
	.text_bg_color = T_BGR(128, 128, 128),
	}
};

static void init_chart(void) __attribute__((constructor));
void init_chart(void)
{
	chart_obj_info.interfaces[0] = T_STRING_ID(widget);
	TObjectRegisterType(TW_CHART, &chart_obj_info);
	_TwRegisterStyle(TW_CHART, T_ARRAY_NUM(chart_style), chart_style);
	return;
}