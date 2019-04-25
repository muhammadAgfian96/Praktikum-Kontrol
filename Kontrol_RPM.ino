#include <Keypad.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "logo.h"
//Punya TFT
#define TFT_CS 53
#define TFT_RST 39
#define TFT_DC 49
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Punya motor
#define dir 2
#define pwm 3
long int start_time = 0;


//Punya keypad
//const byte ROWS = 4;
//const byte COLS = 3;
//char keys[ROWS][COLS] = {
//  {'1','2','3'},
//  {'4','5','6'},
//  {'7','8','9'},
//  {'*','0','#'}
//};
//byte rowPins[ROWS] = { 10, 9, 8, 7 }; //Belum Rapih
//byte colPins[COLS] = { 6, 5, 4}; //Belum Rapih
//Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//float num = 0;


const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4};

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '1'},
  {'4', '5', '6', '2'},
  {'7', '8', '9', '3'},
  {'*', '0', '#', 'D'}
};

Keypad kpd = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
float num = 0;


//Punya push button
#define up 28
#define down 32
#define ok 30
#define back 34

//Punya PING
#define detect 36
float jarak;

//Punya parsing
#define BUF_LEN 128
#define TERM_CHAR '\n'
String readString = "";
int commaIndex = 0;
int secondCommaIndex = 0;
int thirdCommaIndex = 0;
int fourthCommaIndex = 0;
int fifthCommaIndex = 0;
int sixthCommaIndex = 0;
String firstValue = "";
String secondValue = "";
String thirdValue = "";
String fourthValue = "";
String fifthValue = "";
String sixthValue = "";
String Con = "";
float string1 = 0;
float string2 = 0;
float string3 = 0;
float string4 = 0;
String test;
char buff[33];

//Punya Kontrol
float err, PID, lasterr, errSum, lasttime, a[4], e;
int Sp, sp = 0;
int Kp, kp = 10;
int Ki, ki = 20;
int Kd, kd = 30;
int gain, GAIN = 60;


//Punya Interface
int menu = 0;
int i = 0;
int j = 0;
int Mode = 0, mode = 40;
int Kmode = 0, kmode = 50;

//punya motor
#define dir_1 11
#define dir_2 12
#define pwm_modul 13

void arah_on() {
  digitalWrite(dir_1, HIGH);
  digitalWrite(dir_2, LOW);
}

// punya RPM
int z = 0;
float value = 0;
volatile byte half_revolutions;
//volatile unsigned long rev=0;
volatile byte rev = 0;
unsigned int rpmcount, rpm;
unsigned long timeold, lastmillis;
int time;
int high = 0;
int pulsesperturn = 1;

void sensor_rpm() {
  high = 0;
  //Serial.println(high);
  if (half_revolutions >= 1) {
    detachInterrupt(digitalPinToInterrupt(21)); //Disable interrupt when calculating
    rpm = 30 * 1000 / (millis() - timeold) * half_revolutions;
    //     rpm = rpm*2;
    half_revolutions = 0;
    timeold = millis();
    attachInterrupt(digitalPinToInterrupt(21), isr, RISING); //enable interrupt
  }
  //  Serial.println(rpm);
  return rpm;
}

void isr() //interrupt service routine
{
  half_revolutions++;
}

void move_cursor(int y, int y1)
{
  tft.fillTriangle(0, 38 + (y * 17), 0, 48 + (y * 17), 10, 43 + (y * 17), ST7735_BLACK);
  tft.fillTriangle(0, 38 + (y1 * 17), 0, 48 + (y1 * 17), 10, 43 + (y1 * 17), ST7735_RED);
}

void  GetNumber() {
  char key = kpd.getKey();
  switch (key) {
    case NO_KEY:
      break;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

      num = num * 10 + (key - '0');
      break;

    case 'C':
      num = 0;
      break;

  }

  key = kpd.getKey();

}

void disp0()
{
  i = 0;
  motoroff();
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("KONTROL");
  tft.setCursor(20, 17);
  tft.println("RPM");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.setCursor(20, 40);
  tft.println("1.START");
  tft.setCursor(20, 57);
  tft.println("2.BT MODE");
  tft.setCursor(20, 74);
  tft.println("3.SET PARAMETER");
  tft.setCursor(20, 91);
  tft.println("4.MODE KONTROL");
  tft.setCursor(20, 108);
  tft.println("5.TRIAL");
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}

