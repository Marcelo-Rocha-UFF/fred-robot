#include <VarSpeedServo.h>
#include <Adafruit_NeoPixel.h>
#include "LedControl.h"

#include "FredSymbols.h" // All symbols definitions
// #include "FredControlGirl.h" // Girl parameters
#include "FredControlBoy.h" // Boy parameters

#define STATUS_FREE "ST:FREE   " // 10 bytes de tamanho
#define STATUS_POSE "ST:POSE   "
#define STATUS_LEDS "ST:LEDS   "
#define STATUS_MOVE "ST:MOVE   "
#define STATUS_STMP "ST:STMP   "

char LEG_LEFT_PIN = 2;
char FOOT_LEFT_PIN = 3;
char LEG_RIGHT_PIN = 4;
char FOOT_RIGHT_PIN = 5;

// Sensores de toque
int TOUCH_HEAD_PIN = 7;
int TOUCH_HEAD_VALUE = 0;
int TOUCH_LEFT_PIN = 8;
int TOUCH_LEFT_VALUE = 0;
int TOUCH_RIGHT_PIN = 9;
int TOUCH_RIGHT_VALUE = 0;

// variaveis de estado do robô
char veloc_srv = 40; // servo movement velocity
char state_pose = NULL; // estado da posição inicial
char state_expression = NULL; // estado da expressão (display) inicial
char state_exp_aux = NULL; // variavel mantem seu estado mesmo depois da funcao terminar
char state_led = NULL; // estado do anel de leds

unsigned long eye_blinking_mills = millis(); // eye blinking
unsigned long eye_blinking_delay = 5000; // delay eye blinking
unsigned long foot_mills = millis(); // foot effect
unsigned long foot_delay = 5000; // delay foot effect
unsigned long led_speech_mills = millis(); // LED speech effect
unsigned long led_speech_delay = 400; // LED speech delay effect

VarSpeedServo leg_left_srv;
VarSpeedServo foot_left_srv;
VarSpeedServo leg_right_srv;
VarSpeedServo foot_right_srv;

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn (azul ou verde)
 pin 11 is connected to the CLK     (cinza ou marron)
 pin 10 is connected to LOAD    (branco ou laranja)
 We have 3  MAX72XX.
 */
LedControl lc=LedControl(12,11,10,3);
int delay_leds = 200; // determina a velocidade do efeito dos leds. Quanto maior o valor mais lento.

// defining expressions vars
Expression fred;
Expression in_love;
Expression broken;
Expression neutral;
Expression pleased;
Expression happy;
Expression sad;
Expression angry, angry2;
Expression surprised, surprised2;
Expression afraid;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define PIN 6
Adafruit_NeoPixel ring = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);
 

void setup() {

  Serial.begin(9600);

  pinMode(TOUCH_HEAD_PIN, INPUT);
  pinMode(TOUCH_LEFT_PIN, INPUT);
  pinMode(TOUCH_RIGHT_PIN, INPUT);

  fred.right_eye = fr;
  fred.left_eye = ed;
  fred.mouth = heart;

  in_love.right_eye = r_eye_in_love;
  in_love.left_eye = l_eye_in_love;
  in_love.mouth = m_in_love;

  broken.right_eye = r_eye_broken;
  broken.left_eye = l_eye_in_broken;
  broken.mouth = m_in_broken;

  neutral.right_eye = r_eye_neutral;
  neutral.left_eye = l_eye_neutral;
  neutral.mouth = m_neutral;

  pleased.right_eye = r_eye_pleased;
  pleased.left_eye = l_eye_pleased;
  pleased.mouth = m_pleased;

  happy.right_eye = r_eye_happy;
  happy.left_eye = l_eye_in_happy;
  happy.mouth = m_in_happy;

  sad.right_eye = r_eye_sad;
  sad.left_eye = l_eye_in_sad;
  sad.mouth = m_in_sad;

  angry.right_eye = r_eye_angry;
  angry.left_eye = l_eye_in_angry;
  angry.mouth = m_in_angry;

  angry2.right_eye = r_eye_angry2;
  angry2.left_eye = l_eye_in_angry2;
  angry2.mouth = m_in_angry2;

  surprised.right_eye = r_eye_surprised;
  surprised.left_eye = l_eye_in_surprised;
  surprised.mouth = m_in_surprised;

  surprised2.right_eye = r_eye_surprised;
  surprised2.left_eye = l_eye_in_surprised;
  surprised2.mouth = m_in_surprised2;

  afraid.right_eye = r_eye_afraid;
  afraid.left_eye = l_eye_in_afraid;
  afraid.mouth = m_in_afraid;
  
    /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  lc.shutdown(1,false);
  lc.shutdown(2,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,1);
  lc.setIntensity(1,1);
  lc.setIntensity(2,1);
  /* and clear the display */
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  lc.clearDisplay(2);

  ring.begin();
  ring.setBrightness(5); //adjust brightness here
  ring.show(); // Initialize all pixels to 'off'
  
  expression_show(fred); 
  delay(3000);
  colorWipe(ring.Color(255, 0, 0), delay_leds, 'r'); // Black (off)
  state_led = 'r';
  expression_show(broken);
  state_expression = 'b';
  servos_attach();
  pose_init('1');
}
 
