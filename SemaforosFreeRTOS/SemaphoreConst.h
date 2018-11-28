#ifndef SEMAPHORE_CONST_FILE
  #define SEMAPHORE_CONST_FILE

  //Indice Array para contar autos
  #define STREET_A 0
  #define STREET_B 1
  //Pines Semáforos
  #define SEMAPHORE_A_LED_GREEN   A0
  #define SEMAPHORE_A_LED_YELLOW  A1
  #define SEMAPHORE_A_LED_RED     A2
  #define SEMAPHORE_B_LED_GREEN   A3
  #define SEMAPHORE_B_LED_YELLOW  A4
  #define SEMAPHORE_B_LED_RED     A5
  #define PEDESTRIAN_A_LED_BLUE   8
  #define PEDESTRIAN_A_LED_RED    9
  #define PEDESTRIAN_B_LED_BLUE   10
  #define PEDESTRIAN_B_LED_RED    11
  //Indice Array control de flancos de los sensores y botones
  #define SENSOR_A_START  0
  #define SENSOR_A_FINISH 1
  #define SENSOR_B_START  2
  #define SENSOR_B_FINISH 3
  #define BUTTON_A 4
  #define BUTTON_B 5
  //Pines Sensores
  #define STREET_A_START  4
  #define STREET_A_FINISH 5
  #define STREET_B_START  6
  #define STREET_B_FINISH 7
  //Pines Botones
  #define BUTTON_PEDESTRIAN_A 2
  #define BUTTON_PEDESTRIAN_B 3

  //ESTADOS
  #define SEMAPHORE_A_ON      0
  #define SEMAPHORE_A_CHANGE  1
  #define SEMAPHORE_A_OFF     2
  #define SEMAPHORE_B_ON      3
  #define SEMAPHORE_B_CHANGE  4
  #define SEMAPHORE_B_OFF     5

  //Tiempoes límite
  #define TIME_MIN  20 //Tiempo mínimo que debe estar en verde
  #define TIME_MAX  40 //Tiempo máximo que puede estar en verde(*) excepto que en el otro carril no haya alguien
  #define TIME_WAIT 8  //Tiempo de la espera en amarrillo
  #define TIME_OFF  3  //Tiempo con los dos semaforos bloqueados

  //(*) excepto que en el otro carril no haya alguien
#endif
