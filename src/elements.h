#define ANIM_DURATION 1000
#define ANIM_DELAY 0

static Window *s_main_window;
static Layer *s_background_layer, *s_time_layer, *s_sleep_layer;

static PropertyAnimation *s_sleep_animation;

static AppTimer *timer = NULL;

static int8_t Y;
static int8_t X;
static int8_t column;
static int8_t row[6];

static int8_t hour;
static int8_t hour_24;
static int8_t hour1;
static int8_t hour2;
static int8_t minute1;
static int8_t minute2;
static int8_t second1;
static int8_t second2;

static bool sleep;
static bool sleeping = false;
static bool first_run = true;
static bool do_reverse = false;

static int8_t bat;

static void do_animation();
static void timer_callback(void *data);