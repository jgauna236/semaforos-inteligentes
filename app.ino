#define LEDAMARILLO1 2
#define LEDROJO1 3
#define LEDAMARILLO2 4
#define LEDROJO2 5


    //Tiempo que se tarda de rojo a verde 
    long tiempoCambio=2000; 


    //booleano para coarroborar que semaforo esta encendido
    boolean activo = true;


void setup()
{


 
    // Modo entrada/salida de los pines
    pinMode(LEDAMARILLO1, OUTPUT);
    pinMode(LEDROJO1, OUTPUT);
    pinMode(LEDAMARILLO2, OUTPUT);
    pinMode(LEDROJO2, OUTPUT);
//    pinMode(PULSADOR1, INPUT);
//    pinMode(PULSADOR2, INPUT);
    
    // Apagamos todos los LEDs
    digitalWrite(LEDAMARILLO1, LOW);
    digitalWrite(LEDROJO1, LOW);
    digitalWrite(LEDAMARILLO2, LOW);
    digitalWrite(LEDROJO2, LOW);
    
    // Estado inicial: semáforo 1 activo, semáforo 2 no activo
    digitalWrite(LEDROJO2, HIGH);




}


void cambiarSemaforos(){
    if (activo){
        digitalWrite(LEDAMARILLO2, HIGH);
        digitalWrite(LEDROJO2, LOW);
        delayMicroseconds(tiempoCambio);
        digitalWrite(LEDAMARILLO2, LOW);
        delayMicroseconds(tiempoCambio);
        digitalWrite(LEDROJO1, HIGH);

        activo = false;
    }else{
        digitalWrite(LEDAMARILLO1, HIGH);
        digitalWrite(LEDROJO1, LOW);
        delayMicroseconds(tiempoCambio);
        digitalWrite(LEDAMARILLO1, LOW);
        delayMicroseconds(tiempoCambio);
        digitalWrite(LEDROJO2, HIGH);
      
      	activo = true;
    }


}


void loop()
{

    cambiarSemaforos();
}





