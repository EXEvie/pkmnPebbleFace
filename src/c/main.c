#include <pebble.h>
/* 
Size of screen - 144 x 168 pixels
*/

/*
Current issues:

*/

  
 
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static BitmapLayer *s_forward_layer;
static GBitmap *s_forward_bitmap;
static BitmapLayer *s_backward_layer;
static GBitmap *s_backward_bitmap;
static TextLayer *s_time_layer;
static TextLayer *s_pebble_batt;
static TextLayer *s_front_layer;
static TextLayer *s_date_layer;
static TextLayer *s_back_layer;
static Window *s_main_window;

uint32_t pkmn_key = 2;

static char s_battery_buffer[32];
int batt_percent = 100;


//flags
int batt_check_flag = 0; //checks if battery % loaded yet
int eevee_layer_check = 0; //checks if Eevee text/image created
int starter_layer_flag = 0; //checks if starter pkmn layer created
int flag_bluetooth_connect = 1;
int flag_bluetooth_disconnect = 0;


/*
Starter inits
*/
//0 for bulbasaur, 1 for squirtle, 2 for charmander
int pkmn_choice = 2;
//0 for starter, 1 for mid, 2 for final evo
int pkmn_count;




char starter_pkmn_text[3][3][12] ={
  {"Bulbasaur","Ivysaur","Venusaur"},
  {"Squirtle","Wartortle","Blastoise"},
  {"Charmander","Charmeleon","Charizard"},  
}; 

char starter_pkmn_image[3][3] = {
  {RESOURCE_ID_Bulbasaur_back,RESOURCE_ID_Ivysaur_back,RESOURCE_ID_Venusaur_back},
  {RESOURCE_ID_Squirtle,RESOURCE_ID_Wartortle,RESOURCE_ID_Blastoise},
  {RESOURCE_ID_Charmander,RESOURCE_ID_Charmeleon,RESOURCE_ID_Charizard}
};

/*
Eevee inits
*/

static char Eeveelution;
static char Eevee_text[60];
static int umb_check = 0;
static int espeon_check = 0;
static int jolt_check = 0;
static int Kuriboh = 0;
static int Missingno = 0;

static Layer *window_layer;
//int x;


/*
Updating images and text
*/

static void starter_evolution(){
//handles generation of starter pkmn image and text
  //Destroy image layer if it already exists, otherwise memory overflow issues
  if (starter_layer_flag == 1){
    gbitmap_destroy(s_backward_bitmap);
  }
  
  if(persist_exists(pkmn_key)) {
    // Read persisted value
    pkmn_choice = persist_read_int(pkmn_key);
  } 
  else {
    // Choose a default value
    pkmn_choice = 2;
  }
  
  //Bit of maths to make evolution states take up ~1/3 of battery life each
  snprintf(s_battery_buffer, sizeof(s_battery_buffer),"%s \n L %d",starter_pkmn_text[pkmn_choice][pkmn_count],(100-batt_percent)/2 + 1);
  text_layer_set_text(s_pebble_batt, s_battery_buffer);
  APP_LOG(APP_LOG_LEVEL_INFO, "battery layer set.");

  s_backward_layer = bitmap_layer_create(GRect(7,62,58,60));
  layer_add_child(window_layer,bitmap_layer_get_layer(s_backward_layer));
  s_backward_bitmap = gbitmap_create_with_resource(starter_pkmn_image[pkmn_choice][pkmn_count]);
  bitmap_layer_set_alignment(s_backward_layer, GAlignBottom);
  bitmap_layer_set_bitmap(s_backward_layer, s_backward_bitmap);
  
  starter_layer_flag = 1;
}


