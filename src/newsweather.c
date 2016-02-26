#include <pebble.h>
#include "newsweather.h"

static char s_news_array[6][50];
static char s_weather[26];

TextLayer *s_weatherText;
TextLayer *s_newsText;

/**
 * Update the weather info buffer
 */
void update_weather(Tuple *temp_tuple, Tuple *conditions_tuple, Tuple *location_tuple)
{
  // Store incoming information
  char temperature_buffer[6];
  char conditions_buffer[16];
  char location_buffer[16];
  
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
    snprintf(s_weather, sizeof(s_weather), "%s %s %s", location_buffer, temperature_buffer, conditions_buffer);
  } else if (temp_tuple){
    snprintf(s_weather, sizeof(s_weather), "%s", temperature_buffer);
  } else if (conditions_tuple){
    snprintf(s_weather, sizeof(s_weather), "%s", conditions_buffer);
  } else {
    snprintf(s_weather, sizeof(s_weather), "%s", "No weather data");
  }
}

/**
 * Display the news and weather
 */
void display_news_weather()
{
  // Write the news into a buffer
  static char s_buffer[50] = "Loading news...";

 // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp); 
  int news_ctr = tick_time->tm_min % 6;
  
  if (s_news_array[news_ctr] && strlen(s_news_array[news_ctr]) > 0)
  { 
    snprintf(s_buffer, sizeof(s_buffer), "%s", s_news_array[news_ctr]);
  }
  
  // Display this news on the TextLayer
  text_layer_set_text(s_newsText, s_buffer);
  
  // Display the weather on the TextLayer
  text_layer_set_text(s_weatherText, s_weather);
}

/**
 *  Send data request message
 */
void request_news_weather_data()
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
  
  Tuple *headline;
  
  // Assumption here is that KEY_HEADLINE_1 - KEY_HEADLINE_6 are contiguous
  for (int i = KEY_HEADLINE_1;i <= KEY_HEADLINE_6; i++)
  {
    headline = dict_find(iterator, i);
    if (headline){
      strncpy(s_news_array[i - KEY_HEADLINE_1],headline->value->cstring,sizeof(s_news_array[i - KEY_HEADLINE_1]));
    }
  }
  
  display_news_weather();
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
 *  Init the news and weather subsystem
 */
void init_news_weather()
{
    // init news array
  for (int i = 0; i < 6; i++)
  {
    strncpy(s_news_array[i],"",sizeof(s_news_array[i]));
  }
   
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
 *  Clean up news and weather subsystem
 */
void deinit_news_weather()
{
  app_message_deregister_callbacks();  
}
