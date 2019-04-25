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
#define dir_kiri 12
#define dir_kanan 11
#define pwm 13
String yeah ="heheh";
 
//Punya keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = { 10, 9, 8, 7 }; //Belum Rapih
byte colPins[COLS] = { 6, 5, 4}; //Belum Rapih
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
float num = 0;

//Punya push button
#define up 28
#define down 32
#define ok 30
#define back 34

//Punya PING
#define Ping 36
float jarak;

//Punya parsing
#define BUF_LEN 128
#define TERM_CHAR '\n'
String readString="";
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
float string1=0;
float string2=0;
float string3=0;
float string4=0;
String test;
char buff[33];

//Punya Kontrol
float err, PID, lasterr, errSum,lasttime,a[4],e;
int Sp, sp=0;
int Kp, kp=10;
int Ki, ki=20;
int Kd, kd=30;
int gain, GAIN=60;
long int start_time=0;

//Punya Interface
int menu = 0;
int i = 0;
int j = 0;
int Mode = 0, mode = 40;
int Kmode = 0, kmode = 50;

void move_cursor(int y,int y1){
  tft.fillTriangle(0, 38+(y*17), 0, 48+(y*17), 10, 43+(y*17), ST7735_BLACK);
  tft.fillTriangle(0, 38+(y1*17), 0, 48+(y1*17), 10, 43+(y1*17), ST7735_RED);  
}

void  GetNumber(){
   char key = kpd.getKey();
      switch (key){
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
//  sensor_ping();
  i=0;
  motoroff();
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("KONTROL");
  tft.setCursor(20,17);
  tft.println("POSISI");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.setCursor(20,40);
  tft.print("1.START = ");  
//  tft.println(jarak);
  tft.setCursor(20,57);
  tft.println("2.BT MODE");
  tft.setCursor(20,74);
  tft.println("3.SET PARAMETER");
  tft.setCursor(20,91);
  tft.println("4.MODE KONTROL");
  tft.setCursor(20,108);
  tft.println("5.TRIAL");
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}

void disp2()
{
  i=0;
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("BT");
  tft.setCursor(20,17);
  tft.println("MODE");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.setCursor(20,40);
  tft.println("1.OFF");
  tft.setCursor(20,57);
  tft.println("2.ON");
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}


void disp3()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("SET");
  tft.setCursor(20,17);
  tft.println("PARAMETER");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}

void disp4()
{
  i=0;
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("MODE");
  tft.setCursor(20,17);
  tft.println("KONTROL");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.setCursor(20,40);
  tft.println("1.ON-OFF");
  tft.setCursor(20,57);
  tft.println("2.PID");
  tft.fillTriangle(0, 38, 0, 48, 10, 43, ST7735_RED);
}

void disp5()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("TRIAL");
  tft.setCursor(20,17);
  tft.println("PROGRAM");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
kiri();analogWrite(pwm,70);
}

void disp1()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("START");
  tft.setCursor(20,17);
  tft.println("BT MODE");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  kiri();analogWrite(pwm,70);
}


void disp6()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("START");
  tft.setCursor(20,17);
  tft.println("ON-OFF");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  kiri();analogWrite(pwm,70);
}


void disp7()
{
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(20,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("START");
  tft.setCursor(20,17);
  tft.println("PID");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  kiri();analogWrite(pwm,70);
}

void sensor_ping()
{
  long duration;
  pinMode(Ping, OUTPUT);
  digitalWrite(Ping, LOW);
  delayMicroseconds(2);
  digitalWrite(Ping, HIGH);
  delayMicroseconds(5);
  digitalWrite(Ping, LOW);

  pinMode(Ping, INPUT);
  duration = pulseIn(Ping, HIGH);

  jarak = duration / 29 / 2;
}

void load_logo()
{
  int h = 100,w = 100, row, col, buffidx=0;
  for (row=20; row<20+h; row++) {
    for (col=20; col<20+w; col++) {
      tft.drawPixel(col, row, pgm_read_word(logo + buffidx));
      buffidx++;
    } 
  }
}




void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}


