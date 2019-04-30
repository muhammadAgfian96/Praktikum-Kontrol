#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define interrupts() sei()
#define noInterrupts() cli()

Servo myservo;  // create servo object to control a servo

int pos = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile int NbTopsFan;
int Calc;
//int hallsensor = 2;
int imenu = 0, imenuTemp = 0;
int up = 6, down = 8, ok = 7;
int P = 0, I = 1000, D = 0, Sp = 0, err, errt, errI = 0;
int cnt = 0;
int i = 0, ertmp[20];
float PID;

volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
long unsigned int waktu = 0;

void flow () {
  flow_frequency++;
}
void baca_sensor(){
  /*
  sumber kodingan sensor: https://www.electroschematics.com/12145/working-with-water-flow-sensors-arduino/
  */
   currentTime = millis();
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
//      detachInterrupt(digitalPinToInterrupt(0));
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
//      sei();
      flow_frequency = 0; // Reset Counter
   }
}
void setup()
{
//  pinMode(hallsensor, INPUT);
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   
  lcd.begin();
  myservo.attach(10);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WWW.DINOTERA.COM");
  lcd.setCursor(0, 1);
  lcd.print("  FLOW CONTROL  ");
  delay(300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("kami sayang");
  lcd.setCursor(0, 1);
  lcd.print("kalian :* ");
  delay(500);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), flow, FALLING);
  
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
//  attachInterrupt(00, rpm, RISING);

}

void loop() {
  menu();
}

void operasi()
{
  lcd.clear();
  waktu = millis();
  Serial.println("\n\n==MULAI AMBIL DATA YA SAYANG==\n\nwaktu(ms)\tset point \tflow");
  
  while (imenu > 0) {
   
    baca_sensor();
    //seting servo
    err = Sp - l_hour;
    PID  = 0;
    errI = errI + err;
    PID = (P * err * 0.1) + ((P / (I * 0.1)) * errI * 0.001 * 0.5) + P * 0.1 * D * (err - errt) * 0.001 * 0.02;
    errt = err;
    PID = pos + PID;
    if (PID < 0) PID = 0;
    if (PID >= 90) PID = 90;

    myservo.write(90-PID);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Q(L/h):");
    lcd.print(l_hour);

    lcd.setCursor(13, 0);
    if (Sp / 100 <= 0 ) lcd.print(" ");
    if (Sp / 10 <= 0) lcd.print(" ");
    lcd.print(Sp);
//    lcd.print(err);

    lcd.setCursor(0, 1);
    lcd.print("Teta  :");
    lcd.print(PID);
    lcd.setCursor(13, 1);
    lcd.print("deg");
    cnt++;

    Serial.print(millis() - waktu);
    Serial.print("\t");
    Serial.print(Sp);
    Serial.print("\t");
    Serial.println(l_hour, DEC);

    if (digitalRead(down) == 1) {
      imenu = -1;
      sei();
      for (i = 0; i < 10; i++) ertmp[i] = 0;
    }
  }
  delay(100);
}

void manualSet() {
  while (imenu > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setting  Servo ");
    lcd.setCursor(0, 1);
    lcd.print(pos);
    lcd.print(" deg");
    if (digitalRead(up) == 1) {
      if (pos < 90) pos += 5;
    }
    else if (digitalRead(down) == 1) {
      if (pos > 0) pos -= 5;
    }

    else if (digitalRead(ok) == 1) {
      imenu = -1;
    }
    delay(100);
  }
  delay(300);
}

void SetP() {
  while (imenu > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setting Kp ");
    lcd.setCursor(0, 1);
    lcd.print("0.1 x ");
    lcd.print(P);
    //lcd.print(" deg");
    if (digitalRead(up) == 1)
    {
      if (P < 100) {
        P += 1;
      }
    }
    if (digitalRead(down) == 1)
    {
      if (P > 0) {
        P -= 1;
      }
    }

    if (digitalRead(ok) == 1) imenu = -1;
    delay(100);
  }
  delay(300);
}

