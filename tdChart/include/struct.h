#ifndef _TDSTRUCT_H_
#define _TDSTRUCT_H_

typedef enum chart_type {
    TYPE_COORDINATES = 0x00,//普通直角坐标图
    TYPE_PIE         = 0x01,//饼状图
    TYPE_ECHARTS     = 0x02,//双轴图
    TYPE_RADAR       = 0x03,//雷达图
    TYPE_FUNNEL      = 0x04,//漏斗图
} CHART_TYPE;

//设定刻度时的轴的类型
typedef enum scale_type {
    SCALE_TYPE_X      = 0x00,//x轴
    SCALE_TYPE_Y     = 0x01,//y轴
    SCALE_TYPE_Y2     = 0x02,//y2轴
    
} SCALE_TYPE;

//以下两个结构体用于保存确定数据类型时的刻度数值
typedef struct scale_info_int{
    int x_scale[16];
    int y_scale[16];
    int y_start;
    int y_end;
    int y2_scale[16];
    int y2_start;
    int y2_end;

} SCALE_INFO_INT;

typedef struct scale_info_double{
    double x_scale[16];
    double y_scale[16];
    double y_start;
    double y_end;
    double y2_scale[16];
    double y2_start;
    double y2_end;
} SCALE_INFO_DOUBLE;

typedef struct scale_info_string{
    char x_string[16][32];
    char y_string[16][32];
    char y2_string[16][32];
} SCALE_INFO_STRING;


typedef struct {
    Tbool if_show_grid;//是否显示网格
    Tbool if_show_tag;
    Tbool if_show_scale;
    Tbool if_show_legend;
    double origin_x;
    double origin_y;
    double axis_w;
    double axis_h;
    double pie_r;
    double x_unit;//刻度每个单元长度
    double y_unit;
    double y2_unit;
    int scale_font_size;//刻度字体大小
    int x_data_type;//0:int;1:double;2:str
    int y_data_type;
    int y2_data_type;
    int his_num;//绘制的图的批次
    int line_num;
    int point_num;
    int x_scale_num;//轴分为多少刻度
    int y_scale_num;
    int y2_scale_num;
    int legend_num;//图例数量
    int node_num;//数据节点批次
    SCALE_INFO_INT scale_int_info;//刻度范围信息
    SCALE_INFO_DOUBLE scale_double_info;
    SCALE_INFO_STRING scale_string_info;
    char *colors_for_pie[16];;
    int x_scale_cap_int[16];
    double x_scale_cap_double[16];
    char* x_scale_cap_str[16];
    int y_scale_cap_int[16];
    double y_scale_cap_double[16];
    char* y_scale_cap_str[16];
    int x_set_ok;
    int y_set_ok;
    int y2_set_ok;
} CAIRO_INFO;

typedef enum show_type{
    SHOW_TYPE_HISTOGRAM = 0x00,//柱状图
    SHOW_TYPE_LINE      = 0x01,//折线图
    SHOW_TYPE_POINT     = 0x02,//散点图
}SHOW_TYPE;

typedef struct data_node{
    struct data_node *parent_node;
    struct data_node *child_node;
    struct data_node *last_node;
    struct data_node *next_node;
    SHOW_TYPE show_type;
    TTable *value;
}  TwChartNode;

typedef struct data_model{
    int chart_type_num;
    SHOW_TYPE show_type_1;
    SHOW_TYPE show_type_2;
    TwChartNode *root_node;
    CHART_TYPE chart_type;
    CAIRO_INFO cairo_info;
} TwChartDM;



#endif