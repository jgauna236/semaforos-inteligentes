//Librería para FreeRtos
#include <Arduino_FreeRTOS.h>
//funciones de FreeRTOS para semáforos
#include <semphr.h>
#include "SemaphoreConst.h"

//Defino semáforo para el serial
SemaphoreHandle_t serial_sem;

//Defino las tareas que se van a utilizar
void configPines();
void set_states(void *pvParameters);
void read_inputs(void *pvParameters);

int count_cars[2] = {0,0};
int inputs_now[6] = {0,0,0,0,0,0};
int inputs_prev[6] = {0,0,0,0,0,0};
int PA_flag = 0;
int PB_flag = 0;
int half_seconds = TIME_MIN;
int state= SEMAPHORE_A_ON;
int debug = 0;

void setup(){
  Serial.begin(9600);
  configPines();
  if(serial_sem == NULL){ vSemaphoreCreateBinary(serial_sem); }
  xTaskCreate(read_inputs,(const portCHAR *)"READ INPUTS", 128, NULL, 1, NULL);
  xTaskCreate(set_states,(const portCHAR *)"SET STATES", 128, NULL, 1, NULL);
}

void loop(){}

void configPines(){
  //Salidas
  //Semáforo Autos Calle A
  pinMode(SEMAPHORE_A_LED_GREEN, OUTPUT); //Verde
  pinMode(SEMAPHORE_A_LED_YELLOW, OUTPUT); //Amarrillo
  pinMode(SEMAPHORE_A_LED_RED, OUTPUT); //Rojo
  //Semáforo Autos Calle B
  pinMode(SEMAPHORE_B_LED_GREEN, OUTPUT); //Verde
  pinMode(SEMAPHORE_B_LED_YELLOW, OUTPUT); //Amarrillo
  pinMode(SEMAPHORE_B_LED_RED, OUTPUT); //Rojo
  //Semáforo Peaton Calle A
  pinMode(PEDESTRIAN_A_LED_BLUE, OUTPUT); //Azul
  pinMode(PEDESTRIAN_A_LED_RED, OUTPUT); //Rojo
  //Semáforo Peaton Calle B
  pinMode(PEDESTRIAN_B_LED_BLUE, OUTPUT); //Azul
  pinMode(PEDESTRIAN_B_LED_RED, OUTPUT); //Rojo

  //Entradas
  //Calle A
  pinMode(STREET_A_START, INPUT); //Inicio
  pinMode(STREET_A_FINISH, INPUT); //Final
  //Calle B
  pinMode(STREET_B_START, INPUT); //Inicio
  pinMode(STREET_B_FINISH, INPUT); //Final
  //Interruptores
  pinMode(BUTTON_PEDESTRIAN_A, INPUT); //Semáforo Peaton Calle A
  pinMode(BUTTON_PEDESTRIAN_B, INPUT); //Semáforo Peaton Calle B
}