void loop() {
  run();
}

///////////////////////////////////////////////////////////////// Função principal que roda as outras funções //////////////////////////
// looping principal
void run(){
  while (1){
    processa_serial_port(); // 
    blinking_eyes(); // pisca olhos aleatoreamente
    anim_feet(); // move os pés do FRED aleatoreamente
    sense_touch(); // Verifica se o robo foi tocado e muda o seu estado, executando a animaçao
  }
}


///////////////////////////////////////////////////////////////// Funções para os LEDs RGB ////////////////////////////////////////
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait, char type) {
  if (state_expression == 'T') {
    state_led = type;
    servos_attach();
  }
  else {
    servos_detach();
    delay(1); // evita o truncamento no envio pela porta serial
    //Serial.print(STATUS_LEDS);
    for(uint16_t i=0; i<ring.numPixels() / 4 + 1; i++) {
      ring.setPixelColor(4 - i, c);
      ring.setPixelColor(4 + i, c);
      ring.setPixelColor(12 - i, c);
      ring.setPixelColor(12 + i, c);
      ring.show();
      delay(wait);
//      blinking_eyes();
    }
    delay(1);
    //Serial.print(STATUS_FREE);
  }
  state_led = type;
  servos_attach();
}

// efeito no LED usado na fala do robô
void colorSpeech(uint32_t c, bool type) {
  servos_detach();
  if (type == false){ // acende os LEDS pares
    for(uint16_t i=0; i<=ring.numPixels(); i++) {
        if (i % 3 == 0){
          ring.setPixelColor(i, c);
//          ring.show();
        }
        else {
          ring.setPixelColor(i, (0, 0, 0));
//          ring.show();
        }
        ring.show();
    }
  }
  else { // acende os LEDs ímpares
    for(uint16_t i=0; i<=ring.numPixels(); i++) {
        if (i % 3 != 0){
          ring.setPixelColor(i, c);
        }
        else {
          ring.setPixelColor(i, (0, 0, 0));
        }
        ring.show();
    }
  }
}
 

void rainbow(uint8_t wait, char type) {
  servos_detach();
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_LEDS);
  uint16_t i, j;
  // primeira vez
  for(j=0; j<256; j++) {
    for(i=0; i<ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel((i+j) & 255));
    }
    ring.show();
    delay(wait);
  }
  state_led = type;
  delay(1);
  //Serial.print(STATUS_FREE);
  servos_attach();
}
 
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  servos_detach();
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_LEDS);
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel(((i * 256 / ring.numPixels()) + j) & 255));
    }
    ring.show();
    delay(wait);
  }
  delay(1);
  //Serial.print(STATUS_FREE);
  servos_attach();
}
 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

///////////////////////////////////////////////////////////////// Função que mostra uma Expressão no DISPLAY ///////////////////////////////
// show an expression
void expression_show(Expression expression){
  char state_expression_aux = state_expression; // para verificar se state_expression igual a "T"
  if (expression.right_eye == fr)
    state_expression = 'f';
  else if (expression.right_eye == r_eye_in_love)
    state_expression = 'i';
  else if (expression.right_eye == r_eye_broken)
    state_expression = 'b';
  else if (expression.right_eye == r_eye_neutral)
    state_expression = 'n';
  else if (expression.right_eye == r_eye_neutral_blink)
    state_expression = '-';
  else if (expression.right_eye == r_eye_pleased)
    state_expression = 'p';
  else if (expression.right_eye == r_eye_pleased_blink)
    state_expression = '-';
  else if (expression.right_eye == r_eye_happy)
    state_expression = 'h';
  else if (expression.right_eye == r_eye_sad)
    state_expression = 's';
  else if (expression.right_eye == r_eye_angry)
    state_expression = 'a';
  else if (expression.right_eye == r_eye_angry2)
    state_expression = 'A';
  else if (expression.right_eye == r_eye_surprised)
    state_expression = 'u';
  else if (expression.right_eye == r_eye_afraid)
    state_expression = 'r';
  matrix0_write(expression.right_eye);
  matrix1_write(expression.left_eye);
  matrix2_write(expression.mouth);
  if (state_expression_aux == 'T') state_expression = state_expression_aux; // isso tem influência nas animações dos leds quendo falando.
}


///////////////////////////////////////////////////////////////// Funções de controle individual de cada matriz 8x8 //////////////////////////
void matrix0_write(char * symbol)
{
  lc.setRow(0,0,symbol[0]);
  lc.setRow(0,1,symbol[1]);
  lc.setRow(0,2,symbol[2]);
  lc.setRow(0,3,symbol[3]);
  lc.setRow(0,4,symbol[4]);
  lc.setRow(0,5,symbol[5]);
  lc.setRow(0,6,symbol[6]);
  lc.setRow(0,7,symbol[7]);
}

void matrix1_write(char * symbol)
{
  lc.setRow(1,0,symbol[0]);
  lc.setRow(1,1,symbol[1]);
  lc.setRow(1,2,symbol[2]);
  lc.setRow(1,3,symbol[3]);
  lc.setRow(1,4,symbol[4]);
  lc.setRow(1,5,symbol[5]);
  lc.setRow(1,6,symbol[6]);
  lc.setRow(1,7,symbol[7]);
}