void hitungPID(float input,float Sp,float Kp, float Ki, float Kd)
{
  float timeChange = (millis()-lasttime)/1000;
  err = Sp - input;
  errSum = (err * timeChange);
  float dErr = (err - lasterr) / timeChange;
  
  PID = ((Kp * err) + (Ki * errSum) + (Kd * dErr));
  
  if(PID>255) {PID=255;}
  if(PID<-255) {PID=-255;}
  if(PID<0) {PID=PID*(-1);}
  
  if(err<0) {kiri();analogWrite(pwm,PID);}
  else {kanan();analogWrite(pwm,PID); }
  lasterr = err;
  lasttime=millis();
}

void onoff(float input,float Sp)
{
  e=input-Sp;
 if(input<Sp) {kanan();analogWrite(pwm,EEPROM.read(GAIN)); }
 else {kiri();analogWrite(pwm,255-EEPROM.read(GAIN)); }
}

void baca_eep()
{
  Sp = EEPROM.read(sp);
  Kp = EEPROM.read(kp);
  Ki = EEPROM.read(ki);
  Kd = EEPROM.read(kd);
  gain = EEPROM.read(GAIN);
}

void motoroff()
{
kanan();
  analogWrite(pwm,0);
}

void setup(void) {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(up,INPUT_PULLUP);
  pinMode(down,INPUT_PULLUP);
  pinMode(ok,INPUT_PULLUP);
  pinMode(back,INPUT_PULLUP);
  pinMode(dir_kiri,OUTPUT);
  pinMode(dir_kanan,OUTPUT);
  pinMode(pwm,OUTPUT);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_WHITE);
  tft.setRotation(3);
  load_logo();
  delay(1000);
  disp0();
}