static void Eevee(){//Eevee function checks for states defined by time, charge state etc and assigns name and image based on them
  if (eevee_layer_check ==1){
    gbitmap_destroy(s_forward_bitmap);
    //text_layer_destroy(s_front_layer); //Somehow destroying txt layer prevents new one from appearing?
  }
  if(Kuriboh==1){
    snprintf(Eevee_text,60,"Kuriboh    ");
    Eeveelution = RESOURCE_ID_Kuriboh;
  }
  else{
    if(jolt_check ==1){
    //unload eeveelution?
      snprintf(Eevee_text,60,"Jolteon    L30");
      Eeveelution = RESOURCE_ID_Jolteon_front;
    }
    else{
      if(Missingno ==1){
        snprintf(Eevee_text,60,"Missingno L121");
        Eeveelution = RESOURCE_ID_Missingno;
      }
      else{
        if(umb_check ==1){
          snprintf(Eevee_text,60,"Umbreon    L35");
          Eeveelution = RESOURCE_ID_Umbreon_front;
        }
        else{
          if(espeon_check ==1){
            snprintf(Eevee_text,60,"Espeon    L35");
            Eeveelution = RESOURCE_ID_Espeon;
          }
          else{
            snprintf(Eevee_text,60,"Eevee      L20");
            Eeveelution = RESOURCE_ID_Eevee_front; 
          }             
        }
      }
    }
  }
  //Eevee text
  s_front_layer = text_layer_create(GRect(10,3,120,60));
  layer_add_child(window_layer, text_layer_get_layer(s_front_layer));
  text_layer_set_background_color(s_front_layer,GColorClear);
  text_layer_set_text_color(s_front_layer, GColorBlack);
  text_layer_set_font(s_front_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_front_layer, GTextAlignmentLeft);
  text_layer_set_text(s_front_layer,Eevee_text);
  
  
  
  //Eevee image layer
  s_forward_layer = bitmap_layer_create(GRect(84,9,58,60));
  layer_add_child(window_layer,bitmap_layer_get_layer(s_forward_layer));
  APP_LOG(APP_LOG_LEVEL_INFO, "Eevee layer loaded");
  s_forward_bitmap = gbitmap_create_with_resource(Eeveelution);
  bitmap_layer_set_bitmap(s_forward_layer, s_forward_bitmap);
  APP_LOG(APP_LOG_LEVEL_INFO, "Eevee fine post-picture.");
    
  eevee_layer_check = 1;
}


/*
Configuration and communication
*/

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Read pkmn preferences
  Tuple *pkmn_choice_tup = dict_find(iter, MESSAGE_KEY_pkmn_choice_in);
  if(pkmn_choice_tup){
    pkmn_choice = pkmn_choice_tup->value->int8-48;
  }
  APP_LOG(APP_LOG_LEVEL_INFO,"pkmn_choice = %d",pkmn_choice);
  persist_write_int(pkmn_key, pkmn_choice);
  if(jolt_check==0){
    starter_evolution();
  }
  
}


/*
Updating time/date/battery
*/

