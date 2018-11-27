#include <avr/io.h>
#include <avr/interrupt.h>

#define SA 0
#define SB 1

#define OUT_SAG A0
#define OUT_SAY A1
#define OUT_SAR A2
#define OUT_SBG A3
#define OUT_SBY A4
#define OUT_SBR A5
#define OUT_PAG 8
#define OUT_PAR 9
#define OUT_PBG 10
#define OUT_PBR 11

#define IN_PA 2
#define IN_PB 3

#define IN_A_start  4
#define IN_A_finish 5
#define IN_B_start  6
#define IN_B_finish 7

#define T_MIN 3
#define T_MAX 10

const int tiempoAntirrebote = 10;
int count_cars[2]={0,0};
int inputs_now[6]={0,0,0,0,0,0};
int inputs_prev[6]={0,0,0,0,0,0};
int timer_flag=0;
int count_flag=0;
int PA_flag=0;
int PB_flag=0;
int seconds=0;
int estado=0;

void setup(){
  
  //Serial
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Inicio Semaforos Inteligentes");
  
  //Salidas
  //Semáforo Autos Calle A
  pinMode(OUT_SAG, OUTPUT); //Verde
  pinMode(OUT_SAY, OUTPUT); //Amarrillo
  pinMode(OUT_SAR, OUTPUT); //Rojo
  //Semáforo Autos Calle B
  pinMode(OUT_SBG, OUTPUT); //Verde
  pinMode(OUT_SBY, OUTPUT); //Amarrillo
  pinMode(OUT_SBR, OUTPUT); //Rojo
  //Semáforo Peaton Calle A
  pinMode(OUT_PAG, OUTPUT); //Azul
  pinMode(OUT_PAR, OUTPUT); //Rojo
  //Semáforo Peaton Calle B
  pinMode(OUT_PBG, OUTPUT); //Azul
  pinMode(OUT_PBR, OUTPUT); //Rojo

  //Entradas
  //Calle A
  pinMode(IN_A_start, INPUT); //Inicio
  pinMode(IN_A_finish, INPUT); //Final
  //Calle B
  pinMode(IN_B_start, INPUT); //Inicio
  pinMode(IN_B_finish, INPUT); //Final
  //Interruptores
  pinMode(IN_PA, INPUT); //Semáforo Peaton Calle A
  pinMode(IN_PB, INPUT); //Semáforo Peaton Calle B

  //Configurar interrupcion del Timer1 cada 250ms
  cli();
  TCCR1A=0;
  TCCR1B=0;
  OCR1A=3906;
  TCCR1B |= (1<<WGM12);
  TCCR1B |= (1<<CS10);
  TCCR1B |= (1<<CS12);
  TIMSK1=(1<<OCIE1A);
  sei();
}

void loop(){
  if(timer_flag){
    timer_flag = 0;
    count_flag++;
    set_leds();
  }
  if(count_flag > 3){
  	count_flag = 0;
    seconds++;
  }
  listen_inputs();
  set_estado();
}

 boolean antirrebote(int pin)
  {
    int contador = 0;
    boolean restado;
    boolean estadoAnterior;
   
    do
    {
      restado = digitalRead(pin);
      if(restado != estadoAnterior)
      {
        contador = 0;
        estadoAnterior = estado;
      }
      else
      {
        contador = contador+ 1;
      }
      delay(1);
    } while(contador < tiempoAntirrebote);
   
    return restado;
  }
