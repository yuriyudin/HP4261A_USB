// MCU PINS
#define WTGT_n		2
#define FLAG_n		3
#define DTB1		4
#define DTB2		5
#define DTB4		6
#define DTB8		7
#define LED_GATE  13

// U3 TCA9555
#define U3_Addr     0x20
#define FNCA-R		0
#define FNCB-R		1
#define CMA-R		2
#define CMB-R		3
#define DRNA-R		4
#define DRNB-R		5
#define TL-R_n		6
#define FREQ1K-R	7
#define EXE_n		8
#define DRNC-R		9
#define DRNB-R		10
#define REM_n		11
#define INHIBIT_n	12

// U2 TCA9555
#define U2_Addr     0x21
#define CMS			0
#define OR			1
#define FREQ1K_n	2
#define FNCA		3
#define FNCB		4
#define DBLANK_n	5
#define RNGA		8
#define RNGB		9
#define RNGC		10

// Ranges

const char L_RANGE0[] PROGMEM = "UNDEFINED";
const char L_RANGE1[] PROGMEM = "100.0 uH";
const char L_RANGE2[] PROGMEM = "1000 uH";
const char L_RANGE3[] PROGMEM = "10.00 mH";
const char L_RANGE4[] PROGMEM = "100.0 mH";
const char L_RANGE5[] PROGMEM = "1000 mH";
const char L_RANGE6[] PROGMEM = "10.00 H";
const char L_RANGE7[] PROGMEM = "100.0 H";
const char L_RANGE8[] PROGMEM = "1000 H";
const char L_RANGE9[] PROGMEM = "AUTO";
const char * const L_RANGE_NAMES[] PROGMEM = {L_RANGE0, L_RANGE1, L_RANGE2, L_RANGE3, L_RANGE4, L_RANGE5, L_RANGE6, L_RANGE7, L_RANGE8, L_RANGE9};

const char C_RANGE0[] PROGMEM = "UNDEFINED";
const char C_RANGE1[] PROGMEM = "100.0 pF";
const char C_RANGE2[] PROGMEM = "1000 pF";
const char C_RANGE3[] PROGMEM = "10.00 nF";
const char C_RANGE4[] PROGMEM = "100.0 nF";
const char C_RANGE5[] PROGMEM = "1000 nF";
const char C_RANGE6[] PROGMEM = "10.00 uF";
const char C_RANGE7[] PROGMEM = "100 uF";
const char C_RANGE8[] PROGMEM = "1000 uF";
const char C_RANGE9[] PROGMEM = "10.00 mF";
const char C_RANGE10[] PROGMEM = "AUTO";
const char * const C_RANGE_NAMES[] PROGMEM = {C_RANGE0, C_RANGE1, C_RANGE2, C_RANGE3, C_RANGE4, C_RANGE5, C_RANGE6, C_RANGE7, C_RANGE8, C_RANGE9, C_RANGE10};

const char R_RANGE0[] PROGMEM = "UNDEFINED";
const char R_RANGE1[] PROGMEM = "1000 mOhm";
const char R_RANGE2[] PROGMEM = "10.00 Ohm";
const char R_RANGE3[] PROGMEM = "100.0 Ohm";
const char R_RANGE4[] PROGMEM = "1000 Ohm";
const char R_RANGE5[] PROGMEM = "10.00 kOhm";
const char R_RANGE6[] PROGMEM = "100.0 kOhm";
const char R_RANGE7[] PROGMEM = "1000 kOhm";
const char R_RANGE8[] PROGMEM = "10.00 MOhm";
const char R_RANGE9[] PROGMEM = "AUTO";
const char * const R_RANGE_NAMES[] PROGMEM = {R_RANGE0, R_RANGE1, R_RANGE2, R_RANGE3, R_RANGE4, R_RANGE5, R_RANGE6, R_RANGE7, R_RANGE8, R_RANGE9};


// Functions
//const String FUNCTION[] = {"C", "R", "UNDEFINED", "L"};
#define FUNC_C      0
#define FUNC_R      1
#define FUNC_UNDEF  2
#define FUNC_L      3

