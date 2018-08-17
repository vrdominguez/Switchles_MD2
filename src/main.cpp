/** ***************************************************************************
 * Control de cambio de region (Europa, USA, Japon), o de frecuencia (50/60Hz)
 * de la Mega Drive 2, utilizando unicamente el boton de reset de la misma
 * consola. Segun el tiempo que se pulsa el boton:
 *    - Menos de 1 segundo: Reset
 *    - Entre 1 y 2 segundos: Cambio entre 50Hz y 60Hz
 *    - Mas de 2 segundos: Cambio ciclico de Region: EUR->USA->JAP->EUR
 * ************************************************************************* */
#include <Arduino.h>
#include "LowPower.h"

// --- CONFIGURACION DE PINES CONECTADOS A LA CONSOLA -------------------------
int resetInput =  2; // Controla la interrupcion del boton reset
int pinJAP     = 12; // Controla el idioma de la consola
int pinNTSC    = 13; // Controla la frecuencia de refresco
int pinReset   =  8; // Controla el reseteo

// --- SALIDAS PARA CONTROLAR LA ILUMINACION DEL LED --------------------------
int rled = 11; // Pin rojo de led RGB
int gled = 10; // Pin verde de led RGB
int bled = 9;  // Pin azul de led RGB

// --- CONTROL DEL BUZZER -----------------------------------------------------
int pinBuzzer = 3; // Pin para buzzer (debe ser uno de los marcados como ~)

// --- COLORES LED ------------------------------------------------------------
int jap[3] = {255,000,  0}; // Rojo para Japon
int usa[3] = {  0,255,  0}; // Verde para USA
int eur[3] = {  0,  0,255}; // Azul para Europa
int cus[3] = {255,  0,255}; // Magenta para "personalizada"

// --- OTRAS VARIABLES --------------------------------------------------------
String region = "EUR"; // Guarda la region actual

// --- FUNCIONES --------------------------------------------------------------
/**
 * Establece el color del led rgb
 *
 * @param int[] array con los valores de RGB (3 valores, entre 0 y 255)
 * @return void
 */
void colorearLed ( int rgb[3] ) {
  analogWrite(rled,rgb[0]);
  analogWrite(gled,rgb[1]);
  analogWrite(bled,rgb[2]);
}

/**
 * Apaga el led RGB
 *
 * @param ---
 * @return void
*/
void apagarLed() {
  analogWrite(rled,0);
  analogWrite(gled,0);
  analogWrite(bled,0);
}

/**
 * Hace parpadear el led RGB, en el color, frecuencia y repeticiones indicadas
 *
 * @param int[] color con los valores de RGB (3 valores, entre 0 y 255)
 * @param int tiempo milisegundos de encendido/apagado
 * @param int repeticiones numero de ciclos de apagado y encendido
 * @return void
 */
void parpadearLED(int color[3], int tiempo=50, int repeticiones=5) {
  for( int i = 0; i < repeticiones; i++) {
    apagarLed();
    delay(tiempo);
    colorearLed(color);
    delay(tiempo);
  }
}

/**
 * Establece la region como Europa (no japones, 50Hz) y pone el led en azul
 *
 * @param ---
 * @return void
 */
void setEUR() {
  digitalWrite(pinJAP, HIGH);
  digitalWrite(pinNTSC, LOW);

  parpadearLED(eur,125,2);

  region = "EUR";
}

/**
 * Establece la region como USA (no japones, 60Hz) y pone el led en verde
 *
 * @param ---
 * @return void
 */
void setUSA() {
  digitalWrite(pinJAP, HIGH);
  digitalWrite(pinNTSC, HIGH);
  
  parpadearLED(usa);

  region = "USA";
}

/**
 * Establece la region como Japon (japones, 60Hz) y pone el led en rojo
 *
 * @param ---
 * @return void
 */
void setJAP() {
  digitalWrite(pinJAP, LOW);
  digitalWrite(pinNTSC, HIGH);
  
  parpadearLED(jap);

  region = "JAP";
}

/**
 * Establece la region como "custom" (japones, 50Hz) y pone el led en morado.
 * Solamente se puede llegar a esta combinacion haciedo un cambio de frecuencia
 * desde la region "Japon", en lugar de cambiar de region. No es una de las
 * regiones existentes en la consola
 *
 * @param ---
 * @return void
 */
