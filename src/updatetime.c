#include "updatetime.h"

static const char* s_hour_strings[] = {"twelve","one","two","three","four","five","six","seven","eight","nine","ten","eleven"};
static const char* s_min_strings[] = {"o'clock","five past","ten past","quarter past","twenty past","twenty five past","half past","twenty five to","twenty to","quarter to","ten to","five to"};
static const char* s_acc_strings[] = {"coming up to","exactly","just gone"};

static char acc_buffer[13];
static char s_buffer[25];

/**
 * Update the time string displayed on the TextLayer
 */
void update_timeish(TextLayer *s_timeText, TextLayer *s_timeAccText)
{
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  switch(tick_time->tm_min)
  {
    case 0:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_hour_strings[(tick_time->tm_hour % 12)],s_min_strings[0]);
      break;
    case 1:
    case 2:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_hour_strings[tick_time->tm_hour % 12],s_min_strings[0]);
      break;
    case 3:
    case 4:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[1], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 5:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[1], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 6:
    case 7:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[1], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 8:
    case 9:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[2], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 10:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[2], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 11:
    case 12:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[2], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 13:
    case 14:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[3], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 15:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[3], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 16:
    case 17:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[3], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 18:
    case 19:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[4], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 20:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[4], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 21:
    case 22:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[4], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 23:
    case 24:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[5], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 25:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[5], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 26:
    case 27:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[5], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 28:
    case 29:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[6], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 30:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[6], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 31:
    case 32:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[6], s_hour_strings[tick_time->tm_hour % 12]);
      break;
    case 33:
    case 34:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[7], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 35:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[7], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 36:
    case 37:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[7], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 38:
    case 39:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[8], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 40:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[8], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 41:
    case 42:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[8], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 43:
    case 44:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[9], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 45:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[9], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 46:
    case 47:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[9], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 48:
    case 49:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[10], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 50:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[10], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 51:
    case 52:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[10], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 53:
    case 54:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[11], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 55:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[1]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[11], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 56:
    case 57:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[2]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_min_strings[11], s_hour_strings[(tick_time->tm_hour + 1) % 12]);
      break;
    case 58:
    case 59:
      snprintf(acc_buffer,sizeof(acc_buffer),"%s",s_acc_strings[0]);
      snprintf(s_buffer,sizeof(s_buffer),"%s\n%s",s_hour_strings[(tick_time->tm_hour + 1) % 12], s_min_strings[0]);
      break;
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_timeAccText, acc_buffer);
  text_layer_set_text(s_timeText, s_buffer);
}

/**
 * Update the time string displayed on the TextLayer
 */
void update_time(TextLayer *s_timeText)
{
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_timeText, s_buffer);
}

/**
 * Update the time string displayed on the TextLayer
 */
void update_date(TextLayer *s_dateText)
{
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current day, month and day of the month into a buffer
  static char s_buffer[23];
  strftime(s_buffer, sizeof(s_buffer), "%A %e %B", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_dateText, s_buffer);
}