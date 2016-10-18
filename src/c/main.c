#include <pebble.h>
#include "main.h"

static Window *s_window;
static TextLayer *s_label_faction

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
  
  
// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorClear;
  settings.ForegroundColor = GColorBlack;
  settings.Faction = false;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements

static void prv_update_display() {
  // Background color
  window_set_background_color(s_window, settings.BackgroundColor);


  // factions
  if (settings.Faction) {
    
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
    GRect(0, 0, bounds.size.w, 50));


  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BROTHERHOOD);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));


  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}
  
  else {
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorBlueMoon);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
    // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ABSTERGO);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  }


// Handle the response from AppMessage

  // Second Tick
  Tuple *Faction_t = dict_find(iter, MESSAGE_KEY_Faction);
  if (Faction_t) {
    settings.Faction = Faction_t->value->int32 == 1;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

// Window Load event
static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_label_secondtick = text_layer_create(GRect(0, bounds.size.h/4, bounds.size.w, 30));
  text_layer_set_text_alignment(s_label_secondtick, GTextAlignmentCenter);
  text_layer_set_background_color(s_label_secondtick, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_label_secondtick));

  s_label_animations = text_layer_create(GRect(0, bounds.size.h/2, bounds.size.w, 30));
  text_layer_set_text_alignment(s_label_animations, GTextAlignmentCenter);
  text_layer_set_background_color(s_label_animations, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_label_animations));

  prv_update_display();
}

// Window Unload event
static void prv_window_unload(Window *window) {
  layer_destroy(text_layer_get_layer(s_label_secondtick));
  layer_destroy(text_layer_get_layer(s_label_animations));
}

static void prv_init(void) {
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });

  window_stack_push(s_window, true);
}

static void prv_deinit(void) {
  if (s_window) {
    window_destroy(s_window);
  }
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}