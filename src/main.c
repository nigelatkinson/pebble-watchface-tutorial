#include <math.h>
#include "updatetime.h"
#include "hardware.h"
#include "newsweather.h"

static Window *s_mainWindow;

// Custom fonts
//static GFont s_timeFont;
//static GFont s_dateFont;

// Text layers
static TextLayer *s_timeText;
static TextLayer *s_timeAccText;
static TextLayer *s_dateText;

// Colours
#define POWER_ICON_BACK_COLOR GColorClear
#define BT_ICON_BACK_COLOR GColorClear
#define TIME_TEXT_BACK_COLOR GColorClear
#define TIME_TEXT_FORE_COLOR_DAY GColorWhite
#define TIME_TEXT_FORE_COLOR_NIGHT GColorElectricBlue
#define WEATHER_TEXT_FORE_COLOR_WARM GColorYellow
#define WEATHER_TEXT_FORE_COLOR_COLD GColorVeryLightBlue
#define WEATHER_TEXT_FORE_COLOR_HOT GColorRajah
#define TIME_TEXT_FORE_COLOR TIME_TEXT_FORE_COLOR_NIGHT
#define WINDOW_BACK_COLOR GColorOxfordBlue

#define SLEEPY_TIME_START_HOUR 0
#define SLEEPY_TIME_END_HOUR 6

static bool s_is_sleepy_time;

#define NEWS_WEATHER_UPDATE_PERIOD_MINS 12

void set_weather_text_colour()
{
    // Set weather text colour
    int temp = get_temperature();
    if (temp < 10)
    {
      text_layer_set_text_color(s_weatherText, WEATHER_TEXT_FORE_COLOR_COLD);
    } else if (temp > 25) {
      text_layer_set_text_color(s_weatherText, WEATHER_TEXT_FORE_COLOR_HOT);
    } else {
      text_layer_set_text_color(s_weatherText, WEATHER_TEXT_FORE_COLOR_WARM);
    }
}

/**
 * Tick timer handler
 */
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{ 
  s_is_sleepy_time = (tick_time->tm_hour <= SLEEPY_TIME_END_HOUR && tick_time->tm_hour >= SLEEPY_TIME_START_HOUR);
  
  if (s_is_sleepy_time)
  {
    text_layer_set_text_color(s_timeAccText, TIME_TEXT_FORE_COLOR_NIGHT);
    text_layer_set_text_color(s_timeText, TIME_TEXT_FORE_COLOR_NIGHT);
    text_layer_set_text_color(s_dateText, TIME_TEXT_FORE_COLOR_NIGHT);
    text_layer_set_text_color(s_newsText, TIME_TEXT_FORE_COLOR_NIGHT);
  } else {
    text_layer_set_text_color(s_timeAccText, TIME_TEXT_FORE_COLOR_DAY);
    text_layer_set_text_color(s_timeText, TIME_TEXT_FORE_COLOR_DAY);
    text_layer_set_text_color(s_dateText, TIME_TEXT_FORE_COLOR_DAY);
    text_layer_set_text_color(s_newsText, TIME_TEXT_FORE_COLOR_DAY);
  }
  //APP_LOG(APP_LOG_LEVEL_INFO, "Updating displayed news story...");
  display_news_weather();
  
  update_timeish(s_timeText,s_timeAccText);
  
  // update the date on the hour
  if(tick_time->tm_min == 0)
  {
    //APP_LOG(APP_LOG_LEVEL_INFO, "Updating date...");
    update_date(s_dateText);
  }

  // Get news/weather update every 12 minutes outside sleepy time
  if(!s_is_sleepy_time && (tick_time->tm_min % NEWS_WEATHER_UPDATE_PERIOD_MINS == 0))
  {
    request_news_weather_data();
    
    // Set weather text colour
    set_weather_text_colour();
    
    // refresh time to prevent clipping of third line
    update_timeish(s_timeText,s_timeAccText);
  }
}

/**
 * Window load handler function
 */
