/*
  File name: HP4261A.h
  Version: 1.0.0
  Purpose: Firmware for the Hewlett Packard 4261A LCR meter USB adapter

  Documentation:
  https://github.com/yuriyudin/HP4261A_USB

  Copyright (c) 2023 Yuri Yudin (yyudin@hotmail.com)
 
  License:
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Dependencies:
    Arduino Command Parser https://www.arduino.cc/reference/en/libraries/commandparser/
                           https://github.com/Uberi/Arduino-CommandParser
    Arduino library for I2C TCA9555 https://www.arduino.cc/reference/en/libraries/tca9555/
                                    https://github.com/RobTillaart/TCA9555
*/        

// MCU PINS
#define WTGT_n	  2
#define FLAG_n	  3
#define DTB1		  4
#define DTB2		  5
#define DTB4		  6
#define DTB8		  7
#define LED_GATE  13

// U3 TCA9555
#define U3_Addr   0x20
#define FNCA_R		0
#define FNCB_R		1
#define CMA_R		  2
#define CMB_R		  3
#define DRNA_R		4
#define DRNB_R		5
#define TL_R_n		6
#define FREQ1K_R	7
#define EXE_n		  8
#define DRNC_R		9
#define DRNB_R		10
#define REM_n		  11
#define INHIBIT_n	12

// U2 TCA9555
#define U2_Addr   0x21
#define CMS			  0
#define OR			  1
#define FREQ1K_n	2
#define FNCA		  3
#define FNCB		  4
#define DBLANK_n	5
#define RNGA		  8
#define RNGB		  9
#define RNGC		  10

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
const char L_RANGE10[] PROGMEM = "AUTO";
const char * const L_RANGE_NAMES[] PROGMEM = {L_RANGE0, L_RANGE1, L_RANGE2, L_RANGE3, L_RANGE4, L_RANGE5, L_RANGE6, L_RANGE7, L_RANGE8, L_RANGE9, L_RANGE10};

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
#define MODE_PAR    0
#define MODE_SER    1
#define MODE_AUTO   2

const char MODE_NAME0[] PROGMEM = "PAR";
const char MODE_NAME1[] PROGMEM = "SER";
const char MODE_NAME2[] PROGMEM = "AUTO";
const char * const MODE_NAMES[] PROGMEM = {MODE_NAME0, MODE_NAME1, MODE_NAME2};

// Measurement Frequency
#define FREQ_1000   0
#define FREQ_120    1

const char FREQ_NAME0[] PROGMEM = "1000";
const char FREQ_NAME1[] PROGMEM = "120";
const char * const FREQ_NAMES[] PROGMEM = {FREQ_NAME0, FREQ_NAME1};

// Measurement Level
#define LEV_1000   0
#define LEV_50     1

const char LEV_NAME0[] PROGMEM = "1000";
const char LEV_NAME1[] PROGMEM = "50";
const char * const LEV_NAMES[] PROGMEM = {LEV_NAME0, LEV_NAME1};

// Remote Control
#define REM_ON     0
#define REM_OFF    1

const char REM_NAME0[] PROGMEM = "ON";
const char REM_NAME1[] PROGMEM = "OFF";
const char * const REM_NAMES[] PROGMEM = {REM_NAME0, REM_NAME1};

// Measurement Regime
#define MEAS_CONT    0
#define MEAS_SGL     1

const char MEAS_NAME0[] PROGMEM = "CONT";
const char MEAS_NAME1[] PROGMEM = "SGL";
const char MEAS_NAME2[] PROGMEM = "ABORT";
const char * const MEAS_NAMES[] PROGMEM = {MEAS_NAME0, MEAS_NAME1, MEAS_NAME2};

// D Blank
//const String DBLANK[] = {"FALSE", "TRUE"};

// Out of Range and D Blank
//const String OOR[] = {"FALSE", "TRUE"};
#define GEN_FALSE   0
#define GEN_TRUE    1
#define GEN_OOR     2

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
#define HEADER    0
#define HELP      1
#define IDN       2
#define OOR       8
#define TRIG      9
#define SPACE     12
#define COMMA     16

const char text0[] PROGMEM = \
"************ HP4261A USB ADAPTER ************\n\r\
*            Firmware ver: 1.0.0            *\n\r\
*  Author\: Yuri Yudin (yyudin@hotmail.com)  *\n\r\
* https://github.com/yuriyudin/HP4261A_USB  *\n\r\
*      Read HELP for list of commands       *\n\r\
*********************************************\n\r";
const char text1[] PROGMEM =\
"*** Commands set ***\n\r\
*IDN? | returns instrument identification\n\r\
*TRG  | Triggers a measurement in remote control single cycle operation\n\r\
SET   | Sets instrument controls:\n\r\
    FUNC <L/C/R>        | Measurement function (inductance, capacitance, resistance)\n\r\
    MODE <PAR/SER/AUTO> | Circut mode (parallel, series, auto)\n\r\
    FREQ <120/1000>     | Measurement frequency (120 Hz, 1000 Hz)\n\r\
    RNG <1 ... 9>       | Measurement range (9 = Auto)\n\r\
    LEV <50/1000>       | Test level (50 mV, 1 V)\n\r\
    REM <ON/OFF>        | Instrument control (on = remote, off = local)\n\r\
    MEAS <CONT/SGL>     | Measurement cycle (continuous, single)\n\r\
                          (CONT also prints LCR data in local control operation)\n\r\
SET?  | Returns instrument controls";
const char text2[] PROGMEM = "HEWLETT PACKARD 4261A LCR METER";
const char text3[] PROGMEM = "FUNCTION/MODE: ";
const char text4[] PROGMEM = "MEAS. FREQUENCY: ";
const char text5[] PROGMEM = "MEAS. RANGE: ";
const char text6[] PROGMEM = "LCR ";
const char text7[] PROGMEM = "D ";
const char text8[] PROGMEM = "OUT OF RANGE";
const char text9[] PROGMEM = "*TRG";
const char text10[] PROGMEM = "ERROR";
const char text11[] PROGMEM = "SET ";
const char text12[] PROGMEM = " ";
const char text13[] PROGMEM = "SET? ";
const char text14[] PROGMEM = "\(";
const char text15[] PROGMEM = "\)";
const char text16[] PROGMEM = ",";
const char * const TEXTS[] PROGMEM = {text0, text1, text2, text3, text4, text5, text6, text7, text8, text9, text10, text11, text12, text13, text14, text15, text16};