//Updates time/date  
static void update_time(){
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char buffer[] = "00:00";
  static char hour[] = "00";
  //create buffer
  static char date_buffer[] = "00 / 00";
  //write current time into buffer
  strftime(buffer, sizeof(buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  strftime(hour,sizeof(hour),"%H",tick_time);
  text_layer_set_text(s_time_layer, buffer);
  strftime(date_buffer, sizeof("00 / 00"), "%d / %m",tick_time);//date in dd/mm format
  text_layer_set_text(s_date_layer, date_buffer);
  if (strncmp(date_buffer,"01 / 04",7)==0){
    Kuriboh = 1;
  }
  else{
    Kuriboh = 0;
  }
  
  int front_level = atoi(hour);//atoi(buffer + strlen("00:00"));
  if((front_level < 6)||(front_level > 19)){
    umb_check = 1;
  }
  else{
    if((front_level <15)&&(front_level >=11)){
      espeon_check = 1;
    }
    else{
      umb_check = 0;
      espeon_check = 0;
    }
  }
  if(jolt_check==0){
    Eevee();
    starter_evolution();//called every time time updates (once per minute), so won't update Jolteon until next minute change 
  }
}



//Handles time
static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}



//Handles Battery level display and corresponding evolution of starter pkmn. If charging, shows Jolteon
static void pebble_battery(BatteryChargeState charge_state){
  
  APP_LOG(APP_LOG_LEVEL_INFO, "battery_buffer_created.");
  
  
  batt_percent = charge_state.charge_percent;
  batt_check_flag = 1;
  
  if(charge_state.is_plugged){
    if(jolt_check ==0){
      jolt_check = 1;
      pkmn_count = 0;
      Eevee();
    }
    else{
      jolt_check = 1;
      pkmn_count = 0;
    }
  }
  //Runs Eevee() only if the last check showed that watch was charging
  else{
    if(jolt_check ==1){
      jolt_check = 0;
      Eevee();
    }
    jolt_check = 0;
    
    if (charge_state.charge_percent > 70){
      pkmn_count = 0;
    }
    else{
      if (charge_state.charge_percent > 30){
        pkmn_count = 1;
      }
      else{
        pkmn_count = 2;
      }
    }
    
  }
}

//Checks bluetooth connection, shows Missingno if not connected
static void connection_handler(bool connected){
  if (connected){
    flag_bluetooth_connect = 1;
    Missingno = 0;
    /*if(flag_bluetooth_disconnect == 1){
      Eevee();
      flag_bluetooth_disconnect = 0;
    }*/
  }
  else{
    if(jolt_check ==0){
      flag_bluetooth_disconnect = 1;
      Missingno = 1;
       /*if(flag_bluetooth_connect == 1){
        Eevee();
        flag_bluetooth_connect = 0;
      }*/
    }
  }
 
}






static void main_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Main window load starts.");
  window_layer = window_get_root_layer(window);
  layer_mark_dirty(window_layer);
  APP_LOG(APP_LOG_LEVEL_INFO, "Main window marked dirty");
  
  //Main image layer.  
  s_background_layer = bitmap_layer_create(GRect(0,0,144,168));//x position, y position, width, height
  layer_add_child(window_layer,bitmap_layer_get_layer(s_background_layer));
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_image_pokemon_main);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  APP_LOG(APP_LOG_LEVEL_INFO, "bitmap loaded.");
  
  
  
  //Eevee image layer
  s_forward_layer = bitmap_layer_create(GRect(84,9,58,60));
  layer_add_child(window_get_root_layer(window),bitmap_layer_get_layer(s_forward_layer));
  s_forward_bitmap = gbitmap_create_with_resource(Eeveelution);
  bitmap_layer_set_bitmap(s_forward_layer, s_forward_bitmap);
  APP_LOG(APP_LOG_LEVEL_INFO, "Eevee layer loaded");
  
  //Eevee text
  s_front_layer = text_layer_create(GRect(10,3,120,60));
  layer_add_child(window_layer, text_layer_get_layer(s_front_layer));
  text_layer_set_background_color(s_front_layer,GColorClear);
  text_layer_set_text_color(s_front_layer, GColorBlack);
  text_layer_set_font(s_front_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_front_layer, GTextAlignmentLeft);
  text_layer_set_text(s_front_layer,Eevee_text);
  
  eevee_layer_check = 1;
  
  //Starter pkmn image layer
  s_backward_layer = bitmap_layer_create(GRect(7,62,58,60));
  layer_add_child(window_layer,bitmap_layer_get_layer(s_backward_layer));
  s_backward_bitmap = gbitmap_create_with_resource(starter_pkmn_image[pkmn_choice][pkmn_count]);
  bitmap_layer_set_alignment(s_backward_layer, GAlignBottom);
  bitmap_layer_set_bitmap(s_backward_layer, s_backward_bitmap);
  APP_LOG(APP_LOG_LEVEL_INFO, "Starter layer loaded");
  
   //Pebble battery/starter pkmn text layer
  s_pebble_batt = text_layer_create(GRect(75,66,65,60));
  text_layer_set_background_color(s_pebble_batt,GColorClear);
  text_layer_set_text_color(s_pebble_batt, GColorBlack);
  text_layer_set_font(s_pebble_batt,fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_pebble_batt, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_pebble_batt));
  APP_LOG(APP_LOG_LEVEL_INFO, "battery layer.");
  
  starter_layer_flag = 1;
  
  //time layer
  s_time_layer = text_layer_create(GRect(50,125,100,100));
  text_layer_set_background_color(s_time_layer,GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer,fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));//formatting
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer)); //add as layer
  APP_LOG(APP_LOG_LEVEL_INFO, "time layer");
  
  
   //date
  s_date_layer = text_layer_create(GRect(95,100,40,40));
  text_layer_set_background_color(s_date_layer,GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
 
  //check pebble battery and bluetooth connection on screen load
  pebble_battery(battery_state_service_peek());
  connection_handler(connection_service_peek_pebble_app_connection());
  update_time();
   
 
  APP_LOG(APP_LOG_LEVEL_INFO, "Main window loaded.");
}

static void main_window_unload(Window *window){
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  gbitmap_destroy(s_forward_bitmap);
  bitmap_layer_destroy(s_forward_layer);
  gbitmap_destroy(s_backward_bitmap);
  bitmap_layer_destroy(s_backward_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_pebble_batt);
  text_layer_destroy(s_front_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_back_layer);
  //layer_destroy(window_layer);
  APP_LOG(APP_LOG_LEVEL_INFO, "Main window unloaded.");
}
  
static void init() {
    

  // Open AppMessage connection
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(8, 8);


  s_main_window = window_create();//create main window element
  APP_LOG(APP_LOG_LEVEL_INFO, "Main window created.");
  
  //set handlers to manage window elements
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
  APP_LOG(APP_LOG_LEVEL_INFO, "stack push fine");
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);//register with time-telling thing
  battery_state_service_subscribe(pebble_battery);//updates when battery level changes
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = connection_handler,
  });
   
}

static void deinit() {
  
  window_destroy(s_main_window);//match every create with a destroy  
  connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}