void matrix2_write(char * symbol)
{
  lc.setRow(2,0,symbol[0]);
  lc.setRow(2,1,symbol[1]);
  lc.setRow(2,2,symbol[2]);
  lc.setRow(2,3,symbol[3]);
  lc.setRow(2,4,symbol[4]);
  lc.setRow(2,5,symbol[5]);
  lc.setRow(2,6,symbol[6]);
  lc.setRow(2,7,symbol[7]);
}

///////////////////////////////////////////////////////////////// Funções que controlam as POSES /////////////////////////////////////////
// pose init
void pose_init(char type){ // alinha pernas e pés (tipo 1 = envia info de pose para o nodemcu)
  if (state_pose != 'i') {
    servos_attach();
    char veloc_aux = veloc_srv; //salva o valor da velocidade global
    veloc_srv = 45; // define velocity for init  
    if (type == '1'){ // (tipo 1 = envia info de pose para o nodemcu)
      delay(1); // evita o truncamento no envio pela porta serial
      //Serial.print(STATUS_POSE);
    }
    leg_left_srv.write(LEG_LEFT_EQ, veloc_srv);
    leg_right_srv.write(LEG_RIGHT_EQ, veloc_srv);
    while (leg_left_srv.isMoving()){ delay(1); }
    while (leg_right_srv.isMoving()){ delay(1); }
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv);
    foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv);
    while (foot_left_srv.isMoving()){ delay(1); }
    while (foot_right_srv.isMoving()){ delay(1); }
    state_pose = 'i';
    veloc_srv = veloc_aux;
    foot_mills = millis(); // reinicia o contador para a animação aleatória dos peś
    if (type == '1'){ // (tipo 1 = envia info de pose para o nodemcu)
      delay(100); // evita o truncamento no envio pela porta serial
      //Serial.print(STATUS_FREE);
    }
  }
}

// pose up
void pose_up(char type){
  if (state_pose != 'l' || state_pose != 'L' || state_pose != 'r' || state_pose != 'r' && state_pose || 'u'){
    char velo_aux = veloc_srv; // saves global velocity
    veloc_srv = 60;
    delay(1); // evita o truncamento no envio pela porta serial
    //Serial.print(STATUS_POSE);
    pose_init('2'); // acerta a postura antes pose sem desligar os servos
    if (type == 'l'){
      foot_left_srv.write(140, veloc_srv, true);
      state_pose = 'l'; // state up left
    }
    else if (type == 'L'){
      foot_left_srv.write(30, veloc_srv, true);
      state_pose = 'L'; // state up left
    }
    else if (type == 'r'){
      foot_right_srv.write(25, veloc_srv, true);
      state_pose = 'r'; // state up right
    }
    else if (type == 'R'){
      foot_right_srv.write(140, veloc_srv, true);
      state_pose = 'R'; // state up right
    }
    else if (type == 'u'){ //type = 'u'
      veloc_srv = 70;
      foot_left_srv.write(130, veloc_srv, false);
      foot_right_srv.write(35, veloc_srv, false);
      
      while (foot_right_srv.isMoving()){ delay(1); }
      while (foot_left_srv.isMoving()){ delay(1); }
      state_pose = 'u'; // state up
    }
    servos_detach();
    veloc_srv = velo_aux;
    foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
    delay(100);
    //Serial.print(STATUS_FREE);
  }
}

// pose down broken
void pose_down(char param){
  if (state_pose != 'd' && state_pose != 'D'){
    char velo_aux = veloc_srv; // saves global velocity
    if (param == 'd')
      veloc_srv = 25; // more slow
    else if (param == 'D')
      veloc_srv = 50; // more fast
    delay(1); // evita o truncamento no envio pela porta serial
    //Serial.print(STATUS_POSE);
    pose_init('2'); // acerta a postura antes pose sem desligar os servos
    //servos_attach();
    foot_right_srv.write(155, veloc_srv);
    foot_left_srv.write(20, veloc_srv);
    while (foot_right_srv.isMoving()){ delay(1); }
    while (foot_left_srv.isMoving()){ delay(1); }
    //delay(500);
    //servos_detach();
    state_pose = 'd'; // state up
    veloc_srv = velo_aux;
    foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
    delay(100); // evita o truncamento no envio pela porta serial
    //Serial.print(STATUS_FREE);
  }
}

///////////////////////////////////////////////////////////////// Funções de controle do MOVIMENTO ///////////////////////////////////////
// moving forward
void move_forward(char cycles){
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 70;
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  servos_attach();
  while (cycles > 0){
    leg_right_srv.write(40, veloc_srv, true);
    foot_right_srv.write(60, veloc_srv, true);
    leg_right_srv.write(LEG_RIGHT_EQ, veloc_srv, true);
    foot_right_srv.write(FOOT_LEFT_EQ, veloc_srv, true);
    leg_left_srv.write(135, veloc_srv, true);
    foot_left_srv.write(110, veloc_srv, true);
    leg_left_srv.write(LEG_LEFT_EQ, veloc_srv, true);
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv, true);
    cycles -= 1;
  }
  pose_init("2"); // acerta a posição inicial e desliga os servos
  delay(100);
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
}


