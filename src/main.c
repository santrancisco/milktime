#include "pebble.h"
#include "common.h"
#include "mainmenu.h"
#include "wakeupalert.h"

static void init(void) {
  if(launch_reason() != APP_LAUNCH_WAKEUP){
  if(!persist_exists(MILKTIME_PKEY)){
    persist_write_int(MILKTIME_PKEY,0);
    persist_write_int(MILKTIME_GAP_PKEY, MILKTIME_GAP_DEFAULT);
    persist_write_int(AMOUNT_PKEY, AMOUNT_DEFAULT);
    persist_write_bool(NAPPIE_PKEY, false);
    
    persist_write_int(PREVIOUS_MILKTIME_PKEY, MILKTIME_GAP_DEFAULT);
    persist_write_int(PREVIOUS_AMOUNT_PKEY, AMOUNT_DEFAULT);
    persist_write_bool(PREVIOUS_NAPPIE_PKEY, false);
    
    persist_write_int(CURRENT_FEED_PKEY, CURRENT_FEED_DEFAULT);
    persist_write_int(MIN_FEED_PKEY,MIN_FEED_DEFAULT );
    persist_write_int(MAX_FEED_PKEY,MAX_NUMBER_OF_FEED_DEFAULT );
    persist_write_int(OPTION_MAXGAP_PKEY,OPTION_MAXGAP_DEFAULT);
    persist_write_int(OPTION_USER_GAP_PKEY,MILKTIME_GAP_DEFAULT);
    persist_write_bool(OPTION_USER_GAP_USAGE_PKEY,false);
  }
  show_mainmenu();
  //action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  //action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);
  // Get the count from persistent storage for use if it exists, otherwise use the default
  //num_drinks = persist_exists(NUM_DRINKS_PKEY) ? persist_read_int(NUM_DRINKS_PKEY) : NUM_DRINKS_DEFAULT;
  } else {
    WakeupId id = 0;
    int32_t reason = 0;
    wakeup_get_launch_event(&id, &reason);
    show_mainmenu();
    show_wakeupalert(id, reason);
    // LAUNCH WHEN WAKE UP BY TIMER ---> TELL USER HOW MANY MIL to feed.
  }
}

static void deinit(void) {
  // Save the count into persistent storage on app exit
 // persist_write_int(NUM_DRINKS_PKEY, num_drinks);
  //window_destroy(window);
  //gbitmap_destroy(action_icon_plus);
  //gbitmap_destroy(action_icon_minus);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}