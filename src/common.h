#include <stddef.h> 
#include <stdbool.h>
#ifndef _common_h
#define _common_h
  
//Persistance storage key values
#define MILKTIME_PKEY 1
#define AMOUNT_PKEY 2
#define NAPPIE_PKEY 3  
#define REGISTERING_TIME_PKEY 4
#define MILKTIME_GAP_PKEY 5
#define LAST_FEED_PKEY 6
#define OPTION_MAXGAP_PKEY 7
#define OPTION_USER_GAP_PKEY 8
#define OPTION_USER_GAP_USAGE_PKEY 9
  
#define PREVIOUS_MILKTIME_PKEY 11
#define PREVIOUS_AMOUNT_PKEY 12
#define PREVIOUS_NAPPIE_PKEY 13
  

  
#define CURRENT_FEED_PKEY 100
#define MIN_FEED_PKEY  101
#define MAX_FEED_PKEY  102


// Notes: 200 --> 250 will be used for feeding time for each feed
// Notes: 300 --> 350 will be used for feed amount for each feed
// Notes: 400 --> 350 will be used for nappie state for each feed

// DEFAULT values and other constants
#define WAKEUP_REASON 0
#define PERSIST_KEY_WAKEUP_ID 23
  
#define OPTION_MAXGAP_DEFAULT 240
#define MILKTIME_GAP_DEFAULT 180
#define AMOUNT_DEFAULT 80  
#define MAX_NUMBER_OF_FEED_DEFAULT 10
#define MIN_FEED_DEFAULT 200
#define CURRENT_FEED_DEFAULT 200
#define FEEDTOAMOUNT 100
#define FEEDTONAPPIE 200
#define AMOUNT_AVERAGE 10
#define NAPPIE_AVERAGE 5
  
  bool historyIsFull();
  size_t append_number(char* words, int num);
  size_t append_string(char* buffer, const size_t length, const char* str);
  void predictNextFeed();
  void predictNextFeedTime();
#endif