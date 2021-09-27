#include "tdChart.h"
#include <math.h>

//销毁用于刻度以及用于图例的dc
void destroy_extra_dc(void *obj)
{
    TD_CHART *chart= (TD_CHART*)obj;
    for(int i = 0; i < 16; i++)
	{
		if(chart->dc_x[i] != NULL )
			DcDestroy(chart->dc_x[i]);
		if(chart->dc_y[i] != NULL)
			DcDestroy(chart->dc_y[i]);
		if(chart->dc_y2[i] != NULL)
			DcDestroy(chart->dc_y2[i]);
		if(chart->legend_dc[i] != NULL )
			DcDestroy(chart->legend_dc[i]);
	}
}

/**
 * func:将rgba字符串转化为数字并输出double，范围0-1
 * args：rgba:需要转换的字符串
 *         which_one:需要输出的值：1：r
 *                              2：g
 *                              3：b
 *                              4：a
 * */
double trans_rgba(char *rgba, int which_one)
{
    char *temp_str = malloc(1024);
    char *num = malloc(32);
    bzero(temp_str, 1024);
    bzero(num, 32);
    sprintf(temp_str, "%s", rgba);
    int i = 0;
    int temp1 =-1, temp2 = -1, comma_num = 0;
    while(i < strlen(temp_str))
    {
        if(temp_str[i] == ',')
        {
            temp1 = temp2;
            temp2 = i;
            comma_num++;
            if(comma_num == which_one)
                break;
        }
        i++;
    }
    int else_char = 0;
    if(i < strlen(temp_str))
    {
        for(int j = 0; j < temp2-temp1-1; j++)
        {
            if((temp_str[j+temp1+1] >= '0' && temp_str[j+temp1+1] <= '9') || temp_str[j+temp1+1] == '.')
                num[j-else_char] = temp_str[j+temp1+1];
            else 
                else_char++;
        }
    }
    else
    {
        for (int j = 0; j < strlen(temp_str)-temp2; j++)
        {
            
            if((temp_str[j+temp2+1] >= '0' && temp_str[j+temp2+1] <= '9') || temp_str[j+temp2+1] == '.')
            {
                num[j-else_char] = temp_str[j+temp2+1];
            }
            else 
                else_char++;
        }
        
    }
    double value;
    value = atof(num)/255.0;
    free(temp_str);
    free(num);
    return value;
}

//判断字符是否是中文
int is_zh_ch(char c)
{
    if(~(c >> 8) == 0)
        return 1;
    else return 0;
}

// char *my_itoa(int num)
// {
//     char *a;
//     printf("-1\n");
//     char str[256] = {0};
//     printf("-2\n");
//     sprintf(str, "%d", num);
//     printf("-3\n");
//     a = str;
//     printf("-4\n");
//     return a;
// }
int my_itoa(int val, char* buf)

{
    const unsigned int radix = 10;
    char* p;
    unsigned int a; //every digit
    int len;
    char* b; //start of the digit char
    char temp;
    unsigned int u;
    p = buf;
    if (val < 0)
    {
        *p++ = '-';
        val = 0 - val;
    }
    u = (unsigned int)val;
    b = p;
    do
    {
        a = u % radix;
        u /= radix;
        *p++ = a + '0';
    } while (u > 0);
    len = (int)(p - buf);
    *p-- = 0;
    //swap
    do
    {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;
    } while (b < p);

    return len;
}

//计算是第几组数据
int get_rank_of_rootnode(void *obj, TwDataNode *node)
{
    TD_CHART *chart= (TD_CHART*)obj;
    for(int i = 0; i < chart->data_groups; i++)
    {
        if(chart->root_nodes[i] == node)
            return i;
    }
}