void read_inputs(void *pvParameters){
  while(1){
    inputs_prev[SENSOR_A_START]   = inputs_now[SENSOR_A_START];
    inputs_prev[SENSOR_A_FINISH]  = inputs_now[SENSOR_A_FINISH];
    inputs_prev[SENSOR_B_START]   = inputs_now[SENSOR_B_START];
    inputs_prev[SENSOR_B_FINISH]  = inputs_now[SENSOR_B_FINISH];
    inputs_prev[BUTTON_A]         = inputs_now[BUTTON_A];
    inputs_prev[BUTTON_B]         = inputs_now[BUTTON_B];

    inputs_now[SENSOR_A_START]  = digitalRead(STREET_A_START);
    inputs_now[SENSOR_A_FINISH] = digitalRead(STREET_A_FINISH);
    inputs_now[SENSOR_B_START]  = digitalRead(STREET_B_START);
    inputs_now[SENSOR_B_FINISH] = digitalRead(STREET_B_FINISH);
    inputs_now[BUTTON_A]        = digitalRead(BUTTON_PEDESTRIAN_A);
    inputs_now[BUTTON_B]        = digitalRead(BUTTON_PEDESTRIAN_B);

    if(inputs_prev[SENSOR_A_START] < inputs_now[SENSOR_A_START]){
      count_cars[STREET_A]++;
      xSemaphoreTake(serial_sem, 1 );
      Serial.print("Autos en la calle A: ");
      Serial.println(count_cars[STREET_A]);
      xSemaphoreGive(serial_sem);
    }
    if(inputs_prev[SENSOR_A_FINISH] < inputs_now[SENSOR_A_FINISH]){
      count_cars[STREET_A]--;
      if(count_cars[STREET_A] < 0){ count_cars[STREET_A] = 0; }
      xSemaphoreTake(serial_sem, 1 );
      Serial.print("Autos en la calle A: ");
      Serial.println(count_cars[STREET_A]);
      xSemaphoreGive(serial_sem);
    }
    if(inputs_prev[SENSOR_B_START] < inputs_now[SENSOR_B_START]){
      count_cars[STREET_B]++;
      xSemaphoreTake(serial_sem, 1 );
      Serial.print("Autos en la calle B: ");
      Serial.println(count_cars[STREET_B]);
      xSemaphoreGive(serial_sem);
    }
    if(inputs_prev[SENSOR_B_FINISH] < inputs_now[SENSOR_B_FINISH]){
      count_cars[STREET_B]--;
      if(count_cars[STREET_B] < 0){ count_cars[STREET_B] = 0; }
      xSemaphoreTake(serial_sem, 1 );
      Serial.print("Autos en la calle B: ");
      Serial.println(count_cars[STREET_B]);
      xSemaphoreGive(serial_sem);
    }
    if(inputs_prev[BUTTON_A] < inputs_now[BUTTON_A] &&
      state != SEMAPHORE_B_ON &&
      state != SEMAPHORE_B_CHANGE &&
      state != SEMAPHORE_A_OFF){
      xSemaphoreTake(serial_sem, 1 );
      Serial.println("Boton A presionado");
      xSemaphoreGive(serial_sem);
      PA_flag = 1;
    }
    if(inputs_prev[BUTTON_B] < inputs_now[BUTTON_B] &&
      state != SEMAPHORE_A_ON &&
      state != SEMAPHORE_A_CHANGE &&
      state != SEMAPHORE_B_OFF){
      xSemaphoreTake(serial_sem, 1 );
      Serial.println("Boton B presionado");
      xSemaphoreGive(serial_sem);
      PB_flag = 1;
    }
    
    vTaskDelay(5);
  }
}

void set_states(void *pvParameters){
  while(1){
    half_seconds++;
    if(half_seconds < TIME_MIN){
      switch (state) {
        case SEMAPHORE_A_CHANGE:
          if(half_seconds > TIME_WAIT){
            half_seconds = 0;
            state = SEMAPHORE_A_OFF;
            xSemaphoreTake(serial_sem, 1 );
            Serial.println("STATUS: Todos los semaforos en rojo.");
            xSemaphoreGive(serial_sem);
          }
        break;
        case SEMAPHORE_B_CHANGE:
          if(half_seconds > TIME_WAIT){
            half_seconds = 0;
            state = SEMAPHORE_B_OFF;
            xSemaphoreTake(serial_sem, 1 );
            Serial.println("STATUS: Todos los semaforos en rojo.");
            xSemaphoreGive(serial_sem);
          }
        break;
        case SEMAPHORE_A_OFF:
          if(half_seconds > TIME_OFF){
            half_seconds = 0;
            state = SEMAPHORE_B_ON;
            xSemaphoreTake(serial_sem, 1 );
            Serial.println("STATUS: Semaforo de la Calle B en Verde");
            xSemaphoreGive(serial_sem);
          }
        break;
        case SEMAPHORE_B_OFF:
        if(half_seconds > TIME_OFF){
          half_seconds = 0;
          state = SEMAPHORE_A_ON;
          xSemaphoreTake(serial_sem, 1 );
          Serial.println("STATUS: Semaforo de la Calle A en Verde");
          xSemaphoreGive(serial_sem);
        }
        break;
      }
    } else if(half_seconds > TIME_MAX) {
      switch (state) {
        case SEMAPHORE_A_ON:
          if(count_cars[STREET_B] > 0 || PA_flag == 1){
            state = SEMAPHORE_A_CHANGE;
            PA_flag = 0;
            half_seconds = 0;
            xSemaphoreTake(serial_sem, 1 );
            Serial.println("STATUS: Calle A en Amarillo");
            xSemaphoreGive(serial_sem);
          }
        break;
        case SEMAPHORE_B_ON:
          if(count_cars[STREET_A] > 0 || PB_flag == 1){
            state = SEMAPHORE_B_CHANGE;
            PB_flag = 0;
            half_seconds = 0;
            xSemaphoreTake(serial_sem, 1 );
            Serial.println("STATUS: Calle B en Amarillo");
            xSemaphoreGive(serial_sem);
          }
        break;
      }
    } else {
      switch (state) {
        case SEMAPHORE_A_ON:
          if(count_cars[STREET_B] > count_cars[STREET_A] || PA_flag == 1){
            state = SEMAPHORE_A_CHANGE;
            PA_flag = 0;
            half_seconds = 0;
            xSemaphoreTake(serial_sem, 1 );
            Serial.println("STATUS: Calle A en Amarillo");
            xSemaphoreGive(serial_sem);
          }
        break;
        case SEMAPHORE_B_ON:
          if(count_cars[STREET_A] > count_cars[STREET_A] || PB_flag == 1){
            state = SEMAPHORE_B_CHANGE;
            PB_flag = 0;
            half_seconds = 0;
            xSemaphoreTake(serial_sem, 1 );
            Serial.println("STATUS: Calle A en Amarillo");
            xSemaphoreGive(serial_sem);
          }
        break;
      }
    }
    set_leds();
    vTaskDelay(40);
  }
}