void SetI() {
  while (imenu > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setting Ti ");
    lcd.setCursor(0, 1);
    lcd.print("0.1 * ");
    lcd.print(I);
    //lcd.print(" deg");
    if (digitalRead(up) == 1)
      if (I < 10000) I += 1;
    if (digitalRead(down) == 1)
      if (I > 0) I -= 5;

    if (digitalRead(ok) == 1) imenu = -1;
    delay(100);
  }
  delay(300);
}

void SetD() {
  while (imenu > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setting Td ");
    lcd.setCursor(0, 1);
    lcd.print("0.01 * ");
    lcd.print(D);
    //lcd.print(" deg");
    if (digitalRead(up) == 1)
      if (D < 100) D += 1;
    if (digitalRead(down) == 1)
      if (D > 0) D -= 1;

    if (digitalRead(ok) == 1) imenu = -1;
    delay(100);
  }
  delay(300);
}

void SetPt() {
  while (imenu > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setting SetPoint");
    lcd.setCursor(0, 1);
    lcd.print(Sp);
    lcd.print(" L/H");
    if (digitalRead(up) == 1)
      if (Sp < 1000) Sp += 5;
    if (digitalRead(down) == 1)
      if (Sp > 0) Sp -= 1;

    if (digitalRead(ok) == 1) imenu = -1;
    delay(100);
  }
  delay(300);
}


void trial() {
  lcd.clear();
  waktu = millis();
  Serial.println("\n\n==MULAI AMBIL DATA==\n\nwaktu(ms)\tset point \tflow");

  while (imenu > 0) {
    //sensor
//    NbTopsFan = 0;
////    attachInterrupt(digitalPinToInterrupt(0), rpm, RISING);
//    sei();
//    delay (500);
//    detachInterrupt(digitalPinToInterrupt(0));
//    Calc = (NbTopsFan * 2.0 * 60.0 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour

    baca_sensor();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Q(L/h):");
    lcd.print(l_hour);

    lcd.setCursor(0, 1);
    lcd.print(pos);
    lcd.print(" deg");

    lcd.setCursor(8, 1);
    lcd.print(flow_frequency);
    lcd.print("ff");

    //tombol
    myservo.write(pos);

    Serial.print(millis() - waktu);
    Serial.print("\t");
    Serial.print(Sp);
    Serial.print("\t");
    Serial.println(l_hour, DEC);

    if (digitalRead(up) == 1) {
      if (pos < 90) pos += 90;
    }
    else if (digitalRead(down) == 1) {
      if (pos > 0) pos -= 90;
    }

    if (digitalRead(ok) == 1) {
      imenu = -1;
    }
    if (pos > 90) {
      pos = 90;
    }
    if (pos < 0) {
      pos = 0;
    }
    delay(100);

  }
}

void menu() {
  cnt = 0;
  if (imenu < 0) imenu = 1;
  if (imenu == 0) {
    lcd.setCursor(0, 0);
    lcd.print("kami sayang");
    lcd.setCursor(0, 1);
    lcd.print("kalian :* (ok)");
    if (digitalRead(ok) == 1) {
      delay(300);
      imenu = 1;
    }
  }

  if (imenu > 0) {
    delay(200);
    if (digitalRead(down) == 1) {
      if (imenu >= 7) imenu = 1;
      else imenu++;
    }
    if (digitalRead(up) == 1) {
      if (imenu <= 1) imenu = 7;
      else imenu--;
    }


    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("      MENU      "); \
    lcd.setCursor(0, 1);
    if (imenu == 1) lcd.print("1. Start System ");
    if (imenu == 2) lcd.print("2. Setting Servo");
    if (imenu == 3) lcd.print("3. Set Kp kuy!  ");
    if (imenu == 4) lcd.print("4. Set Ti yuk!  ");
    if (imenu == 5) lcd.print("5. Set Td yok!  ");
    if (imenu == 6) lcd.print("6. Nge-set SP   ");
    if (imenu == 7) lcd.print("7. Trial    say ");


    if (digitalRead(ok) == 1) {
      delay(100);
      errI = 0;
      if (imenu == 1) operasi();
      else if (imenu == 2) manualSet();
      else if (imenu == 3) SetP();
      else if (imenu == 4) SetI();
      else if (imenu == 5) SetD();
      else if (imenu == 6) SetPt();
      else if (imenu == 7) trial();
      delay(100);
    }
  }

}