//判断是不是柱状图
Tbool judge_showtype_if_is_his(TwDataNode *root_node)
{
    T_ID dm_type = TwDMGetRootType(TwDMGetNodeDM(root_node));
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
    
    if(TTableGetTable(info_table, TwDMGetNodeType(TwDMGetChildNode(root_node, TRUE))) != NULL)
    {
        node_info_table = TTableGetTable(info_table, TwDMGetNodeType(TwDMGetChildNode(root_node, TRUE)));
    }
    char *show_type = TTableGetString(node_info_table, TStringID("show_type"), NULL);
    if(strcmp(show_type,"histogram") == 0)
    {
        return TRUE;
    }
    else
        return FALSE;
}

//得到指定位置的节点
TwDataNode *get_child_node(TwDataNode *root_node, int num)
{
    TwDataNode *node;
    node = TwDMGetChildNode(root_node, TRUE);
    for(int i = 0; i < num; i++)
    {
        node = TwDMGetSiblingNode(node, TRUE);
    }
    return node;
}

//排序算法
int compare_int(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

int compare_double(const void *a, const void *b)
{
    return (int)(*(double *)a - *(double *)b);
}

//判断处于坐标轴的第几个刻度
int get_pos_of_scale_int(int x, int x_scales[], int max_num)
{
    int i;
    for(i = 0; i < max_num; i++)
    {
        if(x == x_scales[i])
            break;
    }
    return i;
}

int get_pos_of_scale_double(double x, double x_scales[], int max_num)
{
    int i;
    for(i = 0; i < max_num; i++)
    {
        if((x/x_scales[i])>0.99999 && (x/x_scales[i]) < 1.00001)
            break;
    }
    return i;
}

void deal_y_scale_int(void *obj, int scale_num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    int num,temp;
    int *a = chart->cairo_info.scale_int_info.y_scale;
    int max = a[scale_num-1];
    num = 0;
    while(max)
    {
        max = max/10;
        if(max)
        {
            num++;
            temp = max;
        }
    }
    if(num == 0)
    {
        chart->cairo_info.scale_int_info.y_end = 10;
        chart->cairo_info.scale_int_info.y_start = 1;
    }
    else
    {
        chart->cairo_info.scale_int_info.y_end = (int)pow(10, num)*(temp+1);
        chart->cairo_info.scale_int_info.y_start = chart->cairo_info.scale_int_info.y_end/10;
    }
    for(int i = 0; i < 10; i++)
    {
        chart->cairo_info.scale_int_info.y_scale[i] = chart->cairo_info.scale_int_info.y_start*(i+1);
    }

}

void deal_y_scale_double(void *obj, int scale_num)
{
    TD_CHART *chart= (TD_CHART*)obj;
    double *a = chart->cairo_info.scale_double_info.y_scale;
    double max = a[scale_num-1];
    int num = 0;
    double temp = 1.0;
    if(max > 1)
    {
        while(max > 1)
        {
            max = max/10.0;
            if(max >= 1.0)
            {
                num++;
                temp = max;
            }
        }
        if(num == 0)
        {
            chart->cairo_info.scale_double_info.y_end = 10.0;
            chart->cairo_info.scale_double_info.y_start = 1.0;
        }
        else
        {
            chart->cairo_info.scale_double_info.y_end = pow(10, num)*(temp+1);
            chart->cairo_info.scale_double_info.y_start = chart->cairo_info.scale_double_info.y_end/10.0;
        }
    }
    else
    {
        while(max < 1.0)
        {
            max = max*10.0;
            if(max <= 1.0)
            {
                num--;
                temp = max;
            }
        }
        if(num == 0)
        {
            chart->cairo_info.scale_double_info.y_end = 1.0;
            chart->cairo_info.scale_double_info.y_start = 0.1;
        }
        else
        {
            chart->cairo_info.scale_double_info.y_end = pow(10, num)*(temp+0.1);
            chart->cairo_info.scale_double_info.y_start = chart->cairo_info.scale_double_info.y_end/10.0;
        }
    }
    for(int i = 0; i < 10; i++)
    {
        chart->cairo_info.scale_double_info.y_scale[i] = chart->cairo_info.scale_double_info.y_start*(i+1);
    }
}
