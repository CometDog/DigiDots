static Window *s_main_window;
static Layer *s_background_layer, *s_time_layer, *s_sleep_layer;

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

static int8_t bat;