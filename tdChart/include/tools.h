#include <stdio.h>
void destroy_extra_dc(void *obj);
double trans_rgba(char *rgba, int which_one);
int is_zh_ch(char c);
int my_itoa(int val, char* buf);
int get_rank_of_rootnode(void *obj, TwDataNode *node);
Tbool judge_showtype_if_is_his(TwDataNode *root_node);
TwDataNode *get_child_node(TwDataNode *root_node, int num);
int compare_int(const void *a, const void *b);
int compare_double(const void *a, const void *b);
int get_pos_of_scale_int(int x, int x_scales[], int max_num);
int get_pos_of_scale_double(double x, double x_scales[], int max_num);
void deal_y_scale_int(void *obj, int scale_num);
void deal_y_scale_double(void *obj, int scale_num);