// moving backward
void move_backward(char cycles){
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 70;
  servos_attach();
  delay(500); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    leg_right_srv.write(130, veloc_srv, true); // 35
    //while (leg_right_srv.isMoving()){ delay(1); }
    foot_right_srv.write(60, veloc_srv, true); //45
    //while (foot_right_srv.isMoving()){ delay(1); }
    leg_right_srv.write(LEG_RIGHT_EQ, veloc_srv, true);
    foot_right_srv.write(FOOT_LEFT_EQ, veloc_srv, true);

    leg_left_srv.write(55, veloc_srv, true);
    //while (leg_left_srv.isMoving()){ delay(1); }
    foot_left_srv.write(110, veloc_srv, true); //135
    //while (foot_left_srv.isMoving()){ delay(1); }
    leg_left_srv.write(LEG_LEFT_EQ, veloc_srv, true);
    //while (leg_left_srv.isMoving()){ delay(1); }
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv, true);
    //while (foot_left_srv.isMoving()){ delay(1); }
    cycles -= 1;
  }
  pose_init('2'); // acerta a posição inicial e desliga os servos
  delay(100);
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
}


// move left
void move_left(char cycles){
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 70;
  servos_attach();
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    foot_right_srv.write(150, veloc_srv, true);
    foot_right_srv.write(FOOT_RIGHT_EQ-5, veloc_srv, true); 
    cycles -= 1;
  }
  foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv, true);
  pose_init('2'); // acerta a posição inicial e desliga os servos
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
  delay(100);
  //Serial.print(STATUS_FREE);
} 


// move right
void move_right(char cycles){
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 70;
  servos_attach();
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    foot_left_srv.write(20, veloc_srv, true);
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv, true); 

    cycles -= 1;
  }
  foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv, true);
  pose_init('2'); // acerta a posição inicial e desliga os servos
  delay(100); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
}


// move left moon
void move_left_moon(char cycles){
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 80;
  servos_attach();
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    foot_right_srv.write(30, veloc_srv, true);
    foot_left_srv.write(130, veloc_srv, true);
    foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv, true);
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv, true);
    cycles -= 1;
  }
  pose_init('2'); // acerta a posição inicial e desliga os servos
  delay(100); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés 
}


// move right moon
void move_right_moon(char cycles){
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 80;
  servos_attach();
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    foot_left_srv.write(140, veloc_srv, true);
    foot_right_srv.write(40, veloc_srv, true);
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv, true);
    foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv, true);
    cycles -= 1;
  }
  pose_init('2'); // acerta a posição inicial e desliga os servos
  delay(100);
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
}


// moon walker left and right
void move_moon_walk(char cycles){ // só move os pés, não usa a perna
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  servos_attach();
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 80;
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    move_left_moon(2);
    move_right_moon(2);
    cycles -= 1;
  }
  pose_init('2'); // acerta a posição inicial e desliga os servos
  delay(100);
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
}


// dancing movements
void move_dance1(char cycles){ // só move os pés, não usa a perna
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  servos_attach();
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 50;
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    foot_left_srv.write(45, veloc_srv);
    foot_right_srv.write(50, veloc_srv);
    while (foot_right_srv.isMoving()){ delay(1); }
    while (foot_left_srv.isMoving()){ delay(1); }
    foot_right_srv.write(130, veloc_srv);
    foot_left_srv.write(130, veloc_srv);
    while (foot_left_srv.isMoving()){ delay(1); }
    while (foot_right_srv.isMoving()){ delay(1); }
    foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv);
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv);
    while (foot_left_srv.isMoving()){ delay(1); }
    while (foot_right_srv.isMoving()){ delay(1); }
    cycles -= 1;
  }
  //delay(1000); //////////////////////////////////////////
  pose_init('2'); // acerta a posição inicial e desliga os servos
  delay(100);
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
  
}


// dancing 
void move_dance2(char cycles){ // só move os pés, não usa a perna
  delay(1); // evita o truncamento no envio pela porta serial
  //Serial.print(STATUS_MOVE);
  pose_init('2'); // acerta a posição inicial sem desligar os servos
  servos_attach();
  char velo_aux = veloc_srv; // saves global velocity
  veloc_srv = 50;
  state_pose = 'm'; // muda o estado do robô para algum tipo de movimentação
  while (cycles > 0){
    foot_left_srv.write(40, veloc_srv);
    foot_right_srv.write(130, veloc_srv);
    while (foot_right_srv.isMoving()){ delay(1); }
    while (foot_left_srv.isMoving()){ delay(1); }
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv);
    foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv);
    while (foot_right_srv.isMoving()){ delay(1); }
    while (foot_left_srv.isMoving()){ delay(1); }
    foot_left_srv.write(40, veloc_srv);
    foot_right_srv.write(130, veloc_srv);
    while (foot_right_srv.isMoving()){ delay(1); }
    while (foot_left_srv.isMoving()){ delay(1); }
    foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv);
    foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv);
    while (foot_right_srv.isMoving()){ delay(1); }
    while (foot_left_srv.isMoving()){ delay(1); }
    cycles -= 1;
  }
  pose_init('2'); // acerta a posição inicial e desliga os servos
  delay(100);
  //Serial.print(STATUS_FREE);
  state_pose = 'i';
  veloc_srv = velo_aux;
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
}