void loop() {
 switch (menu){
  //menu utama
  case 0:
//  sensor_ping();
  if (digitalRead(down)==LOW && i < 4) {delay(200); i++; move_cursor(i-1,i);}
  if (digitalRead(up)==LOW && i > 0) {delay(200); i--; move_cursor(i+1,i);}
  if (digitalRead(ok)== LOW)
  {
    if (i == 0) 
    {
      if (EEPROM.read(mode)==1)  {delay(200); tft.fillScreen(ST7735_BLACK);menu=1;disp1();}
      else if(EEPROM.read(mode)==0){
        if (EEPROM.read(kmode)==0) {delay(200); tft.fillScreen(ST7735_BLACK);menu=6;disp6();}
        else if(EEPROM.read(kmode)==1) {delay(200); tft.fillScreen(ST7735_BLACK);menu=7;
        start_time=millis(); 
        Serial.println("\n\n==MULAI AMBIL DATA==\n\nwaktu(ms)\tset point \tposisi"); 
        disp7();}
      }
    }
    else if (i == 1) {delay(200); tft.fillScreen(ST7735_BLACK);menu=2;disp2();}
    else if (i == 2) {delay(200); tft.fillScreen(ST7735_BLACK);menu=3;disp3();i=0;baca_eep();}
    else if (i == 3) {delay(200); tft.fillScreen(ST7735_BLACK);menu=4;disp4();}
    else {delay(200); tft.fillScreen(ST7735_BLACK);menu=5;disp5();}
  }
  
  break;

  //BT Mode
  case 1:
  tft.fillRect(40,40,tft.width(),tft.height(),ST7735_BLACK);
  sensor_ping();
  //yg dikirim sp kp ki kd
     while (Serial1.available())
  {
    char ch = Serial1.read();
    if(ch != TERM_CHAR)
      test += (char)ch;
    else
    break;
    delay(1);
  }
     if(test.length()>0)
      {
        //parse comma
      
     
        commaIndex = test.indexOf(' ');
      secondCommaIndex = test.indexOf(' ', commaIndex+1);
      thirdCommaIndex = test.indexOf(' ', secondCommaIndex+1);
      fourthCommaIndex = test.indexOf(' ', thirdCommaIndex+1);
      fifthCommaIndex = test.indexOf('\n', fourthCommaIndex+1);
    
      firstValue = test.substring(0, commaIndex);
      secondValue = test.substring(commaIndex+1, secondCommaIndex);
      thirdValue = test.substring(secondCommaIndex+1,thirdCommaIndex);
      fourthValue = test.substring(thirdCommaIndex+1,fourthCommaIndex);
      fifthValue = test.substring(fourthCommaIndex+1,fifthCommaIndex);

     
      string1 = secondValue.toFloat();
      string2 = thirdValue.toFloat();
      string3 = fourthValue.toFloat();
      string4 = fifthValue.toFloat();
      
      
    test="";
      }

      if(firstValue.length() > 3 )
      {
        Con = firstValue;
      }
      if(string4<10) {string4=10;}
      if(string4>50) {string4=50;}
      tft.setCursor(0,40);
      tft.print("1.SP = ");
      tft.println(string4);
      tft.print("2.KP = ");
      tft.println(string1);
      tft.print("3.R  = ");
      tft.println(jarak);
      tft.print("4.E  = ");
      tft.println(err);
      tft.print("5.KI = ");
      tft.println(string2);
      tft.print("6.KD = ");
      tft.println(string3);
      tft.print("7.Con= ");
      tft.println(Con);
     sprintf(buff,"%d",(int)jarak);
     Serial1.println(buff);
     if(Con == "start"){
      hitungPID(jarak,string4,string1,string2,string3);
 
    }
    else {motoroff();}

  if (digitalRead(back)== LOW || digitalRead(ok)== LOW)
  {
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
  }
  break;

  case 2:
  if (digitalRead(down)==LOW && i < 1) {delay(200); i++; move_cursor(i-1,i);}
  if (digitalRead(up)==LOW && i > 0) {delay(200); i--; move_cursor(i+1,i);}
  if (digitalRead(ok)== LOW){
    if (i == 0) {EEPROM.update(mode,0);}
    else if (i == 1) {EEPROM.update(mode,1);}
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
  }
  if (digitalRead(back)== LOW){
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
  }
  break;

  case 3:
  GetNumber();
  tft.fillRect(60,40,tft.width(),tft.height(),ST7735_BLACK);
  tft.setCursor(20,40);
  tft.print("1.SP = ");
  tft.print(Sp);
  tft.setCursor(20,57);
  tft.print("2.KP = ");
  tft.print(Kp);
  tft.setCursor(20,74);
  tft.print("3.KI = ");
  tft.print(Ki);
  tft.setCursor(20,91);
  tft.print("4.KD = ");
  tft.print(Kd);
  tft.setCursor(20,108);
  tft.print("5.G  = ");
  tft.print(gain);
    switch (i)
    {
      case 0:
      Sp=num;
      break;
      case 1:
      Kp=num;
      break;
      case 2:
      Ki=num;
      break;
      case 3:
      Kd=num;
      break;
      case 4:
      gain=num;
      break;
    }
    
  if (digitalRead(down)==LOW && i < 4) {delay(200); i++; move_cursor(i-1,i);num=0;}
  if (digitalRead(up)==LOW && i > 0) {delay(200); i--; move_cursor(i+1,i);num=0;}
  if (digitalRead(ok)==LOW){
    EEPROM.update(sp,Sp);EEPROM.update(kp,Kp);EEPROM.update(ki,Ki);EEPROM.update(kd,Kd);EEPROM.update(GAIN,gain);
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
    num = 0;
  }
  if (digitalRead(back)==LOW)
  {
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
    num = 0;
  }
  break;

  case 4:
  if (digitalRead(down)==LOW && i < 1) {delay(200); i++; move_cursor(i-1,i);}
  if (digitalRead(up)==LOW && i > 0) {delay(200); i--; move_cursor(i+1,i);}
  if (digitalRead(ok)== LOW)
  {
    if (i == 0) {EEPROM.write(kmode,0);}
    else if (i == 1) {EEPROM.write(kmode,1);}
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
  }
  if (digitalRead(back)== LOW)
  {
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
  }
  break;

  case 5:
  
  sensor_ping();
  tft.fillRect(50,40,tft.width(),tft.height(),ST7735_BLACK);
  tft.setCursor(20,40);
  tft.print("1.jarak = ");
  tft.print(jarak);
  delay(50);
  
  if (digitalRead(back)== LOW || digitalRead(ok)== LOW){
    delay(200); tft.fillScreen(ST7735_BLACK);
   kanan();analogWrite(pwm,0);
    menu=0;disp0();
  }
  
  if (jarak < 20){kanan();analogWrite(pwm,200); yeah="kanan";} 
  else if (jarak > 40){kiri();analogWrite(pwm,200); yeah = "kiri";}
  else {kanan();analogWrite(pwm,0); yeah = "diam";}

//  test_motor();
  
  tft.setCursor(20,80);
  tft.print("3. arah = ");
  tft.print(yeah);
  
  break;

  //On-Off
  case 6:
  sensor_ping();
  tft.fillRect(60,40,tft.width(),tft.height(),ST7735_BLACK);
  tft.setCursor(20,40);
  tft.print("1.R  = ");
  tft.print(jarak);
  tft.setCursor(20,57);
  tft.print("2.SP = ");
  tft.print(EEPROM.read(sp));
  tft.setCursor(20,74);
  tft.print("2.G  = ");
  tft.print(EEPROM.read(GAIN));
  onoff(jarak,EEPROM.read(sp));
  delay(50);
  
  if (digitalRead(back)== LOW || digitalRead(ok)== LOW){
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
  }
  break;

  case 7:
  sensor_ping();
  tft.fillRect(60,40,tft.width(),tft.height(),ST7735_BLACK);
  tft.setCursor(20,40);
  tft.print("1.R  = ");
  tft.print(jarak);
  tft.setCursor(20,57);
  tft.print("2.SP = ");
  tft.print(EEPROM.read(sp));
  tft.setCursor(20,74);
  tft.print("3.E  = ");
  tft.print(err);
  tft.setCursor(20,91);
  tft.print("4.PID= ");
  tft.print(PID);
  if(jarak<50 && jarak>4){
    hitungPID(jarak,EEPROM.read(sp),EEPROM.read(kp),EEPROM.read(ki),EEPROM.read(kd));
  }
  
  Serial.print(millis()-start_time);
    Serial.print("\t");
      Serial.print(EEPROM.read(sp));
    Serial.print("\t");
      Serial.println(jarak);
      
  if (digitalRead(back)== LOW || digitalRead(ok)== LOW)
  {
    delay(200); tft.fillScreen(ST7735_BLACK);menu=0;disp0();
  }
  break;
  
 }
}

void test_motor(){

  for (int i = 0; i<255; i++){
  kiri();analogWrite(pwm,i); //nge-jauh
  
  tft.setCursor(20,60);
  tft.print("2. pwm = ");
  tft.print(i);
  delay(100);
  }
  for (int i = 0; i<255; i++){
  kanan();analogWrite(pwm,255-i); //nge-narik 
  tft.setCursor(20,60);
  tft.print("2. pwm = ");
  tft.print(i);
  delay(100);
  }  
  }

void kanan(){
  digitalWrite(dir_kiri, LOW);
  digitalWrite(dir_kanan, HIGH);
  }
void kiri(){
  digitalWrite(dir_kanan, LOW);
  digitalWrite(dir_kiri, HIGH);
  }
