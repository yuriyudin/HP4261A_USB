/*
  File name: HP4261A.ino
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

#include "HP4261A.h"
#include "Wire.h"
#include "TCA9555.h"
#include <CommandParser.h>

TCA9555 U2(U2_Addr);
TCA9555 U3(U3_Addr);

byte wtgtCounter = 0;   // ~WTGT clock counter (0 to 7)
byte DTBArray[8]; // DTB data is 8 items x 4 bits wide
bool dataReady = false;
bool dataSent = false; 
bool measurementComplete = false;
uint16_t remoteControls = 0xFFFF;
byte measRegime = 1;
bool trig = false;

struct controlData {
    byte modeVal;
    byte orVal;
    byte freqVal;
    byte fncVal;
    byte dblankVal;
    uint16_t rngVal;
    byte levVal; 
    byte remVal = 1;
    byte measVal = 1;   
};
controlData controls_rd;
controlData controls_wr;

struct measData {
 int data;
 int polarity;
};
measData LCR;
measData D;

typedef CommandParser<11, 2, 8, 6, 64> MyCommandParser;
MyCommandParser parser;

void setup()                                                      
{
  pinMode(LED_GATE, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  U2.begin();
  U3.begin();

  Serial.println((__FlashStringHelper *)pgm_read_word(&TEXTS[HEADER]));
  attachInterrupt(digitalPinToInterrupt(WTGT_n),captureData,RISING);
  attachInterrupt(digitalPinToInterrupt(FLAG_n),processResults,FALLING);

  U3.pinMode16(0x0);
  U3.write16(remoteControls);

  parser.registerCommand("*IDN?", "", &callbackIdn);
  parser.registerCommand("*TRG", "", &callbackTrig);
  parser.registerCommand("HELP", "", &callbackHelp);
  parser.registerCommand("SET", "ss", &callbackSetControls);
  parser.registerCommand("SET?", "", &callbackEchoControls);
  parser.registerCommand("FUNC?", "", &callbackEchoFunc);
  parser.registerCommand("MODE?", "", &callbackEchoMode);
  parser.registerCommand("FREQ?", "", &callbackEchoFreq);
  parser.registerCommand("RNG?", "", &callbackEchoRange);
  parser.registerCommand("LEV?", "", &callbackEchoLev);
  parser.registerCommand("REM?", "", &callbackEchoRem);
  parser.registerCommand("MEAS?", "", &callbackEchoMeas);

  // Read initial instrument controls
  controls_wr = parseControls(U2.read16());
  writeRemoteControls();
}


void loop()  {  

  // Read and process serial commands
  if (Serial.available()) {
    char line[16];
    size_t lineLength = Serial.readBytesUntil('\n', line, 15);
    line[lineLength] = '\0';
    for (int i = 0; i < lineLength; i++ ) // convert characters to uppercase
    {
      if (((line[i] & 0xDF) >= 'A') && ((line[i] & 0xDF) <= 'Z'))  {
        line[i] &= 0xDF;
      }
    }

    char response[MyCommandParser::MAX_RESPONSE_SIZE];
    parser.processCommand(line, response);
    Serial.println(response);
  }

  if ( (dataReady) && !( (controls_wr.measVal == MEAS_SGL) && (controls_wr.remVal == REM_OFF) ) ) {

      // Read instrument controls
      controls_rd = parseControls(U2.read16());    
        
      //Extract LCR data starting at DTBArray index 0 
      LCR = extractData(0);
      //Extract D data starting at DTBArray index 4 
      D = extractData(4);

      printResults();

      digitalWrite(LED_GATE, LOW);
      dataReady = false; 
      dataSent = true; 
  }

    // Trigger a new measurement
    if ( ((controls_wr.measVal == MEAS_CONT) || ((controls_wr.measVal == MEAS_SGL) && (trig))) && (dataSent) && (measurementComplete) ) {  

      U3.write16(remoteControls & ~(1 << EXE_n));
      delay (1);
      U3.write16(remoteControls | (1 << EXE_n));
      trig = false;
      measurementComplete = false;
    }

}


void captureData()
// This ISR function is called on ~WTGT falling edge. It clocks in DTB data from the HP4261A.
// There are 8 ~WTGT pulses per each measurement, so this ISR runs 8 times at 250 us intervals.
// on each ~WTGT pulse we need to capture 4 bits of data (DTB1, DTB2, DTB4, DTB8).
// At the completion of 8 cycles the DTBArray is fully populated with LCR and D measured data.
{  
   measurementComplete = false;
   //dataReady = false;
   dataSent = false;
   digitalWrite(LED_GATE, HIGH);
   
   if (wtgtCounter <= 7) {
       int DTB = 0;
       int val = 0;
       for (int bit = 0; bit <= 3; bit++) { // loop to read 4 bits
           val = digitalRead(DTB1 + bit);
           DTB = DTB | (val << bit);
       }
       DTBArray[wtgtCounter] = DTB;
       wtgtCounter++;
   }

}


void processResults()
// This ISR function executes on ~FLAG rising edge (measurement done) 
// The instrument sends the ~FLAG pulse after the completion of the 8 ~WTGT cycles 
// It is to signalize to the main loop that it can now read the instrument controls,
// parse the data and send to serial                
{                    
   wtgtCounter = 0; // reset ~WTGT clock counter

   measurementComplete = true;  
   dataReady = true;
}


void printResults()
{
   // Print LCR Data
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[6]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   if (LCR.polarity == -1) {
       Serial.print("-");
   }
   Serial.print(LCR.data);

   // Print D Data
   if (controls_rd.dblankVal == 1) {
       Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[COMMA]));
       Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
       Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[7]));
       Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
       if (D.polarity == -1) {
           Serial.print("-");
           }
        Serial.print(D.data);
   }

   //Print Function
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[COMMA]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&ARG_NAMES[ARG_FUNC]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&FUNC_NAMES[controls_rd.fncVal]));

   //Print Mode
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[COMMA]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&ARG_NAMES[ARG_MODE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&MODE_NAMES[controls_rd.modeVal]));
   
   //Print Measurement Frequency
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[COMMA]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&ARG_NAMES[ARG_FREQ]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&FREQ_NAMES[controls_rd.freqVal]));
   
   //Print LCR Range
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[COMMA]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&ARG_NAMES[ARG_RNG]));
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
   Serial.print(controls_wr.rngVal + 1);
   Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[14]));

   switch (controls_rd.fncVal) {
      case 0: // Capacitance
      if (controls_rd.freqVal == 1) { // shift range up at 120 Hz
        Serial.print((__FlashStringHelper *)pgm_read_word(&C_RANGE_NAMES[controls_rd.rngVal] + 1));
      }
      else Serial.print((__FlashStringHelper *)pgm_read_word(&C_RANGE_NAMES[controls_rd.rngVal]));
      break;
      case 1: // Resistance
      Serial.print((__FlashStringHelper *)pgm_read_word(&R_RANGE_NAMES[controls_rd.rngVal]));
      break;
      case 3: // Inductance
      if (controls_rd.freqVal == 1) { // shift range up at 120 Hz
        Serial.print((__FlashStringHelper *)pgm_read_word(&L_RANGE_NAMES[controls_rd.rngVal + 1]));
      }
      else Serial.print((__FlashStringHelper *)pgm_read_word(&L_RANGE_NAMES[controls_rd.rngVal]));
      break;
      default:
      Serial.print((__FlashStringHelper *)pgm_read_word(&C_RANGE_NAMES[0]));
      break;
       }
      Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[15]));

   // Print Out of Range annunciation
   if (controls_rd.orVal == 1) {
      Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[COMMA]));
      Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[SPACE]));
      Serial.print((__FlashStringHelper *)pgm_read_word(&TEXTS[OOR]));
   } 

   Serial.println((__FlashStringHelper *)pgm_read_word(&TEXTS[12]));
   
}

controlData parseControls(int value)
// Function to parse control bits read from the instrument to values
{
   // Extract instrument controls
   controlData result;
   result.modeVal = bitRead(value, CMS);
   result.orVal = bitRead(value, OR);
   result.freqVal = bitRead(value, FREQ1K_n);
   result.fncVal = bitRead(value, FNCA) | (bitRead(value, FNCB) << 1);
   result.dblankVal = bitRead(value, DBLANK_n);
   result.rngVal = (bitRead(value, RNGC) | (bitRead(value, RNGB) << 1) | (bitRead(value, RNGA) << 2)) + 1; // "+1" because the range table is 1-based    
   return result;
}

measData extractData(int startIndex)
// Function to extract LCR and D data
{
   measData result;
   result.data = 0;
   int mult = 1;
   byte temp;
   for (int j = startIndex; j <= startIndex + 2; j++) { // DTBArray index loop
       temp = 0; 
       for (int bit = 0; bit <= 3; bit++) { // bit index loop
           temp = temp | (int(bitRead(DTBArray[j], bit)) << bit);
       }
       result.data = result.data + temp * mult;
       mult = mult*10;
   }
   result.data = result.data + !bitRead(DTBArray[startIndex + 3], 0)*1000; // 10^3 digit
   if (bitRead(DTBArray[startIndex + 3], 1)) {
       result.polarity = -1;
   }
   else result.polarity = 1;
   return result;
}

void callbackIdn(MyCommandParser::Argument *args, char *response) {
  strlcpy_P(response, (TEXTS[IDN]), MyCommandParser::MAX_RESPONSE_SIZE);
}

void callbackTrig(MyCommandParser::Argument *args, char *response) {
  trig = true;
  if ((controls_wr.remVal == REM_ON) && (controls_wr.measVal == MEAS_SGL)) {
  strlcpy_P(response, (TEXTS[TRIG]), MyCommandParser::MAX_RESPONSE_SIZE);
  }
}

void callbackHelp(MyCommandParser::Argument *args, char *response) {
  Serial.println((__FlashStringHelper *)pgm_read_word(&TEXTS[HELP]));
}

void callbackEchoFunc(MyCommandParser::Argument *args, char *response) {
  strcat_P(response, (PGM_P)pgm_read_word(&(ARG_NAMES[ARG_FUNC])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[SPACE])));
  strcat_P(response, (PGM_P)pgm_read_word(&(FUNC_NAMES[controls_wr.fncVal])));
}

void callbackEchoMode(MyCommandParser::Argument *args, char *response) {
  //strcpy(response, inputTxt);
  strcat_P(response, (PGM_P)pgm_read_word(&(ARG_NAMES[ARG_MODE])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[SPACE])));
  strcat_P(response, (PGM_P)pgm_read_word(&(MODE_NAMES[controls_wr.modeVal])));
}

void callbackEchoFreq(MyCommandParser::Argument *args, char *response) {
  strcat_P(response, (PGM_P)pgm_read_word(&(ARG_NAMES[ARG_FREQ])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[SPACE])));
  strcat_P(response, (PGM_P)pgm_read_word(&(FREQ_NAMES[controls_wr.freqVal])));
}

void callbackEchoRange(MyCommandParser::Argument *args, char *response) {
  char temp[2];
  strcat_P(response, (PGM_P)pgm_read_word(&(ARG_NAMES[ARG_RNG])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[SPACE])));
  dtostrf((controls_wr.rngVal + 1), 1, 0, temp);
  strcat(response, temp);
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[14])));
  if (controls_wr.fncVal == FUNC_C) {
      if (controls_wr.freqVal == FREQ_1000) {
        if (controls_wr.rngVal == 8) strcat_P(response, (PGM_P)pgm_read_word(&(C_RANGE_NAMES[controls_wr.rngVal + 2])));
        else strcat_P(response, (PGM_P)pgm_read_word(&(C_RANGE_NAMES[controls_wr.rngVal + 1])));}
      else {strcat_P(response, (PGM_P)pgm_read_word(&(C_RANGE_NAMES[controls_wr.rngVal + 2])));} // shift range up for C at 120 Hz
  }
  else if (controls_wr.fncVal == FUNC_L) {
      if (controls_wr.freqVal == FREQ_1000) { // 1000 Hz
        if (controls_wr.rngVal == 7) strcat_P(response, (PGM_P)pgm_read_word(&(L_RANGE_NAMES[controls_wr.rngVal]))); 
        else strcat_P(response, (PGM_P)pgm_read_word(&(L_RANGE_NAMES[controls_wr.rngVal + 1])));
        }
          else { // 120 Hz
              if (controls_wr.rngVal >= 7) strcat_P(response, (PGM_P)pgm_read_word(&(L_RANGE_NAMES[controls_wr.rngVal + 1])));
              else strcat_P(response, (PGM_P)pgm_read_word(&(L_RANGE_NAMES[controls_wr.rngVal + 2])));} 
  }      

  else strcat_P(response, (PGM_P)pgm_read_word(&(R_RANGE_NAMES[controls_wr.rngVal + 1])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[15])));
}

void callbackEchoLev(MyCommandParser::Argument *args, char *response) {
  strcat_P(response, (PGM_P)pgm_read_word(&(ARG_NAMES[ARG_LEV])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[SPACE])));
  strcat_P(response, (PGM_P)pgm_read_word(&(LEV_NAMES[controls_wr.levVal])));
}

void callbackEchoRem(MyCommandParser::Argument *args, char *response) {
  strcat_P(response, (PGM_P)pgm_read_word(&(ARG_NAMES[ARG_REM])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[SPACE])));
  strcat_P(response, (PGM_P)pgm_read_word(&(REM_NAMES[controls_wr.remVal])));
}

void callbackEchoMeas(MyCommandParser::Argument *args, char *response) {
  strcat_P(response, (PGM_P)pgm_read_word(&(ARG_NAMES[ARG_MEAS])));
  strcat_P(response, (PGM_P)pgm_read_word(&(TEXTS[SPACE])));
  strcat_P(response, (PGM_P)pgm_read_word(&(MEAS_NAMES[controls_wr.measVal])));
}


void callbackEchoControls(MyCommandParser::Argument *args, char *response) {
  memcpy(response, TEXTS[13], strlen_P(TEXTS[13]) + 1);
  callbackEchoFunc(args, response);
  strcat(response, ", ");
  callbackEchoMode(args, response);
  strcat(response, ", ");
  callbackEchoFreq(args, response);
  strcat(response, ", ");
  callbackEchoRange(args, response);
  strcat(response, ", ");
  callbackEchoLev(args, response);
  strcat(response, ", ");
  callbackEchoRem(args, response);
  strcat(response, ", ");
  callbackEchoMeas(args, response);
}

void callbackSetControls(MyCommandParser::Argument *args, char *response) {
 
  // Set Mode
  if (strcmp_P(args[0].asString, ARG_NAMES[ARG_MODE]) == 0) {    
      if (strcmp_P(args[1].asString, MODE_NAMES[MODE_PAR]) == 0) {
          controls_wr.modeVal = MODE_PAR;
      }
      else if (strcmp_P(args[1].asString, MODE_NAMES[MODE_SER]) == 0) {
          controls_wr.modeVal = MODE_SER;
      }
      else if (strcmp_P(args[1].asString, MODE_NAMES[MODE_AUTO]) == 0) {
          controls_wr.modeVal = MODE_AUTO;
      }
      else {
          // wrong arg value, do nothing
           }
      memcpy(response, TEXTS[11], strlen_P(TEXTS[11]) + 1);
      callbackEchoMode(args, response);
  }
  
  // Set Frequency
  else if (strcmp_P(args[0].asString, ARG_NAMES[ARG_FREQ]) == 0) {
      if (strcmp_P(args[1].asString, FREQ_NAMES[FREQ_1000]) == 0) controls_wr.freqVal = FREQ_1000;
      else if (strcmp_P(args[1].asString, FREQ_NAMES[FREQ_120]) == 0) controls_wr.freqVal = FREQ_120;
      else {
          // wrong arg value, do nothing
           }
      memcpy(response, TEXTS[11], strlen_P(TEXTS[11]) + 1);
      callbackEchoFreq(args, response);
  }  


  // Set Function
  else if (strcmp_P(args[0].asString, ARG_NAMES[ARG_FUNC]) == 0) {
      if (strcmp_P(args[1].asString, FUNC_NAMES[FUNC_C]) == 0) controls_wr.fncVal = FUNC_C;
      else if(strcmp_P(args[1].asString, FUNC_NAMES[FUNC_R]) == 0) controls_wr.fncVal = FUNC_R;
      else if(strcmp_P(args[1].asString, FUNC_NAMES[FUNC_L]) == 0) controls_wr.fncVal = FUNC_L;
      else {
          // wrong arg value, do nothing
           }
      memcpy(response, TEXTS[11], strlen_P(TEXTS[11]) + 1);
      callbackEchoFunc(args, response);
  }   

  // Set Range
  else if (strcmp_P(args[0].asString, ARG_NAMES[ARG_RNG]) == 0) {
      if ((atoi(args[1].asString) >= 1) && (atoi(args[1].asString) <= 9)) {        
         controls_wr.rngVal = atoi(args[1].asString) - 1; // "- 1" is used to convert to zero-based range used internally                    
         }
      else {
          // wrong arg value, do nothing
           }
      memcpy(response, TEXTS[11], strlen_P(TEXTS[11]) + 1);
      callbackEchoRange(args, response);    
  }


  // Set Level
  else if (strcmp_P(args[0].asString, ARG_NAMES[ARG_LEV]) == 0) {
      if (strcmp_P(args[1].asString, LEV_NAMES[LEV_1000]) == 0) controls_wr.levVal = LEV_1000;
      else if (strcmp_P(args[1].asString, LEV_NAMES[LEV_50]) == 0) controls_wr.levVal = LEV_50;
      else {
          // wrong arg value, do nothing
           }
      memcpy(response, TEXTS[11], strlen_P(TEXTS[11]) + 1);
      callbackEchoLev(args, response);   
  }

  // Set Remote Control
  else if (strcmp_P(args[0].asString, ARG_NAMES[ARG_REM]) == 0) {
      if (strcmp_P(args[1].asString, REM_NAMES[REM_OFF]) == 0) controls_wr.remVal = REM_OFF;
      else if (strcmp_P(args[1].asString, REM_NAMES[REM_ON]) == 0) controls_wr.remVal = REM_ON;
      else {
          // wrong arg value, do nothing
           }
      memcpy(response, TEXTS[11], strlen_P(TEXTS[11]) + 1);
      callbackEchoRem(args, response);   

  }

  // Set Measurement Type
  if (strcmp_P(args[0].asString, ARG_NAMES[ARG_MEAS]) == 0) {    
      if (strcmp_P(args[1].asString, MEAS_NAMES[MEAS_CONT]) == 0) {
          controls_wr.measVal = MEAS_CONT;
      }
      else if (strcmp_P(args[1].asString, MEAS_NAMES[MEAS_SGL]) == 0) {
          controls_wr.measVal = MEAS_SGL;
      }
      else {
          // wrong arg value, do nothing
           }
      memcpy(response, TEXTS[11], strlen_P(TEXTS[11]) + 1);
      callbackEchoMeas(args, response);  
  }
writeRemoteControls ();

}

void writeRemoteControls () {
remoteControls = (remoteControls & (~(3 << CMA_R))) | (controls_wr.modeVal << CMA_R);
remoteControls = (remoteControls & ~((1 << FREQ1K_R))) | (controls_wr.freqVal << FREQ1K_R);
remoteControls = (remoteControls & ~((3 << FNCA_R))) | (controls_wr.fncVal << FNCA_R);
remoteControls = (remoteControls & ~((3 << DRNA_R))) | ((controls_wr.rngVal & 3) << DRNA_R);
remoteControls = (remoteControls & ~((3 << DRNC_R))) | ((controls_wr.rngVal & 12) << DRNC_R - 2); 
remoteControls = (remoteControls & ~((1 << TL_R_n))) | (controls_wr.levVal << TL_R_n);
remoteControls = (remoteControls & ~((1 << REM_n))) | (controls_wr.remVal << REM_n);
U3.write16(remoteControls);
}