// stomping foot (right foot)
void move_stomping_foot(char type){
  int cycles = 4;
  pose_init('2');
  servos_attach();
  if (type == 's'){ // right foot
    state_pose = 's'; // muda o estado do robô para batendo o pé direito
    delay(1); // evita o truncamento no envio pela porta serial
    //Serial.print(STATUS_STMP);
    char velo_aux = veloc_srv; // saves global velocity
    veloc_srv = 30;
    leg_right_srv.write(60, veloc_srv, true);
    veloc_srv = 20;
    while (cycles > 0){
      foot_right_srv.write(115, veloc_srv);
      while (foot_right_srv.isMoving()){ delay(1); }
      foot_right_srv.write(FOOT_RIGHT_EQ - 10, veloc_srv);
      while (foot_right_srv.isMoving()){ delay(1); }
      cycles -= 1;
    }
  } else if (type == 'S'){ // left foot
    state_pose = 'S'; // muda o estado do robô para batendo o pé esquerdo
    delay(1); // evita o truncamento no envio pela porta serial
    //Serial.print(STATUS_STMP);
    char velo_aux = veloc_srv; // saves global velocity
    veloc_srv = 30;
    leg_left_srv.write(120, veloc_srv, true);
    veloc_srv = 18;
    while (cycles > 0){
      foot_left_srv.write(60, veloc_srv);
      while (foot_left_srv.isMoving()){ delay(1); }
      foot_left_srv.write(FOOT_LEFT_EQ + 10, veloc_srv);
      while (foot_left_srv.isMoving()){ delay(1); }
      cycles -= 1;
    }
  }
  pose_init('2');
  foot_mills = millis(); // reinicia o contador para o movimento automático dos pés
  delay(100);
  //Serial.print(STATUS_FREE);
}

///////////////////////////////////////////////////////////////// Funções de conexão dos SERVOMOTORES ////////////////////////////////////////
// conecta as portas aos servos
void servos_attach(){
  //config legs and feet
  foot_left_srv.attach(FOOT_LEFT_PIN);
  leg_left_srv.attach(LEG_LEFT_PIN);
  foot_right_srv.attach(FOOT_RIGHT_PIN);
  leg_right_srv.attach(LEG_RIGHT_PIN);
}


// desconecta os servos das suas portas
void servos_detach(){
  //config legs and feet
  foot_left_srv.detach();
  leg_left_srv.detach();
  foot_right_srv.detach();
  leg_right_srv.detach();
}


///////////////////////////////////////////////////////////////// Função que envia o STATUS do FRED pela porta SERIAL//////////////////////
void show_status(){
  //Serial.println("----------------------------");
  //Serial.print("Pose (Status): ");
  //Serial.println(state_pose);
//  Serial.print("Expression (Status): ");
//  Serial.println(state_expression);
//  Serial.print("LEDS (Status): ");
//  Serial.println(state_led);
//  Serial.print("Servos (Global velocity): ");
//  Serial.println(veloc_srv, DEC);
//  Serial.println("----------------------------");
}