const char FUNC_NAME0[] PROGMEM = "C";
const char FUNC_NAME1[] PROGMEM = "R";
const char FUNC_NAME2[] PROGMEM = "UNDEFINED";
const char FUNC_NAME3[] PROGMEM = "L";
const char * const FUNC_NAMES[] PROGMEM = {FUNC_NAME0, FUNC_NAME1, FUNC_NAME2, FUNC_NAME3};

// Circuit Modes
//const String MODE[] = {"p", "s", "auto"};
#define MODE_PAR    0
#define MODE_SER    1
#define MOCE_AUTO   2

const char MODE_NAME0[] PROGMEM = "PAR";
const char MODE_NAME1[] PROGMEM = "SER";
const char MODE_NAME2[] PROGMEM = "AUTO";
const char * const MODE_NAMES[] PROGMEM = {MODE_NAME0, MODE_NAME1, MODE_NAME2};

// Measurement Frequency
//const String FREQ[] = {"1000 Hz", "120 Hz"};
#define FREQ_1000   0
#define FREQ_120    1

const char FREQ_NAME0[] PROGMEM = "1000";
const char FREQ_NAME1[] PROGMEM = "120";
const char * const FREQ_NAMES[] PROGMEM = {FREQ_NAME0, FREQ_NAME0};

// D Blank
//const String DBLANK[] = {"FALSE", "TRUE"};

// Out of Range and D Blank
//const String OOR[] = {"FALSE", "TRUE"};
#define GEN_FALSE  0
#define GEN_TRUE   1
#define GEN_OOR    2

const char GEN_NAME0[] PROGMEM = "FALSE";
const char GEN_NAME1[] PROGMEM = "TRUE";
const char GEN_NAME1[] PROGMEM = "OUT OF RANGE";
const char * const GEN_NAMES[] PROGMEM = {GEN_NAME0, GEN_NAME1, GEN_NAME2};


// Command Arguments
#define ARG_MODE    0
#define ARG_FREQ    1
#define ARG_FUNC    2
#define ARG_RNG     3
#define ARG_LEV     4
#define ARG_REM     5
#define ARG_MEAS    6

const char ARG0[] PROGMEM = "MODE";
const char ARG1[] PROGMEM = "FREQ";
const char ARG2[] PROGMEM = "FUNC";
const char ARG3[] PROGMEM = "RNG";
const char ARG4[] PROGMEM = "LEV";
const char ARG5[] PROGMEM = "REM";
const char ARG6[] PROGMEM = "MEAS";
const char * const ARG_NAMES[] PROGMEM = {ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6};


// Texts
// https://forum.arduino.cc/t/how-to-store-a-string-in-arduinos-flash-memory-and-then-retrieve-it/627725/11
#define HEADER  0
#define HELP      1
#define IDN       2
#define SPACE      12

const char text0[] PROGMEM = \
"************ HP4261A USB ADAPTER ************\n\r\
*  Author\: Yuri Yudin (yyudin@hotmail.com)  *\n\r\
* https://github.com/yuriyudin/HP4261A_USB  *\n\r\
*     TYPE \"HELP\" FOR LIST OF COMMANDS      *\n\r\
*********************************************\n\r";
const char text1[] PROGMEM = "Long and detailed list of all commands help";
const char text2[] PROGMEM = "HEWLETT PACKARD 4261A LCR METER";
const char text3[] PROGMEM = "FUNCTION/MODE: ";
const char text4[] PROGMEM = "MEAS. FREQUENCY: ";
const char text5[] PROGMEM = "MEAS. RANGE: ";
const char text6[] PROGMEM = "LCR DATA: ";
const char text7[] PROGMEM = "DISSIP. FACTOR: ";
const char text8[] PROGMEM = "-----------------------------------";
const char text9[] PROGMEM = "OK: ";
const char text10[] PROGMEM = "ERROR: ";
const char text11[] PROGMEM = ":SET ";
const char text12[] PROGMEM = " ";
const char text13[] PROGMEM = ":SET? ";
const char * const TEXTS[] PROGMEM = {text0, text1, text2, text3, text4, text5, text6, text7, text8, text9, text10, text11, text12, text13};
