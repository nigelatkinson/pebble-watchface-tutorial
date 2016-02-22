#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_LOCATION 2
#define KEY_HEADLINE_1 3
#define KEY_HEADLINE_2 4
#define KEY_HEADLINE_3 5
#define KEY_HEADLINE_4 6
#define KEY_HEADLINE_5 7
#define KEY_HEADLINE_6 8

#include <math.h>
#include "updatetime.h"

static Window *s_mainWindow;

// Custom fonts
static GFont s_timeFont;
static GFont s_dateFont;

// Text layers
static TextLayer *s_timeText;
static TextLayer *s_timeAccText;
static TextLayer *s_dateText;
static TextLayer *s_weatherText;
static TextLayer *s_newsText;

// Bitmaps
static BitmapLayer *s_btIconLayer;
static GBitmap *s_btOnIconBmp;
static GBitmap *s_btOffIconBmp;

static BitmapLayer *s_powerIconLayer;
static GBitmap *s_bat0IconBmp;
static GBitmap *s_bat25IconBmp;
static GBitmap *s_bat50IconBmp;
static GBitmap *s_bat75IconBmp;
static GBitmap *s_bat100IconBmp;

static char news_array[6][50];

/**
 * Update the bluetooth icon displayed on the BitmapLayer
 */
static void update_bt_status(bool connected)
{
 // Set the bitmap onto the layer and add to the window
  if (connected)
  {
    bitmap_layer_set_bitmap(s_btIconLayer, s_btOnIconBmp);
  } else {
    bitmap_layer_set_bitmap(s_btIconLayer, s_btOffIconBmp);
  }
}

/**
 * Update the battery icon displayed on the BitmapLayer
 */
static void update_power_status(BatteryChargeState charge_state)
{
 // Set the bitmap onto the layer and add to the window
  if (charge_state.charge_percent >= 90) {
    bitmap_layer_set_bitmap(s_powerIconLayer, s_bat100IconBmp);
  } else if (charge_state.charge_percent >= 70) {
    bitmap_layer_set_bitmap(s_powerIconLayer, s_bat75IconBmp);
  } else if (charge_state.charge_percent >= 45){
    bitmap_layer_set_bitmap(s_powerIconLayer, s_bat50IconBmp);
  } else if (charge_state.charge_percent >= 20){
    bitmap_layer_set_bitmap(s_powerIconLayer, s_bat25IconBmp);
  } else {
    bitmap_layer_set_bitmap(s_powerIconLayer, s_bat0IconBmp);
  }
}

/**
 * Update the weather info
 */
static void update_weather(Tuple *temp_tuple, Tuple *conditions_tuple, Tuple *location_tuple)
{
  // Write the weather into a buffer
  static char s_buffer[26] = "No weather data";
  // Store incoming information
  static char temperature_buffer[6];
  static char conditions_buffer[16];
  static char location_buffer[16];
  
  if (temp_tuple)
  {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", (int)temp_tuple->value->int32);
  }
  
  if (conditions_tuple)
  {
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  }
  
  if (location_tuple)
  {
    snprintf(location_buffer, sizeof(location_buffer), "%s", location_tuple->value->cstring);
  }
  
  if (temp_tuple && conditions_tuple && location_tuple)
  {
    snprintf(s_buffer, sizeof(s_buffer), "%s %s %s", location_buffer, temperature_buffer, conditions_buffer);
  } else if (temp_tuple){
    snprintf(s_buffer, sizeof(s_buffer), "%s", temperature_buffer);
  } else if (conditions_tuple){
    snprintf(s_buffer, sizeof(s_buffer), "%s", conditions_buffer);
  }
  // Display this weather on the TextLayer
  text_layer_set_text(s_weatherText, s_buffer);
}

/**
 * Update the news headline
 */
static void update_news()
{
  // Write the news into a buffer
  static char s_buffer[50] = "Loading news...";

 // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp); 
  int news_ctr = tick_time->tm_min % 6;
  
  // Debugging
  /*char news_ctr_buf[14];
  snprintf(news_ctr_buf,sizeof(news_ctr_buf),"news ctr - %d",news_ctr);
  APP_LOG(APP_LOG_LEVEL_INFO,news_ctr_buf);
  
  for (int i=0;i<6;i++)
  {
    APP_LOG(APP_LOG_LEVEL_INFO,news_array[i]);
  }*/
  
  if (news_array[news_ctr])
  { 
    snprintf(s_buffer, sizeof(s_buffer), "%s", news_array[news_ctr]);
  }
  
  // Display this weather on the TextLayer
  text_layer_set_text(s_newsText, s_buffer);
}

