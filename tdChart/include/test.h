#ifndef _TDTEST_H_
#define _TDTEST_H_

#include <TWidget/TWidget.h>
#include <TWidget/TWidget_dev.h>
#include "tdAxis.h"
#include "struct.h"
#include "showData.h"
#include <tdCairo.h>
#include <math.h>
//#include <TWidget/cs_text.h>
#include <tdCairo.h>
#include"test4.h"
// #include <cairo/cairo-xlib.h>


typedef struct {
	TWidget widget;
	TDisplayCell *vc;//整个控件
  
} TD_TEST;
#define TW_TEST TStringID("test") 

#endif
