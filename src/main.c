#include "pebble.h"
#include "libs/pebble-assist.h"
  
#ifdef PBL_COLOR
  #include "gcolor_definitions.h"
#endif

static Window *s_main_window;
static Layer *s_time_layer, *s_battery_layer, *s_bluetooth_layer, *s_enemy_layer;
static BitmapLayer *s_background_layer, *s_poke1_layer, *s_poke2_layer;
static GBitmap *s_background_bitmap, *s_poke1_bitmap, *s_poke2_bitmap;
static TextLayer *s_time_label, *s_day_label, *s_date_label;

static char s_day_buffer[] = "XXXXXXXXX"; 
static char s_date_buffer[] ="XX/XXX";  
static char s_time_buffer[] = "00:00";
static GFont s_reg_font, s_time_font;


static void bluetooth_color_proc(Layer *layer, GContext *ctx) {
#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorGreen);
#else
  graphics_context_set_fill_color(ctx, GColorBlack);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(layer), 2, GCornersAll);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *t = localtime(&temp); 

  if(clock_is_24h_style() == true) {
    print_time(s_time_buffer, "%H:%M", t);
  } else {
    print_time(s_time_buffer, "%I:%M", t);
  }
  text_layer_set_text(s_time_label, s_time_buffer);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  print_time(s_day_buffer, "%A", t);
  print_time(s_date_buffer, "%d/%b", t);
  
  text_layer_set_text(s_day_label, s_day_buffer); 
  text_layer_set_text(s_date_label, s_date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(); 
}

static void update_battery(Layer *layer, GContext *ctx) {
  int bat = (battery_state_service_peek().charge_percent) / 10;
  if (bat == 0) {
    bat = 1;
  }
  
  GRect box_frame = GRect(0, 0,((bat * 5)), 4);
  s_battery_layer = layer_create(box_frame);
#ifdef PBL_COLOR
  if (bat >= 6) {
  graphics_context_set_fill_color(ctx, GColorGreen);
  }
  else if (bat >=3) {
    graphics_context_set_fill_color(ctx, GColorYellow);
  }
  else {
    graphics_context_set_fill_color(ctx, GColorRed);
  }
#else
  graphics_context_set_fill_color(ctx, GColorBlack);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(s_battery_layer), 2, GCornersAll);
}

void update_bluetooth(bool connected) {
  if (connected) {
    layer_show(s_enemy_layer);
  }
  else {
    layer_hide(s_enemy_layer);
  }
}

static void window_load(Window *window) {
  
  GRect bounds = window_get_bounds(window);
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_POKEMON_REG_12));
  s_reg_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_POKEMON_BOLD_10));
  
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  s_poke1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_POKE1);
  s_poke2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_POKE2);

  s_background_layer = bitmap_layer_create(bounds);
  s_poke1_layer = bitmap_layer_create(GRect(0, 56, 55, 56));
  s_poke2_layer = bitmap_layer_create(GRect(88, 3, 56, 56));
  s_time_layer = layer_create(bounds);
  s_battery_layer = layer_create(GRect(82,87,144,168));
  s_bluetooth_layer = layer_create(GRect( 22, 22, 50,4));
  s_enemy_layer = layer_create(bounds);
  s_time_label = text_layer_create(GRect(7, 5, 130, 95));
  s_day_label = text_layer_create(GRect(10, 73, 130, 95));
  s_date_label = text_layer_create(GRect(10, 93, 130, 95));
  
  text_layer_set_text(s_time_label, "Time"); 
  text_layer_set_text(s_day_label, "Day"); 
  text_layer_set_text(s_date_label, "Date");
  
  layer_set_update_proc(s_bluetooth_layer, bluetooth_color_proc);
  layer_set_update_proc(s_battery_layer, update_battery);
  layer_set_update_proc(s_time_layer, date_update_proc);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_bitmap(s_poke1_layer, s_poke1_bitmap);
  bitmap_layer_set_bitmap(s_poke2_layer, s_poke2_bitmap);
  
  text_layer_set_colors(s_time_label, GColorBlack, GColorClear);
  text_layer_set_colors(s_day_label, GColorBlack, GColorClear);
  text_layer_set_colors(s_date_label, GColorBlack, GColorClear);
  
  text_layer_set_font(s_time_label, s_time_font); 
  text_layer_set_font(s_date_label, s_reg_font);
  text_layer_set_font(s_day_label, s_reg_font);
  
  text_layer_set_text_alignment(s_time_label, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_date_label, GTextAlignmentRight);
  text_layer_set_text_alignment(s_day_label, GTextAlignmentRight);
  
  bitmap_layer_add_to_window(s_background_layer, window);
  bitmap_layer_add_to_window(s_poke1_layer, window);
  layer_add_to_window(s_battery_layer, window);
  layer_add_to_window(s_time_layer, window);
  layer_add_to_window(s_enemy_layer, window);
  layer_add_to_layer(s_bluetooth_layer, s_enemy_layer);
  bitmap_layer_add_to_layer(s_poke2_layer, s_enemy_layer);
  text_layer_add_to_layer(s_time_label, s_time_layer);
  text_layer_add_to_layer(s_date_label, s_time_layer);
  text_layer_add_to_layer(s_day_label, s_time_layer);
  
  update_time();
}

static void window_unload(Window *s_main_window) {
  bitmap_layer_destroy_safe(s_poke1_layer);
  bitmap_layer_destroy_safe(s_poke2_layer);
  bitmap_layer_destroy_safe(s_background_layer);
  
  layer_destroy_safe(s_battery_layer);
  layer_destroy_safe(s_time_layer);
  layer_destroy_safe(s_enemy_layer);
  layer_destroy_safe(s_bluetooth_layer);

  text_layer_destroy_safe(s_time_label);
  text_layer_destroy_safe(s_date_label);
  text_layer_destroy_safe(s_day_label);
  
  gbitmap_destroy_safe(s_poke1_bitmap);
  gbitmap_destroy_safe(s_poke2_bitmap);
  gbitmap_destroy_safe(s_background_bitmap);
  
  fonts_unload_custom_font(s_reg_font);
  fonts_unload_custom_font(s_time_font);
}


static void init() {
  s_main_window = window_create(); 
  window_handlers(s_main_window, window_load, window_unload);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  bluetooth_connection_service_subscribe(update_bluetooth);
  
  window_stack_push(s_main_window, true);

}

static void deinit() {
  tick_timer_service_unsubscribe();
  
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}