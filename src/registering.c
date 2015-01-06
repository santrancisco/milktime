#include <pebble.h>
#include "registering.h"
#include "common.h"
#include "mainmenu.h"
#include "listing.h"


static Window *s_window;
static GFont s_res_gothic_28_bold;
static GFont s_res_bitham_42_bold;
static GFont s_res_gothic_18_bold;
static GBitmap *s_res_image_action_icon_plus;
static GBitmap *s_res_image_action_icon_check;
static GBitmap *s_res_image_action_icon_minus;
static TextLayer *register_amount;
static TextLayer *register_unit;
static TextLayer *register_title;
static ActionBarLayer *register_action;

static char* STR_HEAVY="Soiled";
static char* STR_LIGHT="Wet";

static int registeringTime;
static struct tm *timeNow;
static int hours;
static int minutes;
static int timeInMin;
static int currentAmount;
static bool nappieState;
static int stage;
static int feedKey;
static bool isNew;

static void update_Time(){
  hours = timeInMin/60;
  minutes = timeInMin%60;
  static char time_str[20];
  snprintf(time_str,sizeof(time_str),"%d:%02d",hours,minutes);
  text_layer_set_text(register_amount, time_str);
}

static void update_Amount(){
    static char amount_str[20];
    snprintf(amount_str,sizeof(amount_str),"%d",currentAmount);
    text_layer_set_text(register_amount, amount_str);
}

static void update_Nappie(){
    if(nappieState){
      text_layer_set_text(register_amount, STR_HEAVY);
    }  else {
      text_layer_set_text(register_amount, STR_LIGHT);
    }
}

static void save_Data(int feedKey){
  persist_write_int(feedKey, timeInMin);
  persist_write_int(feedKey + FEEDTOAMOUNT, currentAmount);
  persist_write_bool(feedKey + FEEDTONAPPIE, nappieState);
  int minfeedKey = persist_read_int(MIN_FEED_PKEY);
  if (isNew){  //if the data is new, reset registeringTime.
    persist_write_int(REGISTERING_TIME_PKEY, 0);
    persist_write_int(LAST_FEED_PKEY,feedKey);
    if (feedKey+1 ==  (minfeedKey + MAX_NUMBER_OF_FEED_DEFAULT)){
      persist_write_int(CURRENT_FEED_PKEY, persist_read_int(MIN_FEED_PKEY));
    } else {
      persist_write_int(CURRENT_FEED_PKEY, feedKey+1);
    }
    refreshRecordTotal();
  } else{
    refresh_menu_item(feedKey-minfeedKey, timeInMin, currentAmount, nappieState);
  }
  
}



static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(stage==0){
    if (timeInMin == 1439){
      timeInMin = 0;
    } else {
      timeInMin += 1;
    }
    update_Time();
  }  else if (stage == 1){
    currentAmount += 10;
    update_Amount();
  } else {
    nappieState = !nappieState;
    update_Nappie();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(stage==0){
    text_layer_set_text(register_title, "Amount");
    update_Amount();
    text_layer_set_text(register_unit, "ml");
  }  else if (stage == 1){
      text_layer_set_text(register_title, "Nappie State");
      text_layer_set_font(register_amount, s_res_gothic_28_bold);
      update_Nappie();
      text_layer_set_text(register_unit, "");
  } else {
    //Save new record
      save_Data(feedKey);
      hide_registering();
  }
  stage += 1;
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(stage==0){
    if (timeInMin == 0){
      timeInMin = 1439;
    } else {
      timeInMin -= 1;
    }
    update_Time();
  }  else if (stage == 1){
     if (currentAmount > 10){
         currentAmount -= 10;
     } else {
         currentAmount = 0;
     }
     update_Amount();
  } else {
    nappieState = !nappieState;
    update_Nappie();
  }
}

static void click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 50;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, repeat_interval_ms, (ClickHandler) select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) down_click_handler);
}

static void initialise_ui(void) {
 
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_bitham_42_bold = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_res_image_action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  s_res_image_action_icon_check = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_CHECK);
  s_res_image_action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);
  
  // register_amount
  register_amount = text_layer_create(GRect(20, 50, 100, 50));
  //text_layer_set_text(register_amount, "0");
  text_layer_set_text_alignment(register_amount, GTextAlignmentCenter);
  text_layer_set_font(register_amount, s_res_gothic_28_bold);
  update_Time();
  layer_add_child(window_get_root_layer(s_window), (Layer *)register_amount);
  
  // register_unit
  register_unit = text_layer_create(GRect(20, 110, 100, 20));
  text_layer_set_text(register_unit, "");
  text_layer_set_text_alignment(register_unit, GTextAlignmentCenter);
  text_layer_set_font(register_unit, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)register_unit);
  
  // register_title
  register_title = text_layer_create(GRect(20, 10, 100, 28));
  text_layer_set_text(register_title, "Time");
  text_layer_set_text_alignment(register_title, GTextAlignmentCenter);
  text_layer_set_font(register_title, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)register_title);
  
  // register_action
  register_action = action_bar_layer_create();
  action_bar_layer_add_to_window(register_action, s_window);
  action_bar_layer_set_background_color(register_action, GColorBlack);
  action_bar_layer_set_icon(register_action, BUTTON_ID_UP, s_res_image_action_icon_plus);
  action_bar_layer_set_icon(register_action, BUTTON_ID_SELECT, s_res_image_action_icon_check);
  action_bar_layer_set_icon(register_action, BUTTON_ID_DOWN, s_res_image_action_icon_minus);
  action_bar_layer_set_click_config_provider(register_action, click_config_provider);
  layer_add_child(window_get_root_layer(s_window), (Layer *)register_action);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(register_amount);
  text_layer_destroy(register_unit);
  text_layer_destroy(register_title);
  action_bar_layer_destroy(register_action);
  gbitmap_destroy(s_res_image_action_icon_plus);
  gbitmap_destroy(s_res_image_action_icon_check);
  gbitmap_destroy(s_res_image_action_icon_minus);
}


static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_registering(void) {   
  //WHen user click on registering a feed, he/she register the time. When the feed finish, he/she will continue putting in the amount of milk and the nappie state. 
  isNew = true;
  stage=0;
  registeringTime = persist_read_int(REGISTERING_TIME_PKEY);
  if (registeringTime != 0){                    //Has the time been recorded?
     timeInMin = registeringTime;
  }else {                                       //Setting registering time to NOW.
  time_t now = time(NULL);
  timeNow = localtime(&now);  
  hours = timeNow->tm_hour;
  minutes = timeNow->tm_min;
  timeInMin = (60*hours)+minutes;
  persist_write_int(REGISTERING_TIME_PKEY, timeInMin);
  }  

  feedKey=persist_read_int(CURRENT_FEED_PKEY);
  
  currentAmount=persist_read_int(AMOUNT_PKEY);
  nappieState=persist_read_bool(NAPPIE_PKEY);
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void show_editing(int feedID) {
  isNew=false;
  feedKey=feedID;
  timeInMin = persist_read_int(feedKey);  
  hours = timeInMin/60;
  minutes = timeInMin%60;
  stage=0;
  currentAmount=persist_read_int(feedKey + FEEDTOAMOUNT);
  nappieState=persist_read_bool(feedKey + FEEDTONAPPIE);
  
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}


void hide_registering(void) {
  window_stack_remove(s_window, true);
}
