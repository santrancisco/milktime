#include "wakeupalert.h"
#include <pebble.h>
#include "common.h"

static Window *s_window;
static GFont s_res_gothic_18_bold;
static GBitmap *s_res_milk_icon;
static TextLayer *s_textlayer_1;
static TextLayer *amount_textlayer;
static TextLayer *nappie_textlayer;
static BitmapLayer *s_bitmaplayer_1;

static char* STR_AMOUNT="Amount:";
static char* STR_ML="ml";
static char* STR_HEAVY="Heavy";
static char* STR_LIGHT="Light";

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_res_milk_icon = gbitmap_create_with_resource(RESOURCE_ID_MILK_ICON);
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(25, 10, 100, 20));
  text_layer_set_text(s_textlayer_1, "Time to feed!");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  // amount_textlayer
  amount_textlayer = text_layer_create(GRect(10, 40, 119, 20));
  text_layer_set_text(amount_textlayer, "Amount: ");
  text_layer_set_font(amount_textlayer, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)amount_textlayer);
  
  // nappie_textlayer
  nappie_textlayer = text_layer_create(GRect(10, 70, 100, 32));
  text_layer_set_text(nappie_textlayer, "Nappie: ");
  text_layer_set_font(nappie_textlayer, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)nappie_textlayer);
  
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(21, 7, 16, 21));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_milk_icon);
  bitmap_layer_set_background_color(s_bitmaplayer_1, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(amount_textlayer);
  text_layer_destroy(nappie_textlayer);
  bitmap_layer_destroy(s_bitmaplayer_1);
  gbitmap_destroy(s_res_milk_icon);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void refreshScreen(void){
    //NEXT milk time prediction text
    int hours = persist_read_int(PREVIOUS_MILKTIME_PKEY)/60;
    int minutes = persist_read_int(PREVIOUS_MILKTIME_PKEY)%60;
    static char amount_text[20];
    static char nappie_text[20];
    amount_text[0]='\0';
    nappie_text[0]='\0';
    int amount = persist_read_int(PREVIOUS_AMOUNT_PKEY);
    snprintf(amount_text,sizeof(amount_text),"Amount: %d", amount);  
    if(persist_read_bool(PREVIOUS_NAPPIE_PKEY)){
      append_string(nappie_text, sizeof(nappie_text), "Nappie: Soiled");
    } else {
      append_string(nappie_text, sizeof(nappie_text), "Nappie: Wet");
    }
    text_layer_set_text(amount_textlayer, amount_text);
    text_layer_set_text(nappie_textlayer, nappie_text);
}

void show_wakeupalert(WakeupId id, int32_t reason) {
  persist_write_int(PREVIOUS_MILKTIME_PKEY, persist_read_int(MILKTIME_PKEY));
  persist_write_int(PREVIOUS_AMOUNT_PKEY, persist_read_int(AMOUNT_PKEY));
  persist_write_bool(PREVIOUS_NAPPIE_PKEY, persist_read_int(NAPPIE_PKEY));
  initialise_ui();
  refreshScreen();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
  persist_delete(PERSIST_KEY_WAKEUP_ID);
  vibes_double_pulse();
  vibes_double_pulse();
}

void hide_wakeupalert(void) {
  window_stack_remove(s_window, true);
}
