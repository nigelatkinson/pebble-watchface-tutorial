#pragma once

#include <pebble.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_LOCATION 2
// KEY_HEADLINE_1 - KEY_HEADLINE_6 must be contiguous
#define KEY_HEADLINE_1 3
#define KEY_HEADLINE_2 4
#define KEY_HEADLINE_3 5
#define KEY_HEADLINE_4 6
#define KEY_HEADLINE_5 7
#define KEY_HEADLINE_6 8

extern TextLayer *s_weatherText;
extern TextLayer *s_newsText;

/**
 * Update the weather info
 */
void update_weather(Tuple *temp_tuple, Tuple *conditions_tuple, Tuple *location_tuple);

/**
 * Update the news headlines
 */
void display_news_weather(void);

/**
 *  Send data request message
 */
void request_news_weather_data(void);

/**
 *  Send data request message
 */
void request_weather_data(void);

/**
 *  Init the news and weather subsystem
 */
void init_news_weather(void);
  
/**
 *  Clean up news and weather subsystem
 */
void deinit_news_weather(void);