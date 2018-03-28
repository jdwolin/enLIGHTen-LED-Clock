// Neopixel
#define PIN 4        // PIN where neopixel / WS2811 strip is attached
#define PINS 2        // PIN where neopixel / WS2811 strip is attached
#define NUMLEDS 100    // Number of leds in the strip
#define NUMLEDSECS 120   // Number of leds in the strip
const char HOSTNAME[] = "clock";   // Friedly hostname
// parameters for automatically cycling favorite patterns
uint32_t autoParams[][4] = {   // color, speed, mode, duration (seconds)
  {0xff0000, 200,  1,  5.0}, // blink red for 5 seconds
  {0x00ff00, 200,  3, 10.0}, // wipe green for 10 seconds
  {0x0000ff, 200, 11,  5.0}, // dual scan blue for 5 seconds
  {0x0000ff, 200, 42, 15.0}  // fireworks for 15 seconds
};

// ***************************************************************************
// Global variables / definitions
// ***************************************************************************
#define DBG_OUTPUT_PORT Serial  // Set debug output port

// List of all color modes
enum MODE { SET_MODE, HOLD, OFF, ALL, WIPE, RAINBOW, RAINBOWCYCLE, THEATERCHASE, THEATERCHASERAINBOW, TV, CUSTOM, WEATHER };
MODE mode = RAINBOW;        // Standard mode that is active when software starts
int ws2812fx_speed = 196;   // Global variable for storing the delay between color changes --> smaller == faster
int brightness = 196;       // Global variable for storing the brightness (255 == 100%)
int ws2812fx_mode = 0;      // Helper variable to set WS2812FX modes
bool exit_func = false;     // Global helper variable to get out of the color modes when mode changes
bool shouldSaveConfig = false;  // For WiFiManger custom config
int rotationcounter = 0;
int minchangedreset = 0;

struct ledstate             // Data structure to store a state of a single led
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

typedef struct ledstate LEDState;     // Define the datatype LEDState
LEDState ledstates[NUMLEDS];          // Get an array of led states to store the state of the whole strip
LEDState main_color = {0,0,250};  // Store the "main color" of the strip used in single color modes
LEDState hournum_color = {250,250,250};
LEDState hour_color = {0,0,250};
LEDState min_color = {0,250,0};
LEDState sec_color = {250,250,250};
LEDState alt_color = {0,250,0};
LEDState sweep_color = {0,0,250};
#define ENABLE_STATE_SAVE             // If defined, save state on reboot
#ifdef ENABLE_STATE_SAVE
char current_state[128];               // Keeps the current state representation
char last_state[128];                  // Save the last state as string representation
unsigned long time_statechange = 0;   // Time when the state last changed
int timeout_statechange_save = 5000;  // Timeout in ms to wait before state is saved
bool state_save_requested = false;    // State has to be saved after timeout
int dimflag = 0, arcflag = 1, dayflag = 0, storebrightness = 240;
#endif


