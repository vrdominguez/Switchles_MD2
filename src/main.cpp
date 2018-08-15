/* ****************************************************************************
 * Control de cambio de region (idioma japones/otro, frecuenta 50/60Hz) para
 * la SEGA Mega Drive 2, mediante el propio botón de reset de la consola
 * ************************************************************************* */
#include <Arduino.h>
#include "LowPower.h"

// --- CONFIGURACION DE PINES CONECTADOS A LA CONSOLA -------------------------
int resetInput = 2; // Controla la interrupcion del boton reset
int pinJAP = 12;    // Controla el idioma de la consola
int pinNTSC = 13;   // Controla la frecuencia de refresco
int pinReset = 8;   // Controla el reseteo

// --- SALIDAS PARA CONTROLAR LA ILUMINACION DEL LED --------------------------
int rled = 11; // Pin rojo de led RGB
int gled = 10; // Pin verde de led RGB
int bled = 9;  // Pin azul de led RGB

// --- OTRAS VARIABLES --------------------------------------------------------
String region = "EUR";   // Guarda la region actual
int inicioPulsacion = 0; // Guarda milisesgundos de inicio de pulsacion

// --- FUNCIONES --------------------------------------------------------------
void colorearLed ( int r, int g, int b ) {
  analogWrite(rled,r);
  analogWrite(gled,g);
  analogWrite(bled,b);
}

void parpadearLED() {
  for( int i = 0; i < 4; i++) {
    colorearLed(253,106,3);
    delay(100);
    colorearLed(0,0,0);
    delay(100);
  }
}

void setEUR() {
  // Idioma no japones, 50Hz
  digitalWrite(pinJAP, HIGH);
  digitalWrite(pinNTSC, LOW);

  // Led azul
  for ( int intensidad = 0; intensidad <= 255; intensidad++ ) {
    colorearLed(0,0,intensidad);
    delay(5);
  }

  // Guardamos la region
  region = "EUR";
}

void setUSA() {
  // Idioma no japones, 60Hz
  digitalWrite(pinJAP, HIGH);
  digitalWrite(pinNTSC, HIGH);
  
  // Led verde
  for ( int intensidad = 0; intensidad <= 255; intensidad++ ) {
    colorearLed(0,intensidad,0);
    delay(5);
  }
  
  // Guardamos la region
  region = "USA";
}

void setJAP() {
  // Idioma no japones, 60Hz
  digitalWrite(pinJAP, LOW);
  digitalWrite(pinNTSC, HIGH);
  
  // Led en rojo, por la bandera de Japon
  for ( int intensidad = 0; intensidad <= 255; intensidad++ ) {
    colorearLed(intensidad,0,0);
    delay(5);
  }

  // Guardamos la region
  region = "JAP";
}

void cambiarRegion() {
  parpadearLED();
  
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

void pulsarReset() {
  // No hay que hacer nada, solo lo usamos para salir de low power
}

void reinicarMD() {
  // TODO: No implementado
}

void controlarReset() {
  inicioPulsacion = millis();

  while ( digitalRead(resetInput) == HIGH ) {
    // No es necesario hacer nada
  }

  if ( (millis() - inicioPulsacion) >= 2000 ) {
    cambiarRegion();
  }
  else {
    reinicarMD(); // No implementado
  }

  inicioPulsacion = 0;
}

// ----- EJECUCION ------------------------------------------------------------
void setup() {
  pinMode(pinJAP, OUTPUT);
  pinMode(pinNTSC, OUTPUT);
  pinMode(rled, OUTPUT);
  pinMode(bled, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(resetInput, INPUT);

  // Por defecto la region original al encender el PAL Europeo
  setEUR();
}

void loop() {
  // Interrupcion que se activa con le boton reset
  attachInterrupt(digitalPinToInterrupt(2), pulsarReset, RISING);

  // Entramos en modo sleep hasta la interrupcion
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // Fin de la interrupcion
  detachInterrupt(0); 

  controlarReset();
}