void setCustom() {
  digitalWrite(pinJAP, LOW);
  digitalWrite(pinNTSC, LOW);
  
  parpadearLED(cus,125,2);

  region = "CUS";
}

/**
 * Configura la region de la consola en una de las 3 existentes (Europera, USA
 * o Japon) de forma ciclica: EUR -> USA -> JAPON -> EUR
 *
 * @param ---
 * @return void
 */
void cambiarRegion() {
  if ( region.compareTo("EUR") == 0 ) {
    setUSA();
  }
  else if ( region.compareTo("USA") == 0 )
  {
    setJAP();
  }
  else {
    setEUR();
  }
}

/**
 * Varia la fecuencia de refresco (50 - 60Hz) sin variar la zona del idioma, 
 * de forma que, si se corresponde con otra de las 3 regiones, cambia a una de
 * ellas o, en otro caso, activa la region "custom" (japones, 50Hz)
 *
 * @param ---
 * @return void
 */
void cambiarFrecuencia() {
  if ( region.compareTo("EUR") == 0 ) {
    setUSA();
  }
  else if ( region.compareTo("USA") == 0 )
  {
    setEUR();
  }
  else if( region.compareTo("JAP") == 0 ) {
    setCustom();
  }
  else {
    setJAP();
  }
}

/**
 * Esta funcion no hace nada, pero es necesario tenerla para salir del modo de
 * bajo consumo (sleep mode) en el que ponemos el arduino hasta que se pulsa
 * reset en la consola
 *
 * @param ---
 * @return void
 */
void pulsarReset() {
  // No hay que hacer nada, solo lo usamos para salir de low power
}

/**
 * Ejecuta la accion de pulsar el reset en la Mega Drive II, poniendo durante
 * un corto espacio de tiempo la linea de reset a GND, para luego volver al 
 * estado de alta impedancia (entrada, con valor 1) 
 * 
 * @param ---
 * @return void
 */
void reinicarMD() {
  pinMode(pinReset, OUTPUT);
  digitalWrite(pinReset, LOW);
  delay(150);
  pinMode(pinReset, INPUT);
  digitalWrite(pinReset, HIGH);
}

/**
 * Controla la pulsacion del boton de reset, determinando, segun el tiempo, si 
 * se debe resetear la consola, cambiar la frecuencia o cambiar la region
 * 
 * @param ---
 * @return void
 */
void controlarReset() {
  int transcurrido = 0;

  // Controlamos el estado a LOW porque el reset se pone a 0 con la pulsacion
  while ( digitalRead(resetInput) == LOW ) {
    delay(1);
    transcurrido++;

    if( transcurrido == 1000 ) {
      tone(pinBuzzer, 512, 200);
    }
    else if ( transcurrido == 2000 ) {
      tone(pinBuzzer, 1024, 200);
    } 
  }
  
  if ( transcurrido >= 2000 ) {
    cambiarRegion();
  }
  else if ( transcurrido >= 1000 ) {
    cambiarFrecuencia();
  }
  else {
    reinicarMD();
  }
}

// ----- EJECUCION ------------------------------------------------------------
void setup() {
  pinMode(pinJAP, OUTPUT);
  pinMode(pinNTSC, OUTPUT);
  pinMode(rled, OUTPUT);
  pinMode(bled, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(resetInput, INPUT_PULLUP);
  
  // La salida de reset tiene que estar en HIGH_Z
  pinMode(pinReset, INPUT);
  digitalWrite(pinReset, HIGH);

  // Pitido de inicio
  tone(pinBuzzer, 1024, 200);

  setEUR(); // Por defecto la region original al encender el PAL Europeo
}

void loop() {
  // Interrupcion que se activa con le boton reset (pasa de 1 a 0 al pulsar)
  attachInterrupt(digitalPinToInterrupt(2), pulsarReset, FALLING);

  // Entramos en modo sleep hasta la interrupcion
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  detachInterrupt(0); // Fin de la interrupcion 

  controlarReset(); // Controlamos la accion a realizar
}