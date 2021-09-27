
#include <TWidget/TWidget.h>
#include <TWidget/TWidgetEx.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "tdChart.h"
#include"test.h"
TWidget *main_win;
TWidget *chart;


void signal_handler(int signo)
{
    signal(signo, signal_handler);
    switch (signo)
    {
    case SIGTERM:
        printf("Process recieve SIGTERM exit\n");    
        /*程序自己退出，或shell里调用kill缺省该进程。该信号可以被阻塞，或被处理
        可以在这里做一些程序退出前的最后处理工作*/
        exit(0);
        break;
    }

}
int click_time;
int click_time1;
int click_time2;
int click_time3;

#if 1
void click_func(void *obj, T_ID event, TTable *in, void *arg, TExist *exist)
{
    
    if(click_time%2 == 0)
    {
        TwChartShowGrid(chart, FALSE);
    }
    else
    {
        TwChartShowGrid(chart, TRUE);
    }
    click_time++;
}
void click_func1(void *obj, T_ID event, TTable *in, void *arg, TExist *exist)
{
    
    if(click_time1%2 == 0)
    {
        TwChartShowLegend(chart, FALSE);
    }
    else
    {
        TwChartShowLegend(chart, TRUE);
    }
    click_time1++;
}
void click_func2(void *obj, T_ID event, TTable *in, void *arg, TExist *exist)
{
    
    if(click_time2%2 == 0)
    {
        TwChartShowTag(chart, FALSE);
    }
    else
    {
        TwChartShowTag(chart, TRUE);
    }
    click_time2++;
}
void click_func3(void *obj, T_ID event, TTable *in, void *arg, TExist *exist)
{
    if(click_time3%2 == 0)
    {
        TwChartShowScale(chart, FALSE);
    }
    else
    {
        TwChartShowScale(chart, TRUE);
    }
    click_time3++;
}
void click_func4(void *obj, T_ID event, TTable *in, void *arg, TExist *exist)
{
    TwChartChangeFrame(chart, TYPE_RADAR, TStringID("node6"));
    TwUpdateShow(); 
}

void click_fun5(void *obj, T_ID event, TTable *in, void *arg, TExist *exist)
{
    printf("aaaaa\n");
    // TwChangeStyle(chart, "test2");
    // TwMove(chart, 24, 0);
    TwChartChangeFrame(chart, TYPE_COORDINATES, TStringID("node1"));
}

int get_rand(int start, int end)
{
    int a;
    // srand((unsigned)time(NULL));
    
    a = rand()%(end-start+1)+start;
    return a;
}

void create_data(T_ID type)
{
    TwDataModel *dm;
    dm = TwDMCreate(TStringID("dm"));

    TwDataNode *root_node = TwDMGetRootNode(dm);
    TwDataNode *node;
    TTable *table;
    char *item[6] = {"智力", "敏捷", "力量", "成长", "韧性", "体力"};
    
    TwDMLock(dm); 
        for(int i = 1; i <= 6; i++)
        {
            node = TwDMAddNode(root_node, type);
            table = TwDMGetNodeValue(node, TRUE);
            int a = rand()%100;
            // printf("aaaaaaa:%d\n", a);
            TTableAddInt(table, TStringID("x"), i);
            // TTableAddString(table, TStringID("x"), item[i-1], -1);
            // TTableAddFloat(table, TStringID("y"), 1.0*a);
            TTableAddInt(table, TStringID("y"), a);
            double c = 255.0/6*i;
            char temp[64] = {0};
            sprintf(temp, "%.2lf,%.2lf,%.2lf,%.2lf", c, c, c, c);
            // TTableAddString(table, TStringID("item"), item[i-1], -1);
        }

    TwDMUnlock(dm);
    if(type == TStringID("node1") || type == TStringID("node5") || type == TStringID("node4"))
        TwChartSetLegendText(chart, root_node, "马龙");
    else if(type == TStringID("node1_2") || type == TStringID("node6"))
        TwChartSetLegendText(chart, root_node, "许昕");
    TwChartSetDM(chart, root_node, TStringID("dm"));

}
#endif
int main(int argc, const char *argv[])
{ 
    TWidget *root;
    root = TwAppInit();
    signal(SIGTERM, &signal_handler);
	click_time = 0;
    click_time1 = 0;
    click_time2 = 0;
    click_time3 = 0;
    /*读取配置文件config.ini*/
    srand((int)time(NULL));
    TwConfigParseFile(TGetExecutePath("../etc/config.ini"));
    main_win = TObjectGetFromName("main_win");
    TWidget *btn = TwCreate(TW_BUTTON, main_win, 600,100,126, 52, NULL, None);
    TObjectAddEventHandler(btn, CLICKED, click_func, NULL);
    TWidget *btn1 = TwCreate(TW_BUTTON, main_win, 600,162,126, 52, NULL, None);
    TObjectAddEventHandler(btn1, CLICKED, click_func1, NULL);
    TWidget *btn2 = TwCreate(TW_BUTTON, main_win, 600,224,126, 52, NULL, None);
    TObjectAddEventHandler(btn2, CLICKED, click_func2, NULL);
    TWidget *btn3 = TwCreate(TW_BUTTON, main_win, 600,286,126, 52, NULL, None);
    TObjectAddEventHandler(btn3, CLICKED, click_func3, NULL);
    TWidget *btn4 = TwCreate(TW_BUTTON, main_win, 600,338,126, 52, NULL, None);
    TObjectAddEventHandler(btn4, CLICKED, click_func4, NULL);
    chart = TwCreate(TW_CHART, main_win, 0, 0, 1000, 500, NULL, None);
    // chart = TObjectGetFromName("chart");
    char *colors[3]; //= {"25,1,67", "6,5,14", "1,1,1"};
    char *color1 = "25,1,67";
    char *color2 = "6, 5, 14";
    char *color3 = "1,1,1";
    colors[0] = color1;
    colors[1] = color2;
    colors[2] = color3;
    TwChartSetDefaultColors(chart, colors, 3);
    TObjectAddEventHandler(chart, CLICKED, click_fun5, NULL);
    
    //create_data(TStringID("node2"));
    TwChartSetType(chart, TYPE_COORDINATES);
    // TwChartSetType(chart, TYPE_RADAR);
    // TwChartSetScaleInt(chart, SCALE_TYPE_X, 0, 6, 1);
    char *item[6] = {"智力", "敏捷", "力量", "成长", "韧性", "体力"};
    int x_scale[6] = {1,2,3,4,5,6};
    int y_scale[10] = {10,20,30,40,50,60,70,80,90,100};
    TwChartSetScaleInt(chart, SCALE_TYPE_X, x_scale, 6);
    TwChartSetScaleInt(chart, SCALE_TYPE_Y, y_scale, 10);
    // TwChartSetType(chart, TYPE_RADAR);
    create_data(TStringID("node1"));
    // create_data(TStringID("node5"));
    // create_data(TStringID("node6"));
    // create_data(TStringID("node1"));
    // create_data(TStringID("node1"));
    // create_data(TStringID("node1_2"));
    // TwChartChangeFrame(chart, TYPE_PIE, TStringID("node4"));
    // TwChartChangeFrame(chart, TYPE_COORDINATES, TStringID("node1"));
    // int window_id = TwGetXWindow(chart);
    // printf("window id is :%d\n", window_id);
    // TXResizeWindow(tw_display, window_id, 20, 20);
    // TwChartSetBackgroundColor(chart, T_RGB(0,255,0));
    
    TwUpdateShow(); 
    TwMainLoop();

    return 0;
}
