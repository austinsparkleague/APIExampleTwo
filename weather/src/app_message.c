#include <pebble.h>

static Window *s_window;	
static TextLayer *s_text_layer;
static GFont s_time_font;

	
// Keys for AppMessage Dictionary
// These should correspond to the values you defined in appinfo.json/Settings
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1
};

// Write message to buffer & send
static void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_cstring(iter, MESSAGE_KEY, "I'm a Pebble!");
	
	dict_write_end(iter);
  app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
    text_layer_set_text(s_text_layer, tuple->value->cstring);
	}
  
  send_message();
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void init(void) {
	s_window = window_create();
	window_stack_push(s_window, true);
  
      // Get the right font to display on the watchface
  s_time_font = fonts_get_system_font(FONT_KEY_GOTHIC_28);

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_text_layer = text_layer_create(bounds);
  text_layer_set_text(s_text_layer, PBL_IF_RECT_ELSE("Loading...", "Loading..."));
  text_layer_set_font(s_text_layer, s_time_font);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
#if PBL_ROUND
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_enable_screen_text_flow_and_paging(s_text_layer, 8);
#endif
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);

  // Initialize AppMessage inbox and outbox buffers with a suitable size
  const int inbox_size = 128;
  const int outbox_size = 128;
	app_message_open(inbox_size, outbox_size);
}

static void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(s_window);
}

int main( void ) {
	init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting");
	app_event_loop();
	deinit();
}