/////////////////////////////////////////////////////////////// Funções de controle de ANIMAÇÕES ////////////////////////////////////////
//animacao da boca falando
void speech_anim(char command){
  // 
  static uint32_t cor_anim;
  if ((command == 'T') && (state_expression != 'T')){ //
    if (state_expression != 'C') state_exp_aux = state_expression; // salva estado atual
    state_expression = 'T';
    led_speech_mills = millis(); // incializa o contador para o efeito dos LEDS
    delay(100); // o modulo de audio do ROS tem uma latencia de ~= 700ms
    bool led_type = false; //variavel booleana usada na alternacia na animação dos LEDs RGB
    while(state_expression == 'T'){
      // animação dos LEDs no tórax
      if (millis() - led_speech_mills >= led_speech_delay){
        if (state_led == 'r') // red
          cor_anim = ring.Color(255, 0, 0);
        else if (state_led == 'b') // blue
          cor_anim = ring.Color(0, 0, 255);
        else if (state_led == 'g') // green
          cor_anim = ring.Color(0, 255, 0);
        else if (state_led == 'w') // white
          cor_anim = ring.Color(255, 255, 255);
        else if (state_led == 'k') // black
          cor_anim = ring.Color(255, 255, 255); // se não tem cor no LED, fala com a cor branca
        else if (state_led == 'n') // rainbow
          cor_anim = ring.Color(0, 255, 0); 
        else if (state_led == 'p') // pink
          cor_anim = ring.Color(255, 0, 255);
        else if (state_led == 'y') // pink
          cor_anim = ring.Color(255, 191, 0);
        colorSpeech(cor_anim, led_type);
        led_type = !led_type;
        led_speech_mills = millis(); // reinicia o contador
      }  
      // animação da boca
      if (random(1, 4) == 1) {
        matrix2_write(m_open);
        delay(random(50, 150));
      } else if (random(1, 4) == 2) {
        matrix2_write(m_mid);
        delay(random(50, 150));
      }
      else if (random(1, 4) == 3) {
        matrix2_write(m_close);
        delay(random(100, 200));
      }
      blinking_eyes();
      processa_serial_port();
      anim_feet(); // move os pés do FRED aleatoreamente
    }
  } 
  // Mouth animatiom without leds animation
  else if ((command == 'C') && (state_expression != 'C')){ //
    if (state_expression != 'T') {
      state_exp_aux = state_expression; // salva estado atual
    } else {
      if (state_led == 'k') cor_anim = ring.Color(0, 0, 0); // caso especial
      state_expression = 'C';
      colorWipe(cor_anim, 0, state_led); // restaura a cor dos LEDs
    }
    state_expression = 'C';
    led_speech_mills = millis(); // incializa o contador para o efeito dos LEDS
    delay(100); // o modulo de audio do ROS tem uma latencia de ~= 700ms
    bool led_type = false; //variavel booleana usada na alternacia na animação dos LEDs RGB
    while(state_expression == 'C'){
      // animação da boca
      if (random(1, 4) == 1) {
        matrix2_write(m_open);
        delay(random(50, 150));
      } else if (random(1, 4) == 2) {
        matrix2_write(m_mid);
        delay(random(50, 150));
      }
      else if (random(1, 4) == 3) {
        matrix2_write(m_close);
        delay(random(100, 200));
      }
      blinking_eyes();
      processa_serial_port();
      anim_feet(); // move os pés do FRED aleatoreamente
     }
   }
   else if ((command == 't') || (command == 'c')) { // 't' or 'c'
    if (state_expression == 'T') {
      if (state_led == 'k') cor_anim = ring.Color(0, 0, 0); // caso especial
      state_expression = state_exp_aux; // restaura para o estado anterior  
      if (state_expression == 'f') expression_show(fred);
      else if (state_expression == 'i') expression_show(in_love);
      else if (state_expression == 'b') expression_show(broken);
      else if (state_expression == 'n') expression_show(neutral);
      else if (state_expression == 'p') expression_show(pleased); 
      else if (state_expression == 'h') expression_show(happy); 
      else if (state_expression == 's') expression_show(sad); 
      else if (state_expression == 'a') expression_show(angry);
      else if (state_expression == 'A') expression_show(angry2);
      else if (state_expression == 'u') expression_show(surprised2); // ficou definido que após a fala, a exp. de surprised fica com boca "neutral" usando a struct "surprised2"
      else if (state_expression == 'r') expression_show(afraid);
      colorWipe(cor_anim, delay_leds, state_led); // restaura a cor dos LEDs
      servos_attach(); 
    }
    else if (state_expression == 'C') {
      state_expression = state_exp_aux; // restaura para o estado anterior  
      if (state_expression == 'f') expression_show(fred);
      else if (state_expression == 'i') expression_show(in_love);
      else if (state_expression == 'b') expression_show(broken);
      else if (state_expression == 'n') expression_show(neutral);
      else if (state_expression == 'p') expression_show(pleased); 
      else if (state_expression == 'h') expression_show(happy); 
      else if (state_expression == 's') expression_show(sad); 
      else if (state_expression == 'a') expression_show(angry);
      else if (state_expression == 'A') expression_show(angry2);
      else if (state_expression == 'u') expression_show(surprised2); // ficou definido que após a fala, a exp. de surprised fica com boca "neutral" usando a struct "surprised2"
      else if (state_expression == 'r') expression_show(afraid);
      servos_attach();
    }
  }
}


/// pisca olhos do FRED
void blinking_eyes(){
  if (state_expression == 'n' || state_expression == 'p'){ // eye blinking, only in neutral or pleased state
    if (millis() - eye_blinking_mills >= eye_blinking_delay){
      char state_exp_aux = state_expression;
      if (state_exp_aux == 'n'){
        matrix0_write(r_eye_neutral_blink);
        matrix1_write(l_eye_neutral_blink);
        delay(100);
        matrix0_write(r_eye_neutral);
        matrix1_write(l_eye_neutral);
      }
      else if (state_exp_aux == 'p'){
        matrix0_write(r_eye_pleased_blink);
        matrix1_write(l_eye_pleased_blink);
        delay(100);
        matrix0_write(r_eye_pleased);
        matrix1_write(l_eye_pleased);
      }
      eye_blinking_mills = millis(); 
      eye_blinking_delay = (random(1, 7) * 1000);  
    }
  }
}

// controla pequenos movimentos do pé do FRED para dar vida a ele
void anim_feet(){
  int veloc_srv_aux = veloc_srv; // saves veloc value
  veloc_srv = 15;
  if (state_pose == 'i' || state_pose == '?' ){ // foot life sim., only in initial ou undefined position
    if (millis() - foot_mills >= foot_delay){
      char mov = random(1, 4); // 3 variaçoes de poses para simular vida
      if (mov == 1) // pose init
        //pose_init();
        state_pose = 'i';
      else if (mov == 2){ // right foot
        servos_attach();
        foot_right_srv.write(FOOT_RIGHT_EQ + 10, veloc_srv, true);
        foot_right_srv.write(FOOT_RIGHT_EQ, veloc_srv, true);
        //pose_init(); // acerta a posição inicial e desliga os servos
      }
      else if (mov == 3){ // left foot
        servos_attach();
        foot_left_srv.write(FOOT_LEFT_EQ - 10, veloc_srv, true);
        foot_left_srv.write(FOOT_LEFT_EQ, veloc_srv, true);
        //pose_init(); // acerta a posição inicial e desliga os servos
      }
      foot_mills = millis(); 
      foot_delay = (random(3, 7) * 1000);  
    }
  }
  veloc_srv = veloc_srv_aux; // restore veloc value
}



