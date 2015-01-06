#include <pebble.h>
#include "listing.h"
#include "common.h"
#include "registering.h"
#include "mainmenu.h"
  
static Window *s_window;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[1];   
static SimpleMenuItem menu_section0_items[MAX_NUMBER_OF_FEED_DEFAULT];
static char titleArray[MAX_NUMBER_OF_FEED_DEFAULT][20];
static char subtitleArray[MAX_NUMBER_OF_FEED_DEFAULT][20];
static char* STR_ML="ml";
static char* STR_HEAVY="Soiled";
static char* STR_LIGHT="Wet";
static int minfeedKey;

void edit_callback(int index, void *context)
{
    show_editing(minfeedKey+index);
}

void refresh_menu_item(int index, int feedtime, int amount, bool nappie){ 
  size_t remaining = sizeof(subtitleArray[index]);
  //memset(subtitleArray[index], 0,remaining);
  subtitleArray[index][0]='\0';
    //Predict the amount and nappie state
  if(nappie){
    remaining -= append_number(subtitleArray[index], amount);
    remaining -= append_string(subtitleArray[index], remaining,STR_ML);
    remaining -= append_string(subtitleArray[index], remaining," - ");
    remaining -= append_string(subtitleArray[index], remaining, STR_HEAVY);
  } else {
    remaining -= append_number(subtitleArray[index], amount);
    remaining -= append_string(subtitleArray[index], remaining,STR_ML);
    remaining -= append_string(subtitleArray[index], remaining," - ");
    remaining -= append_string(subtitleArray[index], remaining, STR_LIGHT);
  }
  memset(titleArray[index], 0, sizeof(titleArray[index]));
  int hours = feedtime/60;
  int minutes = feedtime%60;
  snprintf(titleArray[index], sizeof(titleArray[index]), "%d:%02d", hours, minutes);
  menu_section0_items[index].title=titleArray[index];
  menu_section0_items[index].subtitle = subtitleArray[index];
  //menuItems[index].title=details;
}

void init_listing_sections(void){
    minfeedKey = persist_read_int(MIN_FEED_PKEY);
    int size;
    if (historyIsFull()){
      size = MAX_NUMBER_OF_FEED_DEFAULT;
    } else {
      size = persist_read_int(CURRENT_FEED_PKEY) - persist_read_int(MIN_FEED_PKEY);  
    }
    for (int i = 0; i < size ;i++){
      //int feedTime = persist_read_int(minfeedKey+i);
      //int feedTimeNum = 10;
      //snprintf(feedTime_str, sizeof(feedTime), "%d", feedTime);      
      menu_section0_items[i] = (SimpleMenuItem) {
        .title = "test",
        .callback = &edit_callback
      };
      //refresh_menu_item2(i,1500);
      refresh_menu_item(i,persist_read_int(minfeedKey+i),persist_read_int(minfeedKey+i+FEEDTOAMOUNT),persist_read_bool(minfeedKey+i+FEEDTONAPPIE));
    }
    menu_sections[0] = (SimpleMenuSection) {
        .title = "Feeding history",
        .items = menu_section0_items,
        .num_items = ARRAY_LENGTH(menu_section0_items)
    };
}


static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  GRect frame = layer_get_bounds(window_get_root_layer(s_window));
  init_listing_sections();
  menu_layer = simple_menu_layer_create(frame, s_window, menu_sections, 1, NULL);
  layer_add_child(window_get_root_layer(s_window),simple_menu_layer_get_layer(menu_layer));
}

static void destroy_ui(void) {
  window_destroy(s_window);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_listing(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_listing(void) {
  window_stack_remove(s_window, true);
}
