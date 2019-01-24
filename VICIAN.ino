int dir = 0;                               //направление 0 - стоп, 1 - идти в end position, 2 - идти в start position
int triga = 0;
int MAXSPEED    = 15000;                   //максимальное кол-во STEP)
int GEAR_COUNT  = 9;                       //делитель   (макс. кол-во "передач")
int GEAR;                                  //кол-во STEP в одной "передаче"

//пины концевиков
const int EndMech_FINISH_Pin29 = 29;       //end  position
const int EndMech_START_Pin27 = 27;        //start position

//пины кнопок
const int BUTTON_TO_FINISH_Pin25 = 25;     //кнопка идти в end position
const int BUTTON_TO_START_Pin27 = 23;      //кнопка идти в start position

//состояние концевиков
boolean start;
boolean finish;

//состояние кнопок
boolean state_button_start;
boolean state_button_finish;

//пины LED
int ledOnPin49 = 49;
int ledOffPin51 = 51;

String inString = "";    //incoming str from COM

//формат обмена = 3 цифры
int Digit_1 = 0; //направление
int Digit_2 = 0; //текущая "передача"
int Digit_3 = 0; //зарезервировано

#include <AccelStepper.h>

AccelStepper stepper (1, 8, 9);

void setup()
{
  stepper.setMaxSpeed(MAXSPEED);
  Serial.begin(9600);

  //режим пинов концевиков
  pinMode(EndMech_FINISH_Pin29, INPUT);
  pinMode(EndMech_START_Pin27, INPUT);

  //режим пинов кнопок
  pinMode(BUTTON_TO_FINISH_Pin25, INPUT);
  pinMode(BUTTON_TO_START_Pin27, INPUT);

  //режим пинов LED
  pinMode(ledOnPin49, OUTPUT);
  pinMode(ledOffPin51, OUTPUT);

  //считываем сотояние концевиков
  start  = digitalRead(EndMech_START_Pin27);
  finish = digitalRead(EndMech_FINISH_Pin29);

  //считываем сотояние кнопок
  state_button_start = digitalRead(BUTTON_TO_START_Pin27);
  state_button_finish = digitalRead(BUTTON_TO_FINISH_Pin25);

  led_stop();
}

void loop()
{

  //считываем состояние концевиков
  start = digitalRead(EndMech_START_Pin27);
  finish = digitalRead(EndMech_FINISH_Pin29);

  //
  if (start == LOW && dir == 1)
  {
    stepper.stop();
    led_stop();
    return;
  }
  if (finish == LOW && dir == -1)
  {
    stepper.stop();
    led_stop();
    return;
  }

  //считываем состояние кнопок
  state_button_start = digitalRead(BUTTON_TO_START_Pin27);
  state_button_finish = digitalRead(BUTTON_TO_FINISH_Pin25);

  if (state_button_finish == LOW) {
    dir = -1;
  }

  if (state_button_start == LOW) {
    dir = 1;
  }

  if (Digit_1 == 0) {
    stepper.stop();
    led_stop();
  }
  if (Digit_2 == 0) {
    stepper.stop();
    led_stop();
  }
  if (Digit_3 == 0) {
    stepper.stop();
    led_stop();
  }
  else if (Digit_1 == 1) {
    dir = -1;
  }
  else if (Digit_1 == 2) {
    dir = 1;
  }
  //else {
  //  stepper.stop();
  //  led_stop();
  //}

  //при смене направления остановится
  if (triga != dir) {
    stepper.stop();
    led_stop();
  }

  if (state_button_finish == HIGH) {
    //с кнопок - поедем на половине максималки
    dir = -1;
    Digit_2 = MAXSPEED/2;
    GEAR    = 1;
  }
  else if (state_button_start == HIGH) {
    dir = 1;
    Digit_2 = MAXSPEED/2;
    GEAR    = 1;
  }
  else if (state_button_finish == HIGH && state_button_start == HIGH){
    Digit_1 = 0;
    Digit_2 = 0;
    Digit_3 = 0;
    stepper.stop();
    return;
    led_stop();
  }
  else {
    GEAR    = get_gear(MAXSPEED, GEAR_COUNT);
  }

  triga = dir;

  stepper.setSpeed(dir * GEAR * Digit_2);
  stepper.runSpeed();
  led_work();

}

void serialEvent() {

  while (Serial.available() > 0) {

    byte inChar = Serial.read();

    //c = 0;

    if (isAlphaNumeric(inChar)) {
      // конвертировать входящий байт в символ и склеить:
      inString += (char)inChar;
    }
    // если приняли символ Enter, обрабатываем:
    if (inChar == '\n') {

      if (inString.length() == 3) {

        Digit_1 = map(inString.charAt(0), 48, 57, 1, 10) - 1;
        Digit_2 = map(inString.charAt(1), 48, 57, 1, 10) - 1;
        Digit_3 = map(inString.charAt(2), 48, 57, 1, 10) - 1;

      }
      else {
        if (inString.startsWith("max")) {
          MAXSPEED = inString.substring(3).toInt();
        }
        if (inString.startsWith("gear")) {
          GEAR_COUNT = inString.substring(4).toInt();
        }
      }
      // очистим для нового ввода:
      inString = "";
    }
  }
}

int get_gear(int MAX_SPEED, int G_COUNT) {
  if (G_COUNT == 0) {
    return 0;
  }
  return MAX_SPEED / G_COUNT;
}

int get_dir() {
  
}

bool get_permission(){
  
}

void led_work() {
  digitalWrite(ledOnPin49, HIGH);
  digitalWrite(ledOffPin51, LOW);
}

void led_stop() {
  digitalWrite(ledOnPin49, LOW);
  digitalWrite(ledOffPin51, HIGH);
}
