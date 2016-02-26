#include "hardware.h"

// Bluetooth bitmaps
static GBitmap *s_btOnIconBmp;
//static GBitmap *s_btOffIconBmp;

// Battery bitmaps
static GBitmap *s_bat0IconBmp;
static GBitmap *s_bat25IconBmp;
static GBitmap *s_bat50IconBmp;
static GBitmap *s_bat75IconBmp;
static GBitmap *s_bat100IconBmp;
static GBitmap *s_batChargeIconBmp;

// Bitmap layers
BitmapLayer *s_btIconLayer;
BitmapLayer *s_powerIconLayer;

/**
 * Get battery icon bounds
 */
GRect get_battery_icon_bounds()
{
  return(gbitmap_get_bounds(s_bat0IconBmp));
}

/**
 * Get bluetooth icon bounds
 */
GRect get_bt_icon_bounds()
{
  return(gbitmap_get_bounds(s_btOnIconBmp));
}

/**
 * Update the bluetooth icon displayed on the BitmapLayer
 */
void update_bt_status(bool connected)
{
 // Set the bitmap onto the layer and add to the window
  if (connected)
  {
    layer_set_hidden((Layer *)s_btIconLayer,false);
    bitmap_layer_set_bitmap(s_btIconLayer, s_btOnIconBmp);
  } else {
    layer_set_hidden((Layer *)s_btIconLayer,true);
    //bitmap_layer_set_bitmap(s_btIconLayer, s_btOffIconBmp);
  }
}

/**
 * Update the battery icon displayed on the BitmapLayer
 */
void update_power_status(BatteryChargeState charge_state)
{
  if (charge_state.is_charging) {
    bitmap_layer_set_bitmap(s_powerIconLayer, s_batChargeIconBmp);
  }
  else
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
}

/**
 * Battery state event handler
 */
void handle_battery(BatteryChargeState charge_state)
{
  update_power_status(charge_state);
}

/**
 * Bluetooth connectivity event handler
 */
void handle_bt(bool connected)
{
  if (connected) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is connected!");
    // double double
    vibes_double_pulse();
    psleep(1000);
    vibes_double_pulse();
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is not connected!");
    vibes_double_pulse();
  }
  update_bt_status(connected);
}

/**
 *  Subscribe to hardware services
 */
void init_hardware_handlers()
{
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = handle_bt
  });
  
  battery_state_service_subscribe((BatteryStateHandler) {
    handle_battery
  });  
}

/**
 *  Clean up hardware service handlers
 */
void destroy_hardware_handlers()
{
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
}

/**
 * Set up all of the hardware icon resources
 */
void init_hardware_icons()
{
  // Create GBitmaps
  s_bat0IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_0_WHITE);
  s_bat25IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_25_WHITE);
  s_bat50IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_50_WHITE);
  s_bat75IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_75_WHITE);
  s_bat100IconBmp = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_100_WHITE);
  s_batChargeIconBmp = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGE_WHITE);
  
  // Create GBitmaps
  s_btOnIconBmp = gbitmap_create_with_resource(RESOURCE_ID_PHONE_WHITE);
  //s_btOffIconBmp = gbitmap_create_with_resource(RESOURCE_ID_NO_PHONE);
}

/**
 * clean up gbitmaps
 */
void destroy_hardware_icons()
{
  // destroy GBitmaps
  gbitmap_destroy(s_btOnIconBmp);
//  gbitmap_destroy(s_btOffIconBmp);
  gbitmap_destroy(s_bat0IconBmp);
  gbitmap_destroy(s_bat25IconBmp);
  gbitmap_destroy(s_bat50IconBmp);
  gbitmap_destroy(s_bat75IconBmp);
  gbitmap_destroy(s_bat100IconBmp);
  gbitmap_destroy(s_batChargeIconBmp);
}