
const int EndMech_FINISH_Pin29 = 29; //end   position

const int EndMech_START_Pin27 = 27; //start position

const int BUTTON_TO_FINISH_Pin25 = 25; //кнопка идти в end position

const int BUTTON_TO_START_Pin27 = 23; //кнопка идти в start position

String inString = ""; //incoming str from COM
long    GO       = 0;  //START PROGRAM - угол

long STOP_SIGNAL = 0; //если пришло на COM число 0 - то стоп

int stepPin8 = 8; //

int dirPin9 = 9;

int enblPin10 = 10;

int duration1 = 50; //задержка после STEP ON оптимально 50

int duration2 = 100; //задержка после STEP OFF оптимально 100

boolean rotation = LOW; //low - сгиб    high - разгиб

boolean start;
boolean finish;

boolean state_button_start;
boolean state_button_finish;


int ledOnPin49 = 49;

int ledOffPin51 = 51;

long count;

int once_on_start = 0; //триггер, один раз при включении

int MAX_DEGREE = 52;
int MAX_STEP = 144; //всего шагов
int COUNT_IMP_IN_ONE_STEP = 3250; //количество импульсов в одном обороте

long current_step = 0;

void setup() {

  //pinMode(LED_BUILTIN, OUTPUT);

  pinMode(stepPin8, OUTPUT);

  pinMode(dirPin9, OUTPUT);

  pinMode(EndMech_FINISH_Pin29, INPUT);

  pinMode(EndMech_START_Pin27, INPUT);

  pinMode(BUTTON_TO_FINISH_Pin25, INPUT);

  pinMode(BUTTON_TO_START_Pin27, INPUT);

  //pinMode(enblPin10, OUTPUT);

  digitalWrite(stepPin8, LOW);

  digitalWrite(dirPin9, rotation);

  Serial.begin(9600);

  pinMode(ledOnPin49, OUTPUT);
  pinMode(ledOffPin51, OUTPUT);

  digitalWrite(ledOffPin51, HIGH);

  start  = digitalRead(EndMech_START_Pin27);
  finish = digitalRead(EndMech_FINISH_Pin29);

  state_button_start = digitalRead(BUTTON_TO_START_Pin27);
  state_button_finish = digitalRead(BUTTON_TO_FINISH_Pin25);


}

void loop() {

  if (once_on_start == 0) {

    initialisation();
    once_on_start = 1;
  }

  state_button_start = digitalRead(BUTTON_TO_START_Pin27);
  state_button_finish = digitalRead(BUTTON_TO_FINISH_Pin25);

  if (state_button_finish == LOW) {
    rotation = HIGH;
  }

  if (state_button_start == LOW) {
    rotation = LOW;
  }

  start  = digitalRead(EndMech_START_Pin27);
  finish = digitalRead(EndMech_FINISH_Pin29);


  if (state_button_finish == HIGH || state_button_start == HIGH) {
    count = 3250; //для кнопок
    RotateCycle (count, duration1, duration2, rotation);
    calc_current_step(count, rotation);
  }

  if (GO > 0) {

   //GO = constrain(c, 1, MAX_DEGREE);
   //GO = map(GO, 1, MAX_DEGREE, 1, MAX_STEP);
   //GO = constrain(c, 1, MAX_STEP);
    
   count = 0L;
    
    if (GO > current_step) {
      rotation = LOW;
      count = (long)COUNT_IMP_IN_ONE_STEP * (long)(GO - current_step);
    }
    if (GO < current_step) {
      rotation = HIGH;
      count = (long)COUNT_IMP_IN_ONE_STEP * (long)(current_step - GO);
    }


    //Serial.print("current ");
    //Serial.println(current_step);
    //Serial.print("GO ");
    //Serial.println(GO);
    //Serial.print("count ");
    //Serial.println(count);
    
    if (count > 0) {
      RotateCycle (count, duration1, duration2, rotation);
      calc_current_step(count, rotation);
    }
  
  }
    //Serial.println(current_step);
}


void RotateCycle (long count, int duration1, int duration2, bool dir)
{

  start = digitalRead(EndMech_START_Pin27);
  finish = digitalRead(EndMech_FINISH_Pin29);

  led_work();

  digitalWrite (dirPin9, dir);

  for (long a = 1; a < count; a++) {

    if (start == LOW && dir == HIGH)
    {
      break;
    }
    if (finish == LOW && dir == LOW)
    {
      break;
    }
    if (GO == 0L) //сигнал стоп
    {
      break;
    }
    
    digitalWrite(stepPin8, HIGH);
    delayMicroseconds (duration1);
    digitalWrite(stepPin8, LOW);
    delayMicroseconds (duration2);

    start = digitalRead(EndMech_START_Pin27);
    finish = digitalRead(EndMech_FINISH_Pin29);

  }
  led_stop();
}

void serialEvent() {

  int c; //результат = число из строки

  //Serial.println(current_step);

  while (Serial.available() > 0) {

    int inChar = Serial.read();

    c = 0;
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {

      c = inString.toInt();
      
      // clear the string for new input:
      inString = "";

      if (c > 0) {
        GO = (long)c;
      }
      if (c <= 0) {
        GO = (long)0;
      }
    }
  }
}

void initialisation() { //при загрузке увести в начальное положение и сместить в 1

  rotation = HIGH;
  digitalWrite (dirPin9, rotation);

  led_work();

  while (start == HIGH) {

    for (int a = 1; a < 2; a++) {

      digitalWrite(stepPin8, HIGH);

      delayMicroseconds (duration1);

      digitalWrite(stepPin8, LOW);

      delayMicroseconds (duration2);

      start = digitalRead(EndMech_START_Pin27);

    }
  }
  led_stop();
  current_step = 0;
  delay(100);
  GO = 1;
}

void led_work() {
  digitalWrite(ledOnPin49, HIGH);
  digitalWrite(ledOffPin51, LOW);
}

void led_stop() {
  digitalWrite(ledOnPin49, LOW);
  digitalWrite(ledOffPin51, HIGH);
}

void calc_current_step(long COUNT_IMP, bool rotation) {
  //Serial.println(COUNT_IMP);
  if (rotation == LOW) {
    current_step = current_step + (long)abs(COUNT_IMP / COUNT_IMP_IN_ONE_STEP);
  }
  if (rotation == HIGH) {
    current_step = current_step - (long)abs(COUNT_IMP / COUNT_IMP_IN_ONE_STEP);
  }
}
