#include "test.h"


static TwMethod chart_method = {
	// .event_handler = chart_event_handler,
	// .set_value = chart_set_value,
	// .change_style = chart_change_style,
};


// void _set_default_cairo_info(void *obj){
// 	TD_TEST *chart= (TD_TEST*)obj;
// 	chart->cairo_info.scale_font_size = 10;
// 	chart->cairo_info.if_show_grid = FALSE;
// }

cairo_surface_t *cairo_create_x11_surface0(int x, int y)
{
   Display *dsp;
   Drawable da;
   int screen;
   cairo_surface_t *sfc;

   if ((dsp = XOpenDisplay(NULL)) == NULL)
      exit(1);
   screen = DefaultScreen(dsp);
   da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, x, y, 0, 0, 0);
   XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
   XMapWindow(dsp, da);
	
   sfc =gui_cairo_create_x11_surface(&x, &y, 0);
//    cairo_xlib_surface_set_size(sfc, x, y);

   return sfc;
}

static int test_obj_create(void *obj, TTable *in)
{
	TDisplayCell *parent_vc;
	TD_TEST *chart= (TD_TEST*)obj;
	TWidget *widget = (TWidget*)obj;

	TwLock();

	parent_vc = _TwInit(widget, &chart_method, in);
	if(parent_vc == NULL) {
		TwUnlock();
		return T_FAIL;
	}

	widget->w = 500;
	widget->h = 500;

	chart->vc = DcCreate(DC_WINDOW, parent_vc, widget->cx, widget->cy, widget->w, widget->h, widget);

    cairo_surface_t *sfc;
    cairo_t *ctx;
    sfc = cairo_create_x11_surface0(500, 500);
    ctx = cairo_create(sfc);
    cairo_set_source_rgb(ctx, 1, 1, 1);
    cairo_paint(ctx);
    cairo_move_to(ctx, 20, 20);
    cairo_line_to(ctx, 200, 400);
    cairo_line_to(ctx, 450, 100);
    cairo_line_to(ctx, 20, 20);
    cairo_set_source_rgb(ctx, 0, 0, 1);
    cairo_fill_preserve(ctx);
    cairo_set_line_width(ctx, 5);
    cairo_set_source_rgb(ctx, 1, 1, 0);
    cairo_stroke(ctx);
	// TwChartSetScaleInt(chart, SCALE_TYPE_Y, 0, 100, 10);
	// DcSetExposeFunc(chart->vc, slider_text_expose);
	_TwSetFocusVC(widget, chart->vc);

	_TwInit_Finish(widget);

	TwUnlock();
	return T_SUCCESS;
}
static void test_obj_destroy(void *obj)
{
	TD_TEST *chart = (TD_TEST*)obj;
	TwLock();
	_TwUnInit((TWidget*)obj);
	DcDestroy(chart->vc);
	TwUnlock();
	return ;
}



static TObjectType test_obj_info = {
	.create_func = test_obj_create,
	.destroy_func = test_obj_destroy,
	.obj_size = sizeof(TD_TEST),
	.interface_num = 1,
	.interfaces = {None},
};
static TwStyleNode test_style[] = {
	{
	.status_mask = TW_STATUS_NORMAL|TW_STATUS_DISABLE|TW_STATUS_ACTIVE|TW_STATUS_SELECT,
	.flags_mask = HAS_TEXT_COLOR,
	.text_color = T_BGR(255, 255, 255),
	.text_bg_color = T_BGR(128, 128, 128),
	}
};

static void init_test(void) __attribute__((constructor));
void init_test(void)
{
	test_obj_info.interfaces[0] = T_STRING_ID(widget);
	TObjectRegisterType(TW_TEST, &test_obj_info);
	_TwRegisterStyle(TW_CHART, T_ARRAY_NUM(test_style), test_style);
	return;
}