/**
 * Battery state event handler
 */
static void handle_battery(BatteryChargeState charge_state) {
  /*if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%% charged", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);*/
  update_power_status(charge_state);
}

/**
 * Bluetooth connectivity event handler
 */
void bt_handler(bool connected) {
  if (connected) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is connected!");
    // double double
    vibes_double_pulse();
    vibes_double_pulse();
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is not connected!");
    vibes_double_pulse();
  }
  update_bt_status(connected);
}

/**
 * Tick timer handler
 */
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  //update_time();
  update_timeish(s_timeText,s_timeAccText);
  if(tick_time->tm_min == 0)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "Updating date...");
    update_date(s_dateText);
  }

  // Get news/weather update every 12 minutes
  if(tick_time->tm_min % 12 == 0)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "Fetching updated news and weather...");
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
    dict_write_uint8(iter, 1, 0);
    dict_write_uint8(iter, 2, 0);
    dict_write_uint8(iter, 3, 0);
    dict_write_uint8(iter, 4, 0);
    dict_write_uint8(iter, 5, 0);
    dict_write_uint8(iter, 6, 0);
    dict_write_uint8(iter, 7, 0);
    dict_write_uint8(iter, 8, 0);

    // Send the message!
    app_message_outbox_send();
  }
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Updating displayed news story...");
  update_news();
}

/**
 * AppMessage callback handler
 */
static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{ 
  APP_LOG(APP_LOG_LEVEL_INFO, "Handling inbox received callback...");
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
  Tuple *location_tuple = dict_find(iterator, KEY_LOCATION);
  
  update_weather(temp_tuple, conditions_tuple, location_tuple);
  
  Tuple *headline = dict_find(iterator, KEY_HEADLINE_1);
  
  if (headline){
    strncpy(news_array[0],headline->value->cstring,sizeof(news_array[0]));
  }
  
  headline = dict_find(iterator, KEY_HEADLINE_2);
  
  if (headline){
    strncpy(news_array[1],headline->value->cstring,sizeof(news_array[1]));
  }
  
  headline = dict_find(iterator, KEY_HEADLINE_3);
  
  if (headline){
    strncpy(news_array[2],headline->value->cstring,sizeof(news_array[2]));
  }
  
  headline = dict_find(iterator, KEY_HEADLINE_4);
  
  if (headline){
    strncpy(news_array[3],headline->value->cstring,sizeof(news_array[3]));
  }
  
  headline = dict_find(iterator, KEY_HEADLINE_5);
  
  if (headline){
    strncpy(news_array[4],headline->value->cstring,sizeof(news_array[4]));
  }
  
  headline = dict_find(iterator, KEY_HEADLINE_6);
  
  if (headline){
    strncpy(news_array[5],headline->value->cstring,sizeof(news_array[5]));
  }

  update_news();
}

/**
 * AppMessage error handlers
 */
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  char msg[40];
  snprintf(msg, sizeof(msg), "Message dropped! Reason: %d", reason);
  APP_LOG(APP_LOG_LEVEL_ERROR, msg);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

/**
 * Window load handler function
 */
