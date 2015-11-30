#include <pebble.h>
  
static Window *s_main_window;  
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap1;
static GBitmap *s_background_bitmap2;
static GBitmap *s_background_bitmap3;
int frameCount;
time_t lastStep;

static void update_time()
{
  //Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  double dtime = difftime(lastStep,temp);
  int roundTime = (dtime*100);
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Difference is now %d", roundTime);
  
  //Check if its time to update the picture
  if(difftime(temp,lastStep) > 0.6)
  {
    lastStep = temp;
    frameCount++;
    if(frameCount > 4)
    {
      frameCount = 1;
    }
    switch(frameCount){
      case 1:
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap1);
      break;
      case 3:
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap3);
      break;
      default:
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap2);
      break;
      
    }
  }
  
  //Create a long-lived buffer
  static char buffer[] = "00:00";
  
  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  //Display this time on the text layer
  //text_layer_set_text(s_time_layer, buffer);
  
}

static void main_window_load(Window *window)
{
  //Create GBitmaps and set in Bitmaplayer
  s_background_bitmap1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIMON);
  s_background_bitmap2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIMON_2);
  s_background_bitmap3 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIMON_3);
  frameCount = 0;
  
  s_background_layer = bitmap_layer_create(GRect(0, 10, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap1);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  //Create time Textlayer
  s_time_layer = text_layer_create(GRect(-25,140,139,50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  
  //Load Font
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_24));
  
  //Improve the layout to be more liek a watchface
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  

  //Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window)
{
  //Destroy GBitmaps
  gbitmap_destroy(s_background_bitmap1);
  gbitmap_destroy(s_background_bitmap2);
  gbitmap_destroy(s_background_bitmap3);
  
  //Destory BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  //Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  //Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  update_time();
}

static void init()
{
  //Create Window
  s_main_window = window_create();
  
  //Set handlers to manage the elements in the window
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  //Show the window on the watch with animated equals true
  window_stack_push(s_main_window, true);
  
  //Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  lastStep = time(NULL);
  localtime(&lastStep);
  
}

static void deinit()
{
  //Destroy window
  window_destroy(s_main_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}