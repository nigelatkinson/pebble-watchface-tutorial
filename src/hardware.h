#pragma once

#include <pebble.h>

extern BitmapLayer *s_btIconLayer;
extern BitmapLayer *s_powerIconLayer;

/**
 * Get battery icon bounds
 */
GRect get_battery_icon_bounds(void);

/**
 * Get bluetooth icon bounds
 */
GRect get_bt_icon_bounds(void);


/**
 * Update the bluetooth icon displayed on the BitmapLayer
 */
void update_bt_status(bool connected);

/**
 * Update the battery icon displayed on the BitmapLayer
 */
void update_power_status(BatteryChargeState charge_state);

/**
 * Battery state event handler
 */
void handle_battery(BatteryChargeState charge_state);

/**
 * Bluetooth connectivity event handler
 */
void handle_bt(bool connected);

/**
 *  Subscribe to hardware services
 */
void init_hardware_handlers(void);

/**
 *  Clean up hardware service handlers
 */
void destroy_hardware_handlers(void);

/**
 * Set up all of the hardware icon resources
 */
void init_hardware_icons(void);

/**
 * clean up gbitmaps
 */
void destroy_hardware_icons(void);