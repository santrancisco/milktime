#include <pebble.h>
#include "mainmenu.h"
#include "common.h"
#include "registering.h"
#include "listing.h"
  
  
static Window *s_window;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[2];      // Two sections
static SimpleMenuItem menu_section0_items[3];   // Section 0 (Some settings...) with one entry
static SimpleMenuItem menu_section1_items[2];   // Section 1 (About...) with one entry
static char* STR_NEXTFEED="Next:";
static char* STR_MINUTES="min";
static char* STR_ML="ml";
static char* STR_HEAVY="Soiled";
static char* STR_LIGHT="Wet";

static char nexttime[25];
static char nextguess[25];
static char recordTotal_str[20];

static void initialise_ui(void);

void refreshMilkTime(void){
    //NEXT milk time prediction text
    size_t remaining = sizeof(nexttime);
    //memset(nexttime,0,sizeof(nexttime)); //Fuck it, this is overkill to initialise a string.
    int hours = persist_read_int(MILKTIME_PKEY)/60;
    int minutes = persist_read_int(MILKTIME_PKEY)%60;
    static char time_str[20];
    snprintf(nexttime,sizeof(nexttime),"Next - %d:%02d",hours,minutes);
    menu_section0_items[1].title=nexttime;
  
    remaining = sizeof(nextguess);
    nextguess[0]='\0';
    if(persist_read_bool(NAPPIE_PKEY)){
      remaining -= append_number(nextguess, persist_read_int(AMOUNT_PKEY));
      remaining -= append_string(nextguess, remaining,STR_ML);
      remaining -= append_string(nextguess, remaining," - Nappie: ");
      remaining -= append_string(nextguess, remaining, STR_HEAVY);
    } else {
      remaining -= append_number(nextguess, persist_read_int(AMOUNT_PKEY));
      remaining -= append_string(nextguess, remaining,STR_ML);
      remaining -= append_string(nextguess, remaining," - Nappie: ");
      remaining -= append_string(nextguess, remaining, STR_LIGHT);
    }
    menu_section0_items[1].subtitle=nextguess;
}
void refreshRecordTotal(void){
  //Update total record in feed history menu when changed.
  int recordTotal;
  if(historyIsFull()){
     recordTotal = MAX_NUMBER_OF_FEED_DEFAULT;  
  } else {
    recordTotal = persist_read_int(CURRENT_FEED_PKEY) - persist_read_int(MIN_FEED_PKEY);
  }
  snprintf(recordTotal_str, sizeof(recordTotal_str), "Total: %d", recordTotal);
  menu_section0_items[2].subtitle=recordTotal_str;
}
void refreshMilkTime_callback(int index, void *context){
  predictNextFeedTime();
  predictNextFeed();
  refreshMilkTime();
}

void menu_layer_section0_item_0_callback(int index, void *context)
{
    hide_registering();
    show_registering();
}
void menu_layer_section0_item_2_callback(int index, void *context)
{
    hide_listing();
    show_listing();
}

void init_menu_sections(void){
    menu_section0_items[0] = (SimpleMenuItem) {
        .title = "Register a feed",
        .callback = &menu_layer_section0_item_0_callback
    };
    menu_section0_items[1] = (SimpleMenuItem) {
        .title = "default",
        .callback = &refreshMilkTime_callback
    };
  menu_section0_items[2] = (SimpleMenuItem) {
        .title = "Feed history",
        .callback = &menu_layer_section0_item_2_callback
    };
    menu_section1_items[0] = (SimpleMenuItem) {
        .title = "Options",
        //.callback = &menu_option
    };
    menu_section1_items[1] = (SimpleMenuItem) {
        .title = "san - ebfe.pw",
        //.callback = &menu_option
    };
  
    menu_sections[0] = (SimpleMenuSection) {
        .title = "Current ",
        .items = menu_section0_items,
        .num_items = ARRAY_LENGTH(menu_section0_items)
    };
   menu_sections[1] = (SimpleMenuSection) {
        .title = "Options",
        .items = menu_section1_items,
        .num_items = ARRAY_LENGTH(menu_section1_items)
    };
   refreshMilkTime();
   refreshRecordTotal();
}


static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  GRect frame = layer_get_bounds(window_get_root_layer(s_window));
  init_menu_sections();
  menu_layer = simple_menu_layer_create(frame, s_window, menu_sections, 2, NULL);
 layer_add_child(window_get_root_layer(s_window),simple_menu_layer_get_layer(menu_layer));
}

static void destroy_ui(void) {
  window_destroy(s_window);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_mainmenu(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_mainmenu(void) {
  window_stack_remove(s_window, true);
}