//////////////////////////////////////////////////////////// Funções que processam os comandos recebidos na porta SERIAL ///////////////////
void processa_serial_port(){
  // os comandos são em pares (um char para o comando e outro char para o parâmentro)
  if (Serial.available() >= 2){
    int func = Serial.read(); // lê o comando (primeiro)
    ///////////////////////////////////// comandos para controlar cada servo individualmente ///////////////////////////
    if (func == 'a'){ // foot left
      int angle =Serial.read(); // lê o parâmentro (ângulo)
      //servos_attach();
      leg_left_srv.write(angle, veloc_srv);
      while (leg_left_srv.isMoving()){ delay(1); }
      state_pose = '?';
      //servos_detach();   
    } else
    if (func == 'b'){ // leg left
      int angle =Serial.read(); // lê o parâmentro (ângulo)
      //servos_attach();
      foot_left_srv.write(angle, veloc_srv);
      while (foot_left_srv.isMoving()){ delay(1); }
      state_pose = '?';
      //servos_detach(); 
    } else 
    if (func == 'c'){ // foot right
      int angle =Serial.read(); // lê o parâmentro (ângulo)
      //servos_attach();
      leg_right_srv.write(angle, veloc_srv);
      while (leg_right_srv.isMoving()){ delay(1); }
      state_pose = '?';
      //servos_detach(); 
    } else
    if (func == 'd'){ // leg right
      int angle =Serial.read(); // lê o parâmentro (ângulo)
      //servos_attach();
      foot_right_srv.write(angle, veloc_srv);
      while (foot_right_srv.isMoving()){ delay(1); }
      state_pose = '?';
      //servos_detach();     
    } else

    /////////////////////////////////// comandos para controlar os LEDs RGB do tórax do robô ///////////////////////////
    if (func == 'l'){ // led
      int efx =Serial.read(); // lê o parâmentro (efx)
      if ((state_expression == 'T') || (state_expression == 'C')) delay_leds = 0; // When speeching, leds should change fast!
      if (efx == 'r') colorWipe(ring.Color(255, 0, 0), delay_leds, efx); // Angry Red   
      if (efx == 'b') colorWipe(ring.Color(0, 0, 255), delay_leds, efx); // Sad Blue 
      if (efx == 'g') colorWipe(ring.Color(0, 255, 0), delay_leds, efx); // Green Happy
      if (efx == 'w') colorWipe(ring.Color(255, 255, 255), delay_leds, efx); // White  
      if (efx == 'k') colorWipe(ring.Color(0, 0, 0), delay_leds, efx); // Black (off) 
      if (efx == 'p') colorWipe(ring.Color(255, 0, 255), delay_leds, efx); // Pink
      if (efx == 'y') colorWipe(ring.Color(255, 191, 0), delay_leds, efx); // Yellow

      if (efx == 'n') rainbow(16, efx); // Arco Iris Effect 
    } else

    ///////////////////////////////////// seta a velocidade global dos servomotores ///////////////////////////
    if (func == 'v'){ // define velocity of servomotors
      veloc_srv =Serial.read(); // lê o parâmetro (velocidade)
    } else

    ///////////////////////////////////// comandos para controlar as expressões faciais do robô ///////////////////////////
    if (func == 'e'){
      char param =Serial.read();
      if (param == 'f') {
        expression_show(fred); // Expression Fred and Heart (Greetings)
        state_exp_aux = 'f';
      }
      else if (param == 'i') {
        expression_show(in_love); // Expression In love
        state_exp_aux = 'i';
      }
      else if (param == 'b') {
        expression_show(broken); // Expression Broken
        state_exp_aux = 'b';
      }
      else if (param == 'n') {
        expression_show(neutral); // Expression Neutral
        state_exp_aux = 'n';
      }
      else if (param == 'p') {
        expression_show(pleased); // Expression Pleased
        state_exp_aux = 'p';
      }
      else if (param == 'h') {
        expression_show(happy); // Expression Happy
        state_exp_aux = 'h';
      }
      else if (param == 's') {
        expression_show(sad); // Expression Sad
        state_exp_aux = 's';
      }
      else if (param == 'a') {
        expression_show(angry); // Expression Angry
        state_exp_aux = 'a';
      }
      else if (param == 'A') {
        expression_show(angry2); // Expression Angry2
        state_exp_aux = 'A';
      }
      else if (param == 'u') {
        expression_show(surprised); // Expression Surprised
        state_exp_aux = 'u';
      }
      else if (param == 'r') {
        expression_show(afraid); // Expression Afraid
        state_exp_aux = 'r';
      }
      else if (param == 't') {
        speech_anim(param); // Speech mouth animation stop with led animatiom
        state_exp_aux = 't';
      }
      else if (param == 'T') {
        speech_anim(param); // Speech mouth animation start with led animatiom
        state_exp_aux = 'T';
      }
      else if (param == 'c') {
        speech_anim(param); // Speech mouth animation stop  without led animatiom
        state_exp_aux = 'c';
      }
      else if (param == 'C') {
        speech_anim(param); // Speech mouth animation start  without led animatiom
        state_exp_aux = 'C';
      }
    } else

    ///////////////////////////////////// comandos para controlar os MOVIMENTOS do robô ///////////////////////////
    if (func == 'm'){
      char param =Serial.read(); // lê o parâmetro (tipo) do movimento
      if (param == 'f') move_forward(2); // Walking forward (2 cycles)
      if (param == 'F') move_forward(4); // Walking forward (4 cycles)
      if (param == 'b') move_backward(2); // Walking backward (2 cycles)
      if (param == 'B') move_backward(4); // Walking backward (4 cycles)
      if (param == 'l') move_left(2); // Move left (2 cycles)
      if (param == 'L') move_left_moon(1); // Move left (4 cycles)
      if (param == 'r') move_right(2); // Move right (2 cycles)
      if (param == 'R') move_right_moon(1); // Move right (4 cycles)
      if (param == 'w') move_moon_walk(1); // Moon walker (2 cycles)
      if (param == 'W') move_moon_walk(2); // Moon walker (4 cycles)
      if (param == 'd') move_dance1(2); // Dance 1 (2 cycles)
      if (param == 'D') move_dance1(4); // Dance 1 (4 cycles)
      if (param == 'e') move_dance2(2); // Dance 2 (2 cycles)
      if (param == 'E') move_dance2(4); // Dance 2 (4 cycles)
      if (param == 's') move_stomping_foot('s'); // stomping right foot (4 cycles)
      if (param == 'S') move_stomping_foot('S'); // stomping left foot (4 cycles)
    } else


    ///////////////////////////////////// comandos para controlar as POSES do robô ///////////////////////////
    if (func == 'p'){
      char param =Serial.read(); // lê o parâmetro (tipo da pose)
      if (param == 'i'){
        pose_init('1'); // Pose init (equilíbrio)
      }
      if (param == 'u'){
        pose_up('u'); // Pose up
      }
      if (param == 'l'){
        pose_up('l'); // Pose up (left foot1)
      }
      if (param == 'L'){
        pose_up('L'); // Pose up (left foot2)
      }
      if (param == 'r'){
        pose_up('r'); // Pose up (right foot1)
      }
      if (param == 'R'){
        pose_up('R'); // Pose up (right foot2)
      }
      if (param == 'd'){
        pose_down(param); // Pose down (broken) slow
      }
      if (param == 'D'){
        pose_down(param); // Pose down (broken) fast
      }  
    } else

    ///////////////////////////////////// Envia os valores das variáveis do estado interno do FRED para a porta SERIAL ////////////////////
    if (func == 's'){
      char param =Serial.read();
      if (param == 't') show_status();
    }
  }
}