static void mainWindowLoad(Window *w)
{
  init_hardware_icons();
  
  // Get information about the root window
  Layer *windowLayer = window_get_root_layer(w);
  GRect bounds = layer_get_bounds(windowLayer);
  
  //////////////////////////////////////////
  // Battery
  
  GRect bat_icon_bounds = get_battery_icon_bounds();
  
  // Create BitmapLayer to display the GBitmap
  s_powerIconLayer = bitmap_layer_create(GRect(bounds.size.w - (bat_icon_bounds.size.w + 2),
                                               0, bat_icon_bounds.size.w, bat_icon_bounds.size.h));

  // set background colout and compositing mode 
  bitmap_layer_set_background_color(s_powerIconLayer, POWER_ICON_BACK_COLOR);
  bitmap_layer_set_compositing_mode(s_powerIconLayer, GCompOpSet);
  
  update_power_status(battery_state_service_peek());
  
  // add the bitmap layer to the window
  layer_add_child(windowLayer, bitmap_layer_get_layer(s_powerIconLayer));
  
  //////////////////////////////////////////
  // BT connection

  GRect bt_icon_bounds = get_bt_icon_bounds();
  
  // Create BitmapLayer to display the GBitmap
  s_btIconLayer = bitmap_layer_create(GRect(bounds.size.w - (bat_icon_bounds.size.w + bt_icon_bounds.size.w + 4), 4, bt_icon_bounds.size.w, bt_icon_bounds.size.h));

  // set background colout and compositing mode 
  bitmap_layer_set_background_color(s_btIconLayer, BT_ICON_BACK_COLOR);
  bitmap_layer_set_compositing_mode(s_btIconLayer, GCompOpSet);
  
  update_bt_status(connection_service_peek_pebble_app_connection());
  
  // add the bitmap layer to the window
  layer_add_child(windowLayer, bitmap_layer_get_layer(s_btIconLayer));
  
  ////////////////////////////////////
  // Create the time text layers
  
  // time accuracy text layer
  s_timeAccText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(-2, -6), 94, 28));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_timeAccText, TIME_TEXT_BACK_COLOR);
  text_layer_set_text_color(s_timeAccText, TIME_TEXT_FORE_COLOR);
  
  //s_timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFF_TUSJ_42));
  GFont timeFont = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  
  text_layer_set_font(s_timeAccText, timeFont);
  text_layer_set_text_alignment(s_timeAccText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_timeAccText));
    
  // time text layer
  s_timeText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(20, 16), bounds.size.w-4, 100));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_timeText, TIME_TEXT_BACK_COLOR);
  text_layer_set_text_color(s_timeText, TIME_TEXT_FORE_COLOR);

  //update_time();
  update_timeish(s_timeText,s_timeAccText);
  
  //s_timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFF_TUSJ_42));
  timeFont = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  
  text_layer_set_font(s_timeText, timeFont);
  text_layer_set_text_alignment(s_timeText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_timeText));
  
  ///////////////////////////////////
  // Create the date text layer
  s_dateText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(128, 122), bounds.size.w-4, 16));
  
   // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_dateText, TIME_TEXT_BACK_COLOR);
  text_layer_set_text_color(s_dateText, TIME_TEXT_FORE_COLOR);

  update_date(s_dateText);
  
  //s_dateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFF_TUSJ_24));
  GFont dateFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  
  text_layer_set_font(s_dateText, dateFont);
  text_layer_set_text_alignment(s_dateText, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_dateText));
  
  //////////////////////////////////
  // Create the weather text layer
  s_weatherText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(112, 108), bounds.size.w-4, 18));
  
   // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_weatherText, TIME_TEXT_BACK_COLOR);
  //text_layer_set_text_color(s_weatherText, WEATHER_TEXT_FORE_COLOR_WARM);

  update_weather((Tuple *)NULL,(Tuple *)NULL, (Tuple *)NULL);
  
  GFont weatherFont = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  text_layer_set_font(s_weatherText, weatherFont);
  text_layer_set_text_alignment(s_weatherText, GTextAlignmentLeft);
  
  // Set weather text colour
  set_weather_text_colour();

  // Add it as a child layer to the Window's root layer
  layer_add_child(windowLayer, text_layer_get_layer(s_weatherText));
  
  //////////////////////////////////
  // Create the news text layer
  s_newsText = text_layer_create(GRect(2, PBL_IF_ROUND_ELSE(142, 136), bounds.size.w-4, 30));
  
   // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_newsText, TIME_TEXT_BACK_COLOR);
  text_layer_set_text_color(s_newsText, TIME_TEXT_FORE_COLOR);

  display_news_weather();
  
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
  bitmap_layer_destroy(s_powerIconLayer);

  destroy_hardware_icons();
  
  // Unload GFont
  //fonts_unload_custom_font(s_timeFont);
  //fonts_unload_custom_font(s_dateFont);
}

/**
 * App init
 */
static void init()
{
  init_news_weather();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  init_hardware_handlers();
  
  // create window
  s_mainWindow = window_create();
  
  // set window handlers
  window_set_window_handlers(s_mainWindow, (WindowHandlers) {
    .load = mainWindowLoad,
    .unload = mainWindowUnload
  });
  
  // set background colour
  window_set_background_color(s_mainWindow, WINDOW_BACK_COLOR);
  
  // display window
  window_stack_push(s_mainWindow, true);
}

/**
 * App deinit
 */
static void deinit()
{
  window_destroy(s_mainWindow);
  deinit_news_weather();
  destroy_hardware_handlers();
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