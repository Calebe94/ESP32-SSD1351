/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "../lvgl/lvgl.h"
#include "../lv_examples/lv_apps/demo/demo.h"
// #include "../lv_examples/lv_tests/lv_test_objx/lv_test_tileview/lv_test_tileview.h"
#include "esp_freertos_hooks.h"


// #include "../drv/disp_spi.h"ssd1306_drawBufferEx8
// #include "../drv/ili9341.h"
// #include "../drv/tp_spi.h"
// #include "../drv/xpt2046.h"

#include "ssd1306.h"
#include "lcd/oled_ssd1351.h"

static void IRAM_ATTR lv_tick_task(void);

static lv_obj_t *win = NULL;

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    int32_t x;
    int32_t y;
	printf("%s Parameters: (x1:%d) (y1:%d) (x2:%d) (y2:%d) ", __func__, x1, y1, x2, y2);
    for(y = y1; y <= y2; y++) 
	{
        for(x = x1; x <= x2; x++) 
		{
			ssd1306_putColorPixel8(x, y, !lv_color_to1(*color_p));
            color_p++;
        }
    }
    lv_flush_ready();
}

void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,  lv_color_t color) {
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    int32_t x;
    int32_t y;
	printf("%s Parameters: (x1:%d) (y1:%d) (x2:%d) (y2:%d) ", __func__, x1, y1, x2, y2);

    for(y = y1; y <= y2; y++) 
	{
        for(x = x1; x <= x2; x++) 
		{
          /* Put a pixel to the display.*/
          ssd1306_putColorPixel8(x, y, lv_color_to1(color));
        }
    }
}

void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p) {
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    int32_t x;
    int32_t y;
	printf("%s Parameters: (x1:%d) (y1:%d) (x2:%d) (y2:%d) ", __func__, x1, y1, x2, y2);

    for(y = y1; y <= y2; y++) {
        for(x = x1; x <= x2; x++) {
            /* Put a pixel to the display.*/
            ssd1306_putColorPixel8(x, y, lv_color_to1(*color_p));
            color_p++;
        }
    }
}

