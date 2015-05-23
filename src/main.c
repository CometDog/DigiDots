#include "pebble.h"
#ifdef PBL_COLOR
  #include "gcolor_definitions.h"
#endif
#include "libs/pebble-assist.h"
#include "elements.h"

static void update_bg(Layer *layer, GContext *ctx) {
  X = 19;
  column = 0;
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorDarkGray);
  #else
    graphics_context_set_stroke_color(ctx, GColorWhite);
  #endif
  
  row[0] = 1;
  row[1] = 9;
  row[2] = 5;
  row[3] = 9;
  row[4] = 5;
  row[5] = 9;
  
  if (clock_is_24h_style() == true) {
    row[0] = 2;
    row[1] = 4;
  }
  
  while (column < 6) {
    Y = 125;
    while (row[column] > 0) {
      #ifdef PBL_COLOR
        graphics_fill_circle(ctx, GPoint(X,Y), 5);
      #else
        graphics_draw_circle(ctx, GPoint(X,Y), 5);
      #endif
      Y -= 15;
      row[column] -= 1;
    }
    column += 1;
    if (column % 2 == 0) {
      X += 30;
    }
    else {
      X += 15;
    }
  }
}

static void update_time(Layer *layer, GContext *ctx) {
  time_t epoch = time(NULL); 
  struct tm *t = localtime(&epoch);
  #ifdef PBL_COLOR
    bat = battery_state_service_peek().charge_percent / 10;
  #endif
  
  hour_24 = t->tm_hour;
  
  if (hour_24 >= 22 || hour_24 <= 6) {
    sleep = true;
  }
  else {
    sleep = false;
  }
  
  if (clock_is_24h_style() == true) {
    hour = hour_24;
  }
  else {
    hour = hour_24 % 12;
  }
  
  if (clock_is_24h_style() == false && hour == 12) {
    hour1 = 1;
    hour2 = 2;
  }
  else {
    hour1 = hour / 10;
    hour2 = hour - (hour1 * 10);
  }
  minute1 = t->tm_min / 10;
  minute2 = t->tm_min - (minute1 * 10);
  
  second1 = t->tm_sec / 10;
  second2 = t->tm_sec - (second1 * 10);
  
  row[0] = hour1;
  row[1] = hour2;
  row[2] = minute1;
  row[3] = minute2;
  row[4] = second1;
  row[5] = second2;
  
  X = 19;
  column = 0;
  #ifdef PBL_COLOR
  if (bat >= 7) {
  graphics_context_set_fill_color(ctx, GColorGreen);
  }
  else if (bat >= 4) {
    graphics_context_set_fill_color(ctx, GColorYellow);
  }
  else {
    graphics_context_set_fill_color(ctx, GColorRed);
  }
  #else
    graphics_context_set_fill_color(ctx, GColorWhite);
  #endif
  
  while (column < 6) {
    Y = 125;
    while (row[column] > 0) {
      graphics_fill_circle(ctx, GPoint(X,Y), 5);
      Y -= 15;
      row[column] -= 1;
    }
    column += 1;
    if (column % 2 == 0) {
      X += 30;
    }
    else {
      X += 15;
    }
  }
  
  if (sleep == true && first_run == true) {
    timer = app_timer_register(10 * 1000, timer_callback, NULL);
    first_run = false;
  }
}

static void update_sleep(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  
  graphics_fill_rect(ctx, GRect(0,0,144,168), GCornerNone, 0);
  
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorDarkGray);
  #else
    graphics_context_set_fill_color(ctx, GColorWhite);
  #endif
  
  graphics_fill_circle(ctx, GPoint(69,65), 64);
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  
  graphics_fill_circle(ctx, GPoint(38, 45), 48);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_time_layer);
}

static void bt_handler(bool connected) {
  if (connected) {
    vibes_short_pulse();
  }
  else {
    vibes_double_pulse();
  }
}

static void timer_callback(void *data) {
  if (sleep == true) {
    layer_show(s_sleep_layer);
  }
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  layer_hide(s_sleep_layer);
  if (sleep == true) {
    app_timer_cancel(timer);
    timer = app_timer_register(10 * 1000, timer_callback, NULL);
  }
}

static void main_window_load(Window *window) {
  GRect bounds = GRect(0,19,144,168);
  
  window_set_background_color(window, GColorBlack);
  
  s_background_layer = layer_create(bounds);
  s_time_layer = layer_create(bounds);
  s_sleep_layer = layer_create(bounds);
  layer_set_update_proc(s_background_layer, update_bg);
  layer_set_update_proc(s_time_layer, update_time);
  
  layer_set_update_proc(s_sleep_layer, update_sleep);
  layer_hide(s_sleep_layer);
  
  layer_add_to_window(s_background_layer, window);
  layer_add_to_window(s_time_layer, window);
  layer_add_to_window(s_sleep_layer, window);
}

static void main_window_unload(Window *window) {
  layer_destroy_safe(s_background_layer);
  layer_destroy_safe(s_time_layer);
  layer_destroy_safe(s_sleep_layer);
}

static void init() {
  s_main_window = window_create();
  window_handlers(s_main_window, main_window_load, main_window_unload);
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
  bluetooth_connection_service_subscribe(bt_handler);
}

static void deinit() {
  accel_tap_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  window_destroy_safe(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}