void disp2()
{
  i = 0;
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("BT");
  tft.setCursor(20, 17);
  tft.println("MODE");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.setCursor(20, 40);
  tft.println("1.OFF");
  tft.setCursor(20, 57);
  tft.println("2.ON");
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}


void disp3()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("SET");
  tft.setCursor(20, 17);
  tft.println("PARAMETER");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}

void disp4()
{
  i = 0;
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("MODE");
  tft.setCursor(20, 17);
  tft.println("KONTROL");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.setCursor(20, 40);
  tft.println("1.ON-OFF");
  tft.setCursor(20, 57);
  tft.println("2.PID");
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}

void disp5()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("TRIAL");
  tft.setCursor(20, 17);
  tft.println("PROGRAM");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  //  digitalWrite(dir,LOW);analogWrite(pwm,70);
}

void disp1()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("START");
  tft.setCursor(20, 17);
  tft.println("BT MODE");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  //  digitalWrite(dir,LOW);analogWrite(pwm,70);
}


void disp6()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("START");
  tft.setCursor(20, 17);
  tft.println("ON-OFF");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  //  digitalWrite(dir,LOW);analogWrite(pwm,70);
}


void disp7()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("START");
  tft.setCursor(20, 17);
  tft.println("PID");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  //  digitalWrite(dir,LOW);analogWrite(pwm,70);
}


void load_logo()
{
  int h = 100, w = 100, row, col, buffidx = 0;
  for (row = 20; row < 20 + h; row++) {
    for (col = 20; col < 20 + w; col++) {
      tft.drawPixel(col, row, pgm_read_word(logo + buffidx));
      buffidx++;
    }
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x = tft.width() - 1; x > 6; x -= 6) {
    tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color1);
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color2);
  }
}


void hitungPID(float input, float Sp, float Kp, float Ki, float Kd) {
  float timeChange = (millis() - lasttime) / 1000;
  err = Sp - input;
  errSum = (err * timeChange);
  float dErr = (err - lasterr) / timeChange;
  PID = ((Kp * err) + (Ki * errSum) + (Kd * dErr));
  if (PID > 255) {
    PID = 255;
  }
  if (PID < -255) {
    PID = -255;
  }
  if (PID < 0) {
    PID = PID * (-1);
  }
  arah_on();
  analogWrite(pwm_modul, PID);
  //  analogWrite(pwm,PID+input);
  lasterr = err;
  lasttime = millis();
}

void onoff(float input, float Sp) {
  e = input - Sp;
  if (input < Sp) {
    analogWrite(pwm_modul, input);
  }
  else {
    analogWrite(pwm_modul, 0);
  }
}

void baca_eep() {
  Sp = EEPROM.read(sp);
  Kp = EEPROM.read(kp);
  Ki = EEPROM.read(ki);
  Kd = EEPROM.read(kd);
}

void motoroff() {
  digitalWrite(dir, LOW);
  analogWrite(pwm, 0);
}

void setup(void) {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(back, INPUT_PULLUP);
  pinMode(dir, OUTPUT);
  pinMode(pwm, OUTPUT);
  // punya RPM
  attachInterrupt(digitalPinToInterrupt(21), isr, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
  // punya display
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_WHITE);
  tft.setRotation(3);
  load_logo();
  delay(1000);
  disp0();
}