void lv_test_tileview_1(void)
{
    static const lv_point_t vp[] = {
                   {1,0},          /*First row: only the middle tile*/
            {0,1}, {1,1}, {1,2},   /*Second row: all tree tiles */
                   {2,1}, {2,2},   /*Third row: middle and right tile*/
            {LV_COORD_MIN, LV_COORD_MIN}};

    lv_obj_t * t;
    t = lv_tileview_create(lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(t, vp);
    lv_tileview_set_edge_flash(t, true);

    lv_obj_t * label;

    /*x0, y1 container*/
    lv_obj_t * p01 = lv_obj_create(t, NULL);
    lv_obj_set_click(p01, true);
    lv_obj_set_style(p01, &lv_style_pretty_color);
    lv_obj_set_size(p01, lv_obj_get_width(t), lv_obj_get_height(t));
    lv_tileview_add_element(p01);

    /*Add a  button at x0, y1*/
    lv_obj_t * b01 = lv_btn_create(p01, NULL);
    lv_tileview_add_element(b01);
    lv_obj_align(b01, NULL, LV_ALIGN_CENTER, 0, 50);
    label = lv_label_create(b01, NULL);
    lv_label_set_text(label, "Button");

    /*Add a label to indicate the position*/
    label = lv_label_create(p01, NULL);
    lv_label_set_text(label, "x0, y1");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /*x1, y1 container*/
    lv_obj_t * p11 = lv_obj_create(t, p01);
    lv_obj_align(p11, p01, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_tileview_add_element(p11);

    /*Add a label to indicate the position*/
    label = lv_label_create(p11, NULL);
    lv_label_set_text(label, "x1, y1");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /*x1, y2 list*/
    lv_obj_t * list12 = lv_list_create(t, NULL);
    lv_obj_set_size(list12, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(list12, p11, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_list_set_scroll_propagation(list12, true);
    lv_tileview_add_element(list12);

    lv_obj_t * list_btn;
    list_btn = lv_list_add(list12, NULL, "One", NULL);
    lv_tileview_add_element(list_btn);

    list_btn = lv_list_add(list12, NULL, "Two", NULL);
    lv_tileview_add_element(list_btn);

    list_btn = lv_list_add(list12, NULL, "Three", NULL);
    lv_tileview_add_element(list_btn);

    list_btn = lv_list_add(list12, NULL, "Four", NULL);
    lv_tileview_add_element(list_btn);

    list_btn = lv_list_add(list12, NULL, "Five", NULL);
    lv_tileview_add_element(list_btn);

    list_btn = lv_list_add(list12, NULL, "Six", NULL);
    lv_tileview_add_element(list_btn);

    list_btn = lv_list_add(list12, NULL, "Seven", NULL);
    lv_tileview_add_element(list_btn);

    /*x1, y0 container*/
    lv_obj_t * p10 = lv_obj_create(t, p01);
    lv_tileview_add_element(p10);

    /*Add a label to indicate the position*/
    label = lv_label_create(p10, NULL);
    lv_label_set_text(label, "x1, y0");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(p10, p11, LV_ALIGN_OUT_TOP_MID, 0, 0);

    /*x2, y1 container*/
    lv_obj_t * p21 = lv_obj_create(t, p01);
    lv_tileview_add_element(p21);
    lv_obj_align(p21, p11, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    /*Add a label to indicate the position*/
    label = lv_label_create(p21, NULL);
    lv_label_set_text(label, "x2, y1");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /*x2, y1 container*/
    lv_obj_t * p22 = lv_obj_create(t, p01);
    lv_tileview_add_element(p22);
    lv_obj_align(p22, p21, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    /*Add a label to indicate the position*/
    label = lv_label_create(p22, NULL);
    lv_label_set_text(label, "x2, y2");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Focus on a tile (the list)*/
    lv_tileview_set_tile_act(t, 1, 2, true);
}

void hello_world()
{
	/*Create a title label*/
	lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label, "Default buttons");
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

	/*Create a normal button*/
	lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
	lv_cont_set_fit(btn1, true, true); /*Enable resizing horizontally and vertically*/
	lv_obj_align(btn1, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	lv_obj_set_free_num(btn1, 1);   /*Set a unique number for the button*/
	// lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, btn_click_action);

	/*Add a label to the button*/
	label = lv_label_create(btn1, NULL);
	lv_label_set_text(label, "Normal");

	/*Copy the button and set toggled state. (The release action is copied too)*/
	lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn1);
	lv_obj_align(btn2, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	lv_btn_set_state(btn2, LV_BTN_STATE_TGL_REL);  /*Set toggled state*/
	lv_obj_set_free_num(btn2, 2);               /*Set a unique number for the button*/

	/*Add a label to the toggled button*/
	label = lv_label_create(btn2, NULL);
	lv_label_set_text(label, "Toggled");

	/*Copy the button and set inactive state.*/
	lv_obj_t * btn3 = lv_btn_create(lv_scr_act(), btn1);
	lv_obj_align(btn3, btn2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	lv_btn_set_state(btn3, LV_BTN_STATE_INA);   /*Set inactive state*/
	lv_obj_set_free_num(btn3, 3);               /*Set a unique number for the button*/

	/*Add a label to the inactive button*/
	label = lv_label_create(btn3, NULL);
	lv_label_set_text(label, "Inactive");
}

static void style_mod_cb(lv_style_t *style) {
#if LV_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_ORANGE;

    /*If not empty or has border then emphasis the border*/
    if(style->body.empty == 0 || style->body.border.width != 0) style->body.border.width = LV_DPI / 50;

    style->body.main_color = lv_color_mix(style->body.main_color, LV_COLOR_ORANGE, LV_OPA_70);
    style->body.grad_color = lv_color_mix(style->body.grad_color, LV_COLOR_ORANGE, LV_OPA_70);
    style->body.shadow.color = lv_color_mix(style->body.shadow.color, LV_COLOR_ORANGE, LV_OPA_60);

    style->text.color = lv_color_mix(style->text.color, LV_COLOR_ORANGE, LV_OPA_70);
#else
    style->body.border.opa = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_BLACK;
    style->body.border.width = 3;

#endif
}

static void GUI_MainMenu_Create(void) 
{
  lv_obj_t *label;

#if 1
  lv_group_t *group = lv_group_create(); /* create group for touchless navigation */
//   lv_indev_set_group(LV_GetInputDevice(), group); /* assign group to input device */

  /* change the default focus style which is an orangish thing */
  lv_group_set_style_mod_cb(group, style_mod_cb);

  /* create window */
  win = lv_win_create(lv_scr_act(), NULL);
  lv_win_set_title(win, "Main Menu");
  lv_win_set_btn_size(win, 8);

  /*Make the window content responsive*/
  lv_win_set_layout(win, LV_LAYOUT_PRETTY); /* this will arrange the buttons */

    /* Create SysMon menu buttons */
  lv_obj_t *btn1 = lv_btn_create(win, NULL);             /*Add to the active window */
  //lv_obj_set_pos(btn1, 5, 5);                                    /*Adjust the position*/
  lv_obj_set_size(btn1, 64, 18);
//   lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, Btn_SysMon_click_action);   /*Assign a callback for clicking*/
  label = lv_label_create(btn1, NULL);                  /*Put on 'btn1'*/
  lv_label_set_text(label, "SysMon");
  lv_group_add_obj(group, btn1);

  /* Create Demo menu buttons */
  lv_obj_t *btn2 = lv_btn_create(win, NULL);             /*Add to the active window */
//   lv_btn_set_action(btn2, LV_BTN_ACTION_CLICK, Btn_Demo_click_action);   /*Assign a callback for clicking*/
  lv_obj_set_size(btn2, 64, 18);
  label = lv_label_create(btn2, NULL);                  /*Put on 'btn1'*/
  lv_label_set_text(label, "Demo");
  lv_group_add_obj(group, btn2);

    /* Create Hardware menu buttons */
  lv_obj_t *btn3 = lv_btn_create(win, NULL);             /*Add to the active window */
//   lv_btn_set_action(btn3, LV_BTN_ACTION_CLICK, Btn_Hardware_click_action);   /*Assign a callback for clicking*/
  lv_obj_set_size(btn3, 64, 18);
  label = lv_label_create(btn3, NULL);                  /*Put on 'btn1'*/
  lv_label_set_text(label, "Hardware");
  lv_group_add_obj(group, btn3);
#endif
}

void app_main()
{
	lv_init();

	// disp_spi_init();
	// ili9341_init();

	// tp_spi_init();
    // xpt2046_init();
	ssd1351_128x128_spi_init(16, 5, 17);
    ssd1351_setMode( LCD_MODE_NORMAL );
    ssd1306_clearScreen16();
    ssd1306_negativeMode();
	printf("Ola mundo!\n");

	lv_disp_drv_t disp;
	lv_disp_drv_init(&disp);
	disp.disp_flush = my_disp_flush;
    disp.disp_fill = my_disp_fill;
	disp.disp_map = my_disp_map;
	lv_disp_drv_register(&disp);

    // lv_indev_drv_t indev;
    // lv_indev_drv_init(&indev);
    // indev.read = xpt2046_read;
    // indev.type = LV_INDEV_TYPE_POINTER;
    // lv_indev_drv_register(&indev);

	esp_register_freertos_tick_hook(lv_tick_task);

	// demo_create();
	// lv_test_tileview_1();
	// hello_world();

    lv_theme_mono_init(0 /* hue */,NULL /* use LV_FONT_DEFAULT */);
    lv_theme_set_current( lv_theme_get_mono() );
	
	GUI_MainMenu_Create();

	while(1) {
		vTaskDelay(1);
		lv_task_handler();
	}
}

static void IRAM_ATTR lv_tick_task(void)
{
	lv_tick_inc(portTICK_RATE_MS);
}
