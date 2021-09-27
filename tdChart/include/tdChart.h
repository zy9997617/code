#ifndef _TDCHART_H_
#define _TDCHART_H_

#include <TWidget/TWidget.h>
#include <TWidget/TWidget_dev.h>
#include "tdAxis.h"
#include "struct.h"
#include "showData.h"
#include <tdCairo.h>
#include <math.h>
#include <TWidget/cs_text.h>


#define pi 3.14159265358979323846

typedef struct {
	TWidget widget;
	TDisplayCell *vc;//整个控件
    TDisplayCell *axis_vc;//坐标轴以及数据
    TDisplayCell *caption_vc;
    TwStyle *style;
    int axis_type;
    cairo_surface_t *surface;
    cairo_surface_t *data_surface;//绘制数据
    cairo_surface_t *his_surface;
    cairo_surface_t *legend_surface;//绘制图例
    cairo_surface_t *tag_surface;//绘制标注
    cairo_surface_t *grid_surface;//绘制网格
    cairo_surface_t *axis_surface;//绘制轴
    cairo_surface_t *scale_surface;//绘制刻度
    cairo_surface_t *his_tag_surface;//柱状图专用刻度
    cairo_t *cr;
    cairo_t *his_cr;
    cairo_t *data_cr;
    cairo_t *legend_cr;
    cairo_t *tag_cr;
    cairo_t *grid_cr;
    cairo_t *axis_cr;
    cairo_t *scale_cr;
    cairo_t *his_tag_cr;
    Tbool axis_ok;
    CAIRO_INFO cairo_info;
    int surface_w;
    int surface_h;
    int data_groups;
    TwDataNode *root_nodes[16];
    TDisplayCell *dc_x[16];
    TDisplayCell *dc_y[16];
    TDisplayCell *dc_y2[16];
    TDisplayCell *legend_dc[16];
    TDisplayCell *pie_legend_dc[16];
    TDisplayCell *data_dc[16];
    TDisplayCell *x_cap_dc;
    TDisplayCell *y_cap_dc;
    TDisplayCell *y2_cap_dc;
    int root_nodes_num;
    int x_dc_num;
    int y_dc_num;
    int y2_dc_num;
    char his_pos[16];
    char *caption;
    TwStyle *axis_vc_style;
    int value;
    TWidget *btn;
    TDisplayCell *test;
} TD_CHART;
#define TW_CHART TStringID("chart") 




int TwChartGetFrameType(TWidget *widget);//获取坐标轴/图表类型
void TwChartShowGrid(TWidget *widget, Tbool if_show);//显示/隐藏网格
void TwChartSetDM(TWidget *chart, TwDataNode *show_root, T_ID show_root_type);//将数据与控件绑定并显示
void TwChartSetType(TWidget *widget, CHART_TYPE type);//设置图表为直角坐标还是饼图等

void TwChartSetScaleString(TWidget *widget, SCALE_TYPE scale_type, char **content, int num);
void TwChartSetScaleInt(TWidget *widget, SCALE_TYPE scale_type, int scales[], int num);
void TwChartSetScaleDouble(TWidget *widget, SCALE_TYPE scale_type, double scales[], double num);

void TwChartShowTag(TWidget *widget, Tbool show_hide);
void TwChartShowLegend(TWidget *widget, Tbool show_hide);
void TwChartShowGrid(TWidget *widget, Tbool if_show);
void TwChartShowScale(TWidget *widget, Tbool if_show);

void TwChartChangeFrame(TWidget *widget, CHART_TYPE type, T_ID show_node_type);

void TwChartSetLegendText(TWidget *widget, TwDataNode *root_node, char *text);
void TwChartSetBackgroundColor(TWidget *widget, TColor color);
void TwChartSetDefaultColors(TWidget *widget, char **colors, int num);
Tintp TwChartGetValue(TWidget *widget);

#endif