void loop() {
  switch (menu)
  {
    //menu utama
    case 0:
      arah_on(); analogWrite(pwm_modul, 0);
      if (digitalRead(down) == LOW && i < 4) {
        delay(200);
        i++;
        move_cursor(i - 1, i);
      }
      if (digitalRead(up) == LOW && i > 0) {
        delay(200);
        i--;
        move_cursor(i + 1, i);
      }
      if (digitalRead(ok) == LOW) {
        if (i == 0)
        {
          if (EEPROM.read(mode) == 1)  {
            delay(200);
            tft.fillScreen(ST7735_BLACK);
            menu = 1;
            disp1();
          }
          else if (EEPROM.read(mode) == 0) {
            if (EEPROM.read(kmode) == 0) {
              delay(200);
              tft.fillScreen(ST7735_BLACK);
              menu = 6;
              disp6();
            }
            else if (EEPROM.read(kmode) == 1) {
              delay(200); tft.fillScreen(ST7735_BLACK); menu = 7;
              start_time = millis();
              Serial.println("\n\n==MULAI AMBIL DATA==\n\nwaktu(ms)\tset point \trpm");
              disp7();
            }
          }
        }
        else if (i == 1) {
          delay(200);
          tft.fillScreen(ST7735_BLACK);
          menu = 2;
          disp2();
        }
        else if (i == 2) {
          delay(200);
          tft.fillScreen(ST7735_BLACK);
          menu = 3;
          disp3();
          i = 0;
          baca_eep();
        }
        else if (i == 3) {
          delay(200);
          tft.fillScreen(ST7735_BLACK);
          menu = 4;
          disp4();
        }
        else {
          delay(200);
          tft.fillScreen(ST7735_BLACK);
          menu = 5;
          disp5();
        }
      }

      break;

    //BT Mode
    case 1:
      tft.fillRect(40, 40, tft.width(), tft.height(), ST7735_BLACK);
      sensor_rpm();
      //yg dikirim sp kp ki kd
      while (Serial1.available()) {
        char ch = Serial1.read();
        if (ch != TERM_CHAR)
          test += (char)ch;
        else
          break;
        delay(1);
      }
      if (test.length() > 0)
      {
        //parse comma


        commaIndex = test.indexOf(' ');
        secondCommaIndex = test.indexOf(' ', commaIndex + 1);
        thirdCommaIndex = test.indexOf(' ', secondCommaIndex + 1);
        fourthCommaIndex = test.indexOf(' ', thirdCommaIndex + 1);
        fifthCommaIndex = test.indexOf('\n', fourthCommaIndex + 1);

        firstValue = test.substring(0, commaIndex);
        secondValue = test.substring(commaIndex + 1, secondCommaIndex);
        thirdValue = test.substring(secondCommaIndex + 1, thirdCommaIndex);
        fourthValue = test.substring(thirdCommaIndex + 1, fourthCommaIndex);
        fifthValue = test.substring(fourthCommaIndex + 1, fifthCommaIndex);


        string1 = secondValue.toFloat();
        string2 = thirdValue.toFloat();
        string3 = fourthValue.toFloat();
        string4 = fifthValue.toFloat();

        test = "";
      }

      if (firstValue.length() > 3 )
      {
        Con = firstValue;
      }
      if (string4 < 10) {
        string4 = 10;
      }
      if (string4 > 50) {
        string4 = 50;
      }
      tft.setCursor(0, 40);
      tft.print("1.SP = ");
      tft.println(string4);
      tft.print("2.KP = ");
      tft.println(string1);
      tft.print("3.R  = ");
      tft.println(rpm);
      tft.print("4.E  = ");
      tft.println(err);
      tft.print("5.KI = ");
      tft.println(string2);
      tft.print("6.KD = ");
      tft.println(string3);
      tft.print("7.Con= ");
      tft.println(Con);
      sprintf(buff, "%d", (int)rpm);
      Serial1.println(buff);
      if (Con == "start")
      {
        hitungPID(rpm, string4, string1, string2, string3);

      }
      else {
        motoroff();
      }

      if (digitalRead(back) == LOW || digitalRead(ok) == LOW)
      {
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
      }
      break;

    case 2:
      if (digitalRead(down) == LOW && i < 1) {
        delay(200);
        i++;
        move_cursor(i - 1, i);
      }
      if (digitalRead(up) == LOW && i > 0) {
        delay(200);
        i--;
        move_cursor(i + 1, i);
      }
      if (digitalRead(ok) == LOW)
      {
        if (i == 0) {
          EEPROM.update(mode, 0);
        }
        else if (i == 1) {
          EEPROM.update(mode, 1);
        }
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
      }
      if (digitalRead(back) == LOW)
      {
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
      }
      break;

    case 3:
      GetNumber();
      tft.fillRect(60, 40, tft.width(), tft.height(), ST7735_BLACK);
      tft.setCursor(20, 40);
      tft.print("1.SP = ");
      tft.print(Sp);
      tft.setCursor(20, 57);
      tft.print("2.KP = ");
      tft.print(Kp);
      tft.setCursor(20, 74);
      tft.print("3.KI = ");
      tft.print(Ki);
      tft.setCursor(20, 91);
      tft.print("4.KD = ");
      tft.print(Kd);
      switch (i)
      {
        case 0:
          Sp = num;
          break;
        case 1:
          Kp = num;
          break;
        case 2:
          Ki = num;
          break;
        case 3:
          Kd = num;
          break;
      }

      if (digitalRead(down) == LOW && i < 3) {
        delay(200);
        i++;
        move_cursor(i - 1, i);
        num = 0;
      }
      if (digitalRead(up) == LOW && i > 0) {
        delay(200);
        i--;
        move_cursor(i + 1, i);
        num = 0;
      }
      if (digitalRead(ok) == LOW)
      {
        EEPROM.update(sp, Sp); EEPROM.update(kp, Kp); EEPROM.update(ki, Ki); EEPROM.update(kd, Kd);
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
        num = 0;
      }
      if (digitalRead(back) == LOW)
      {
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
        num = 0;
      }
      break;

    case 4:
      if (digitalRead(down) == LOW && i < 1) {
        delay(200);
        i++;
        move_cursor(i - 1, i);
      }
      if (digitalRead(up) == LOW && i > 0) {
        delay(200);
        i--;
        move_cursor(i + 1, i);
      }
      if (digitalRead(ok) == LOW)
      {
        if (i == 0) {
          EEPROM.write(kmode, 0);
        }
        else if (i == 1) {
          EEPROM.write(kmode, 1);
        }
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
      }
      if (digitalRead(back) == LOW)
      {
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
      }
      break;

    //trial
    case 5:
      sensor_rpm();
      tft.fillRect(50, 40, tft.width(), tft.height(), ST7735_BLACK);
      tft.setCursor(20, 40);
      tft.print("1.R = ");
      tft.print(rpm);
      tft.setCursor(20, 60);
      tft.print("2.count = ");
      tft.print(half_revolutions);
      tft.setCursor(20, 80);
      tft.print("2.pwm = ");
      tft.print(z);

      //delay(55);
      arah_on();
      analogWrite(pwm_modul, z);

      if (digitalRead(down) == LOW) {
        delay(200);
        z = z - 5;
      }
      if (digitalRead(up) == LOW) {
        delay(200);
        z = z + 5;
      }

      if (digitalRead(back) == LOW || digitalRead(ok) == LOW) {
        delay(200); tft.fillScreen(ST7735_BLACK);
        arah_on(); analogWrite(pwm_modul, 0);
        menu = 0; disp0();
      }

      break;

    //On-Off
    case 6:
      sensor_rpm();
      tft.fillRect(60, 40, tft.width(), tft.height(), ST7735_BLACK);
      tft.setCursor(20, 40);
      tft.print("1.V  = ");
      tft.print(rpm);
      tft.setCursor(20, 57);
      tft.print("2.SP = ");
      tft.print(EEPROM.read(sp));
      tft.setCursor(20, 74);
      onoff(jarak, EEPROM.read(sp));
      delay(50);
      if (digitalRead(back) == LOW || digitalRead(ok) == LOW)
      {
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
      }
      break;

    case 7:
      sensor_rpm();
      tft.fillRect(60, 40, tft.width(), tft.height(), ST7735_BLACK);
      tft.setCursor(20, 40);
      tft.print("1.V  = ");
      tft.print(rpm);
      tft.setCursor(20, 57);
      tft.print("2.SP = ");
      tft.print(EEPROM.read(sp));
      tft.setCursor(20, 74);
      tft.print("3.E  = ");
      tft.print(err);
      tft.setCursor(20, 91);
      tft.print("4.PID= ");
      tft.print(PID);
      
      hitungPID(rpm, EEPROM.read(sp), EEPROM.read(kp), EEPROM.read(ki), EEPROM.read(kd));

      Serial.print(millis() - start_time);
      Serial.print("\t");
      Serial.print(EEPROM.read(sp));
      Serial.print("\t");
      Serial.println(rpm);

      if (digitalRead(back) == LOW || digitalRead(ok) == LOW)
      {
        arah_on(); analogWrite(pwm_modul, 0);
        delay(200); tft.fillScreen(ST7735_BLACK); menu = 0; disp0();
      }
      break;

  }
}