void set_leds(){
  switch (estado) {
    case 0:
      digitalWrite(OUT_SAG, HIGH);
      digitalWrite(OUT_SAY, LOW);
      digitalWrite(OUT_SAR, LOW);
      digitalWrite(OUT_SBG, LOW);
      digitalWrite(OUT_SBY, LOW);
      digitalWrite(OUT_SBR, HIGH);
      digitalWrite(OUT_PAG, LOW);
      digitalWrite(OUT_PAR, HIGH);
      digitalWrite(OUT_PBG, HIGH);
      digitalWrite(OUT_PBR, LOW);
    break;
    case 1:
      digitalWrite(OUT_SAG, LOW);
      digitalWrite(OUT_SAY, HIGH);
      digitalWrite(OUT_SAR, LOW);
      digitalWrite(OUT_SBG, LOW);
      digitalWrite(OUT_SBY, LOW);
      digitalWrite(OUT_SBR, HIGH);
      digitalWrite(OUT_PAG, LOW);
      digitalWrite(OUT_PAR, HIGH);
      digitalWrite(OUT_PBG, LOW);
      digitalWrite(OUT_PBR, !digitalRead(OUT_PBR));
    break;
    case 2:
      digitalWrite(OUT_SAG, LOW);
      digitalWrite(OUT_SAY, LOW);
      digitalWrite(OUT_SAR, HIGH);
      digitalWrite(OUT_SBG, LOW);
      digitalWrite(OUT_SBY, LOW);
      digitalWrite(OUT_SBR, HIGH);
      digitalWrite(OUT_PAG, LOW);
      digitalWrite(OUT_PAR, HIGH);
      digitalWrite(OUT_PBG, LOW);
      digitalWrite(OUT_PBR, HIGH);
    break;
    case 3:
      digitalWrite(OUT_SAG, LOW);
      digitalWrite(OUT_SAY, LOW);
      digitalWrite(OUT_SAR, HIGH);
      digitalWrite(OUT_SBG, HIGH);
      digitalWrite(OUT_SBY, LOW);
      digitalWrite(OUT_SBR, LOW);
      digitalWrite(OUT_PAG, HIGH);
      digitalWrite(OUT_PAR, LOW);
      digitalWrite(OUT_PBG, LOW);
      digitalWrite(OUT_PBR, HIGH);
    break;
    case 4:
      digitalWrite(OUT_SAG, LOW);
      digitalWrite(OUT_SAY, LOW);
      digitalWrite(OUT_SAR, HIGH);
      digitalWrite(OUT_SBG, LOW);
      digitalWrite(OUT_SBY, HIGH);
      digitalWrite(OUT_SBR, LOW);
      digitalWrite(OUT_PAG, LOW);
      digitalWrite(OUT_PAR, !digitalRead(OUT_PAR));
      digitalWrite(OUT_PBG, LOW);
      digitalWrite(OUT_PBR, HIGH);
    break;
    case 5:
      digitalWrite(OUT_SAG, LOW);
      digitalWrite(OUT_SAY, LOW);
      digitalWrite(OUT_SAR, HIGH);
      digitalWrite(OUT_SBG, LOW);
      digitalWrite(OUT_SBY, LOW);
      digitalWrite(OUT_SBR, HIGH);
      digitalWrite(OUT_PAG, LOW);
      digitalWrite(OUT_PAR, HIGH);
      digitalWrite(OUT_PBG, LOW);
      digitalWrite(OUT_PBR, HIGH);
    break;
  }
}

void set_estado(){
  if(seconds > T_MIN){
    switch (estado) {
      case 0:
        if(((count_cars[SB] - count_cars[SA])>2)|| PA_flag == 1){
          estado++;
          PA_flag = 0;
          seconds = 0;
        }
      break;
      case 3:
        if(((count_cars[SA] - count_cars[SB])>2) || PB_flag == 1){
          estado ++;
          PB_flag = 0;
          seconds = 0;
        }
        break;
      case 1:
        estado++;
        seconds=0;
        break;
      case 2:
        estado++;
        seconds=0;
        break;
      case 4:
        estado++;
        seconds=0;
        break;
      case 5:
        estado=0;
        seconds=0;
        break;
    }
  }
  if(seconds > T_MAX) {
    seconds = 0;
    if (estado != 5){
      estado++;
    }else {
      estado=0;
    }
  } 
}

void listen_inputs(){
  inputs_prev[0] = inputs_now[0];
  inputs_prev[1] = inputs_now[1];
  inputs_prev[2] = inputs_now[2];
  inputs_prev[3] = inputs_now[3];
  inputs_prev[4] = inputs_now[4];
  inputs_prev[5] = inputs_now[5];

  inputs_now[0] = digitalRead(IN_A_start);
  inputs_now[1] = digitalRead(IN_A_finish);
  inputs_now[2] = digitalRead(IN_B_start);
  inputs_now[3] = digitalRead(IN_B_finish);
  inputs_now[4] = digitalRead(IN_PA);
  inputs_now[5] = digitalRead(IN_PB);


  if(inputs_prev[0] > inputs_now[0]){
    if(antirrebote(IN_A_start)){
      count_cars[SA]++;
      Serial.print("Vehículos en calle A: ");
      Serial.println(count_cars[SA]);
    }
  }
  if(inputs_prev[1] > inputs_now[1]){
    if(antirrebote(IN_A_finish)){
      count_cars[SA]--;
      if(count_cars[SA] < 0){count_cars[SA] = 0;}
      Serial.print("Vehículos en calle A: ");
      Serial.println(count_cars[SA]);
     }
  }
  if(inputs_prev[2] > inputs_now[2]){
    if(antirrebote(IN_B_start)){
      count_cars[SB]++;
      Serial.print("Vehículos en calle B: ");
      Serial.println(count_cars[SB]);
    } 
  }
  if(inputs_prev[3] > inputs_now[3]){
    if(antirrebote(IN_B_finish)){
      count_cars[SB]--;
      if(count_cars[SB] < 0){count_cars[SB] = 0;}
      Serial.print("Vehículos en calle B: ");
      Serial.println(count_cars[SB]);
   }  
  }
  if(inputs_prev[4] != inputs_now[4]){
    if(antirrebote(IN_PA)){
      Serial.println("Peaton presiono boton en en calle A");
      if(estado != 3 && estado != 4){PA_flag = 1;}
   }  
  } 
  if(inputs_prev[5] != inputs_now[5]){
    if(antirrebote(IN_PB)){
      Serial.println("Peaton presiono boton en en calle B");
      if(estado != 0 && estado != 1){PB_flag = 1;}
    }  
  }
}

ISR(TIMER1_COMPA_vect){
  timer_flag = 1;
}