static void mainWindowLoad(Window *w)
{
  GFont weatherFont;
  
  // Get information about the root window
  Layer *windowLayer = window_get_root_layer(w);
  GRect bounds = layer_get_bounds(windowLayer);
  
  //////////////////////////////////////////
  // Battery
  
   // Create GBitmaps
  s_bat0IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BAT_0);
  s_bat25IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BAT_25);
  s_bat50IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BAT_50);
  s_bat75IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BAT_75);
  s_bat100IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BAT_100);
  
  GRect iconBounds = gbitmap_get_bounds(s_bat0IconBmp);
  
  // Create BitmapLayer to display the GBitmap
  s_powerIconLayer = bitmap_layer_create(GRect(bounds.size.w-iconBounds.size.w, 0, iconBounds.size.w, iconBounds.size.h));

  // set background colout and compositing mode 
  bitmap_layer_set_background_color(s_powerIconLayer, GColorClear);
  bitmap_layer_set_compositing_mode(s_powerIconLayer, GCompOpSet);
  
  update_power_status(battery_state_service_peek());
  
  // add the bitmap layer to the window
  layer_add_child(windowLayer, bitmap_layer_get_layer(s_powerIconLayer));
  
  //////////////////////////////////////////
  // BT connection
  
  // Create GBitmaps
  s_btOnIconBmp = gbitmap_create_with_resource(RESOURCE_ID_PHONE);
  s_btOffIconBmp = gbitmap_create_with_resource(RESOURCE_ID_NO_PHONE);

  iconBounds = gbitmap_get_bounds(s_btOnIconBmp);
  
  // Create BitmapLayer to display the GBitmap
  s_btIconLayer = bitmap_layer_create(GRect(88, 0, iconBounds.size.w, iconBounds.size.h));

  // set background colout and compositing mode 
  bitmap_layer_set_background_color(s_btIconLayer, GColorClear);
  bitmap_layer_set_compositing_mode(s_btIconLayer, GCompOpSet);
  
  update_bt_status(connection_service_peek_pebble_app_connection());
  
  // add the bitmap layer to the window
  layer_add_child(windowLayer, bitmap_layer_get_layer(s_btIconLayer));
  
  ////////////////////////////////////
  // Create the time text layers
  
  // time accuracy text layer
  s_timeAccText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(6, 2), 84, 22));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_timeAccText, GColorClear);
  text_layer_set_text_color(s_timeAccText, GColorBlack);
  
  //s_timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFF_TUSJ_42));
  s_timeFont = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  
  text_layer_set_font(s_timeAccText, s_timeFont);
  text_layer_set_text_alignment(s_timeAccText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_timeAccText));
    
  // time text layer
  s_timeText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(22, 18), bounds.size.w-4, 90));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_timeText, GColorClear);
  text_layer_set_text_color(s_timeText, GColorBlack);

  //update_time();
  update_timeish(s_timeText,s_timeAccText);
  
  //s_timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFF_TUSJ_42));
  s_timeFont = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  
  text_layer_set_font(s_timeText, s_timeFont);
  text_layer_set_text_alignment(s_timeText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_timeText));
  
  ///////////////////////////////////
  // Create the date text layer
  s_dateText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(112, 108), bounds.size.w-4, 18));
  
   // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_dateText, GColorClear);
  text_layer_set_text_color(s_dateText, GColorBlack);

  update_date(s_dateText);
  
  //s_dateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFF_TUSJ_24));
  s_dateFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  
  text_layer_set_font(s_dateText, s_dateFont);
  text_layer_set_text_alignment(s_dateText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_dateText));
  
  //////////////////////////////////
  // Create the weather text layer
  s_weatherText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(128, 122), bounds.size.w-4, 16));
  
   // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_weatherText, GColorClear);
  text_layer_set_text_color(s_weatherText, GColorBlack);

  update_weather((Tuple *)NULL,(Tuple *)NULL, (Tuple *)NULL);
  
  weatherFont = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  text_layer_set_font(s_weatherText, weatherFont);
  text_layer_set_text_alignment(s_weatherText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_weatherText));
  
  //////////////////////////////////
  // Create the news text layer
  s_newsText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(142, 136), bounds.size.w-4, 30));
  
   // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_newsText, GColorClear);
  text_layer_set_text_color(s_newsText, GColorBlack);

  update_news();
  
  text_layer_set_font(s_newsText, weatherFont);
  text_layer_set_text_alignment(s_newsText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_newsText));
  
}

/**
 * Window unload handler function
 */
static void mainWindowUnload(Window *w)
{
  // destroy the text layers
  text_layer_destroy(s_timeText);
  text_layer_destroy(s_timeAccText);
  text_layer_destroy(s_dateText);
  text_layer_destroy(s_weatherText);
  text_layer_destroy(s_newsText);
  // destroy the bitmap layers
  bitmap_layer_destroy(s_btIconLayer);
  // destroy GBitmaps
  gbitmap_destroy(s_btOnIconBmp);
  gbitmap_destroy(s_btOffIconBmp);
  // Unload GFont
  fonts_unload_custom_font(s_timeFont);
  fonts_unload_custom_font(s_dateFont);
}

/**
 * App init
 */
static void init()
{
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bt_handler
  });
  
  battery_state_service_subscribe((BatteryStateHandler) {handle_battery});
  
  // create window
  s_mainWindow = window_create();
  
  // set window handlers
  window_set_window_handlers(s_mainWindow, (WindowHandlers) {
    .load = mainWindowLoad,
    .unload = mainWindowUnload
  });
  
  // set background colour
  window_set_background_color(s_mainWindow, GColorWhite);
  
  // display window
  window_stack_push(s_mainWindow, true);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  const int inbox_size = 256;
  const int outbox_size = 64;
  app_message_open(inbox_size, outbox_size);
}

/**
 * App deinit
 */
static void deinit()
{
  window_destroy(s_mainWindow);
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
}

/**
 * Main function
 */
int main(void)
{
  init();
  app_event_loop();
  deinit();
}