void sense_touch(){
  if (digitalRead(TOUCH_HEAD_PIN) == 1)
    TOUCH_HEAD_VALUE = TOUCH_HEAD_VALUE + 1;
      delay(200);
      
  if (digitalRead(TOUCH_LEFT_PIN) == 1)
    TOUCH_LEFT_VALUE = TOUCH_LEFT_VALUE + 1;
      delay(100);

  if (digitalRead(TOUCH_RIGHT_PIN) == 1)
    TOUCH_RIGHT_VALUE = TOUCH_RIGHT_VALUE + 1;
      delay(100);

  if (TOUCH_HEAD_VALUE >=5){
    expression_show(in_love); // Expression In love
    pose_down('i'); // Pose down (broken) slow
    rainbow(16, 'n'); // Arco Iris Effect 
    delay(2000);
    expression_show(neutral); // Expression Neutral
    pose_init('1'); // Pose init (equilíbrio)
    colorWipe(ring.Color(0, 0, 0), delay_leds, 'k'); // Black (off) 
    TOUCH_HEAD_VALUE = 0;
  }

  if (TOUCH_LEFT_VALUE >=2){
    expression_show(happy); // Expression Happy
    pose_up('L'); // Pose up (left foot2)
    colorWipe(ring.Color(0, 255, 0), delay_leds, 'g'); // Green Happy
    delay(3000);
    expression_show(neutral); // Expression Neutral
    pose_init('1'); // Pose init (equilíbrio)
    colorWipe(ring.Color(0, 0, 0), delay_leds, 'k'); // Black (off) 
    TOUCH_LEFT_VALUE = 0;
  }

  if (TOUCH_RIGHT_VALUE >=2){
    expression_show(happy); // Expression Happy
    pose_up('R'); // Pose up (right foot2)
    colorWipe(ring.Color(0, 255, 0), delay_leds, 'g'); // Green Happy
    delay(3000);
    expression_show(neutral); // Expression Neutral
    pose_init('1'); // Pose init (equilíbrio)
    colorWipe(ring.Color(0, 0, 0), delay_leds, 'k'); // Black (off) 
    TOUCH_RIGHT_VALUE = 0;
  }
}
