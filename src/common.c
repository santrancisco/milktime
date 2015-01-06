#include <pebble.h>
#include "common.h"


size_t append_number(char* words, int num) {
  char inttochar[20];
  snprintf(inttochar,sizeof(inttochar),"%d",num);
  strncat(words, inttochar, strlen(inttochar));
  size_t written = strlen(inttochar);
  return written;
}

size_t append_string(char* buffer, const size_t length, const char* str) {
  strncat(buffer, str, length);
  size_t written = strlen(str);
  return (length > written) ? written : length;
}

bool historyIsFull(){
  return persist_exists(persist_read_int(MIN_FEED_PKEY)+MAX_NUMBER_OF_FEED_DEFAULT - 1);
}

void setNextFeedTime(){
  int prediction;
  if(persist_read_bool(OPTION_USER_GAP_USAGE_PKEY)){
    prediction = persist_read_int(persist_read_int(LAST_FEED_PKEY)) + persist_read_int(OPTION_USER_GAP_PKEY);
  } else  {
    prediction = persist_read_int(persist_read_int(LAST_FEED_PKEY)) + persist_read_int(MILKTIME_GAP_PKEY);
  }
  if (prediction > 1439){
    prediction -= 1440;
  }
  persist_write_int(MILKTIME_PKEY,prediction);
}

void calculateAverageGap(){ // NEED at least
  if(persist_exists(MIN_FEED_PKEY+1)){
    int mingap = persist_read_int(OPTION_MAXGAP_PKEY);
    int minFeed = persist_read_int(MIN_FEED_PKEY);
    int maxnumberofFeed = MAX_NUMBER_OF_FEED_DEFAULT;
    int gap=0;
    int gap_sum = 0;
    int gap_counter = 0;
    int counter = 0; 
    int feed_i = persist_read_int(CURRENT_FEED_PKEY) - 1;
    int newer = feed_i;
    while ((feed_i > minFeed) && (counter < 10)) { 
      if((feed_i == minFeed)&&(historyIsFull())){
        feed_i = minFeed + maxnumberofFeed - 1;
      } else {
        feed_i--;
      }
      gap = persist_read_int(newer) - persist_read_int(feed_i);
      if(gap < 0){  // If it's rolled over to the next day.
        gap += 1440;
      }
      if(gap < mingap){
        gap_sum += gap;
        gap_counter++;
      }
      counter++;
      newer = feed_i;
  }
    
  int avg = gap_sum/gap_counter;
  persist_write_int(MILKTIME_GAP_PKEY, avg);
  } else {
    persist_write_int(MILKTIME_GAP_PKEY, MILKTIME_GAP_DEFAULT);
  }
}

//scheduling alarm for the next Milk time ;)
void scheduleAlarm(){
  static WakeupId wakeup_id;
  struct tm *timeNow;
  time_t now = time(NULL);
  timeNow = localtime(&now);  
  int timeInMin = (60*timeNow->tm_hour)+timeNow->tm_min;
  int schedulegap = persist_read_int(MILKTIME_PKEY) - timeInMin;
  if(schedulegap<0){
    schedulegap+=1440;
  }
  time_t future_time = now + 60*(schedulegap);
  wakeup_cancel_all();
  persist_delete(PERSIST_KEY_WAKEUP_ID);
  wakeup_id = wakeup_schedule(future_time, WAKEUP_REASON, true);
  persist_write_int(PERSIST_KEY_WAKEUP_ID, wakeup_id);
}

void predictNextFeedTime(){ //If at least 2 entry already exist then proceed.
  calculateAverageGap();
  setNextFeedTime();
  scheduleAlarm();
}



void predictNextFeed(){
  if(persist_exists(MIN_FEED_PKEY+1)){
  int nextFeedTime = persist_read_int(MILKTIME_PKEY);
  int currentFeed = persist_read_int(CURRENT_FEED_PKEY);
  int maxnumberofFeed = MAX_NUMBER_OF_FEED_DEFAULT;
  int minFeed = persist_read_int(MIN_FEED_PKEY);
  int amountSum = 0;
  int nappie_tolerance = 30;
  int nappie_counter = 0;
  int amount_counter = 0;
  int size = MAX_NUMBER_OF_FEED_DEFAULT;
  
  if (!historyIsFull()) {
      size = currentFeed - minFeed;  
    }
  int counter = 0;
  int feed_i = currentFeed-1;
  while (counter < size){ 
    if (counter<10){
        amountSum += persist_read_int(feed_i+FEEDTOAMOUNT);
        amount_counter++;
    }
    //If the feed time is between +- 40 minutes, considering it for the next nappie state.
    if((persist_read_int(feed_i) > (nextFeedTime - nappie_tolerance)) && (persist_read_int(feed_i) < (nextFeedTime + nappie_tolerance))){
       if(persist_read_bool(feed_i + FEEDTONAPPIE)){
         nappie_counter++;
       } else {
         nappie_counter--;
       }
    }
    
    if(feed_i < minFeed){
      feed_i = minFeed + maxnumberofFeed - 1;
    } else {
      feed_i--;
    }
    counter++;
  }
  persist_write_int(AMOUNT_PKEY, 10*((amountSum/amount_counter)/10));
  if(nappie_counter > 0){
    persist_write_bool(NAPPIE_PKEY, true);
  } else {
    persist_write_bool(NAPPIE_PKEY, false);
  }
  }
}