void set_leds(){
  //Muestro los led según el estado
  switch (state) {
    case SEMAPHORE_A_ON:
      digitalWrite(SEMAPHORE_A_LED_GREEN, HIGH);
      digitalWrite(SEMAPHORE_A_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_A_LED_RED, LOW);
      digitalWrite(SEMAPHORE_B_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_B_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_B_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_A_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_A_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_B_LED_BLUE, HIGH);
      digitalWrite(PEDESTRIAN_B_LED_RED, LOW);
    break;
    case SEMAPHORE_A_CHANGE:
      digitalWrite(SEMAPHORE_A_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_A_LED_YELLOW, HIGH);
      digitalWrite(SEMAPHORE_A_LED_RED, LOW);
      digitalWrite(SEMAPHORE_B_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_B_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_B_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_A_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_A_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_B_LED_BLUE, !digitalRead(PEDESTRIAN_B_LED_BLUE));
      digitalWrite(PEDESTRIAN_B_LED_RED, LOW);
    break;
    case SEMAPHORE_A_OFF:
      digitalWrite(SEMAPHORE_A_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_A_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_A_LED_RED, HIGH);
      digitalWrite(SEMAPHORE_B_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_B_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_B_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_A_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_A_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_B_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_B_LED_RED, HIGH);
    break;
    case SEMAPHORE_B_ON:
      digitalWrite(SEMAPHORE_A_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_A_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_A_LED_RED, HIGH);
      digitalWrite(SEMAPHORE_B_LED_GREEN, HIGH);
      digitalWrite(SEMAPHORE_B_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_B_LED_RED, LOW);
      digitalWrite(PEDESTRIAN_A_LED_BLUE, HIGH);
      digitalWrite(PEDESTRIAN_A_LED_RED, LOW);
      digitalWrite(PEDESTRIAN_B_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_B_LED_RED, HIGH);
    break;
    case SEMAPHORE_B_CHANGE:
      digitalWrite(SEMAPHORE_A_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_A_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_A_LED_RED, HIGH);
      digitalWrite(SEMAPHORE_B_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_B_LED_YELLOW, HIGH);
      digitalWrite(SEMAPHORE_B_LED_RED, LOW);
      digitalWrite(PEDESTRIAN_A_LED_BLUE, !digitalRead(PEDESTRIAN_A_LED_BLUE));
      digitalWrite(PEDESTRIAN_A_LED_RED, LOW);
      digitalWrite(PEDESTRIAN_B_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_B_LED_RED, HIGH);      
    break;
    case SEMAPHORE_B_OFF:
      digitalWrite(SEMAPHORE_A_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_A_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_A_LED_RED, HIGH);
      digitalWrite(SEMAPHORE_B_LED_GREEN, LOW);
      digitalWrite(SEMAPHORE_B_LED_YELLOW, LOW);
      digitalWrite(SEMAPHORE_B_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_A_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_A_LED_RED, HIGH);
      digitalWrite(PEDESTRIAN_B_LED_BLUE, LOW);
      digitalWrite(PEDESTRIAN_B_LED_RED, HIGH);
    break;
  }
}
