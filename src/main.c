#include "pebble.h"
#include "pebble-assist.h"
  
#ifdef PBL_COLOR // Only use this for color
  #include "gcolor_definitions.h" // Allows the use of colors
#endif

static Window *s_main_window; // Main window
static Layer *s_solid_bg_layer, *s_time_layer, *s_battery_layer, *s_bluetooth_layer; // Solid color background and date layer
static BitmapLayer *s_background_layer, *s_poke1_layer, *s_poke2_layer; // Create face bitmap layer
static GBitmap *s_background_bitmap, *s_poke1_bitmap, *s_poke2_bitmap; // Create bitmap of face
static TextLayer *s_time_label, *s_day_label, *s_date_label, *s_month_label; // Create time layer and all the labels for it
static char s_day_buffer[] = "XXXXXXXXX"; // Create the buffer for the day
static char s_date_buffer[] ="XX/XXX"; // Create the buffer for the date number
static char s_month_buffer[] = "XXXX"; // Create the buffer for the star date
static char s_time_buffer[] = "00:00"; // Create a buffer for the time
static GFont s_reg_font, s_bold_font, s_time_font; // Create fonts

// Makes text uppercase when called
char *upcase(char *str)
{
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 0x20;
        }
    }

    return str;
}

// Update background when called
static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite); // Create fill color
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone); // Apply that color
}

// Update the time when called
static void update_time() {
  time_t temp = time(NULL); // Get the time
  struct tm *t = localtime(&temp); // Create the time structure

  if(clock_is_24h_style() == true) {
    print_time(s_time_buffer, "%H:%M", t);
  } else {
    print_time(s_time_buffer, "%I:%M", t);
  }
  text_layer_set_text(s_time_label, s_time_buffer); // Apply time to time layer
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL); // Get the time
  struct tm *t = localtime(&now); // Create the time structure

  print_time(s_day_buffer, "%A", t);
  print_time(s_date_buffer, "%d/%b", t);

  
  //upcase(s_day_buffer);
  //upcase(s_month_buffer);
  
  text_layer_set_text(s_day_label, s_day_buffer); // Apply to day label
  text_layer_set_text(s_date_label, s_date_buffer); // Apply to date number label
}

// Used to call the update time function
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(); // Updates time
}

static void update_battery(Layer *layer, GContext *ctx) {
  int bat = (battery_state_service_peek().charge_percent) / 10;
  
  GRect box_frame = GRect(0, 0,((bat * 5)), 5);
  s_battery_layer = layer_create(box_frame);
#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorGreen);
#else
  graphics_context_set_fill_color(ctx, GColorBlack);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(s_battery_layer), 0, GCornerNone);
}

void update_bluetooth(bool connected) {
  if (connected) {
    
  }
  else {
    
  }
}

// Loads the layers onto the main window
static void window_load(Window *window) {
  
  GRect bounds = window_get_bounds(window);
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_POKEMON_REG_12)); // Time font
  s_reg_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_POKEMON_BOLD_10));
  //s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_16)); // Date font
  //s_star_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_BOLD_25)); // Star font
  
  //s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND); // Set the bitmap resource

  s_solid_bg_layer = layer_create(bounds); // Create the solid color background layer
  s_background_layer = bitmap_layer_create(bounds); // Create the face layer
  s_poke1_layer = bitmap_layer_create(GRect(72, 5, 130, 95));
  s_poke2_layer = bitmap_layer_create(GRect(10, 75, 130, 95));
  s_time_layer = layer_create(bounds);
  s_battery_layer = layer_create(GRect(92,89,144,168));
  s_bluetooth_layer = layer_create(GRect( 20, 20, 50,5));
  s_time_label = text_layer_create(GRect(10, 5, 130, 95)); // Create the time layer
  s_day_label = text_layer_create(GRect(10, 75, 130, 95)); // Create the day label
  s_date_label = text_layer_create(GRect(10, 95, 130, 95)); // Create the star date label
  
  
/*#ifdef PBL_COLOR
  graphics_context_set_fill_color(s_bluetooth_layer, GColorGreen);
#else
  graphics_context_set_fill_color(s_bluetooth_layer, GColorBlack);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(s_bluetooth_layer), 0, GCornerNone);*/
  
  text_layer_set_text(s_time_label, "Time"); // Set the time layer to time buffer
  text_layer_set_text(s_day_label, "Day"); // Set day label to day buffer
  text_layer_set_text(s_date_label, "Date"); // Set date number label to date number buffer
  
  layer_set_update_proc(s_solid_bg_layer, bg_update_proc); // Update the solid color background
  layer_set_update_proc(s_battery_layer, update_battery);
  layer_set_update_proc(s_time_layer, date_update_proc); // Update the date layer
  //bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap); // Set the face bitmap to the background layer
  
  text_layer_set_colors(s_time_label, GColorBlack, GColorClear);
  text_layer_set_colors(s_day_label, GColorBlack, GColorClear);
  text_layer_set_colors(s_date_label, GColorBlack, GColorClear);
  
  /*#if PBL_PLATFORM_BASALT // Only set this for 3.0 +
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet); // Set the face layer to be a transparent png image
  #endif*/
  
  text_layer_set_font(s_time_label, s_time_font); // Set time font
  text_layer_set_font(s_date_label, s_reg_font);
  text_layer_set_font(s_day_label, s_reg_font);
  //text_layer_set_font(s_day_label, s_date_font); // Set day font
  //text_layer_set_font(s_num_label, s_date_font); // Set date number font
  //text_layer_set_font(s_star_label, s_star_font); // Set star date font
  
  text_layer_set_text_alignment(s_time_label, GTextAlignmentLeft); // Align time text
  text_layer_set_text_alignment(s_date_label, GTextAlignmentRight);
  text_layer_set_text_alignment(s_day_label, GTextAlignmentRight);
  
  layer_add_to_window(s_solid_bg_layer, window);
  layer_add_to_window(s_battery_layer, window);
  layer_add_to_window(s_bluetooth_layer, window);
  layer_add_to_window(s_time_layer, window);
  bitmap_layer_add_to_window(s_background_layer, window);
  text_layer_add_to_layer(s_time_label, s_time_layer);
  text_layer_add_to_layer(s_date_label, s_time_layer);
  text_layer_add_to_layer(s_day_label, s_time_layer);
  
  update_time(); // Update time immediately to avoid blank time
}

// Unload the layers from the main window
static void window_unload(Window *s_main_window) {
  layer_destroy(s_solid_bg_layer); // Destroy the background color
  gbitmap_destroy(s_background_bitmap); // Destroy face bitmap
  bitmap_layer_destroy(s_background_layer); // Destroy face layer
  
  text_layer_destroy(s_time_label); // Destroy time layer 
  text_layer_destroy(s_day_label); // Destroy day label
}

// Initialize the main window
static void init() {
  s_main_window = window_create(); // Create main window
  window_handlers(s_main_window, window_load, window_unload);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler); // Update time every minute
  bluetooth_connection_service_subscribe(update_bluetooth);
  
  window_stack_push(s_main_window, true); // Show window. Animated = true

}

// Deinitialize the main window
static void deinit() {
  tick_timer_service_unsubscribe(); // Unsubscribe from the tick timer
  
  window_destroy(s_main_window); // Destroy the main window
}

int main() {
  init();
  app_event_loop();
  deinit();
}