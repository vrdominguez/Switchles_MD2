#include <Arduino.h>
#include "LowPower.h"

// Pines para control de megadrive
int pinJAP = 12;
int pinNTSC = 13;
int pinReset = 8;

// Pines para led indicador de region
int rled = 11;
int gled = 10;
int bled = 9;

// Pulsado de reset
int resetInput = 2;

// Region actual de consola
String region = "EUR";

// ----------------------------------------------------
void parpadearLED() {
  for( int i = 0; i < 4; i++) {
    analogWrite(rled,255);
    analogWrite(gled,255);
    analogWrite(bled,0);
    delay(100);
    analogWrite(rled,0);
    analogWrite(gled,0);
    analogWrite(bled,0);
    delay(100);
  }
}

void setEUR() {
  // Idioma no japones, 50Hz
  digitalWrite(pinJAP, HIGH);
  digitalWrite(pinNTSC, LOW);

  // Led azul, bandera UE
  analogWrite(rled,0);
  analogWrite(gled,0);
  for ( int intensidad = 0; intensidad <= 255; intensidad++ ) {
    analogWrite(bled,intensidad);
    delay(5);
  }

  // Guardamos la region
  region = "EUR";
}

void setUSA() {
  // Idioma no japones, 60Hz
  digitalWrite(pinJAP, HIGH);
  digitalWrite(pinNTSC, HIGH);
  
  // Led en rojo
  analogWrite(rled,0);
  analogWrite(bled,0);
  for ( int intensidad = 0; intensidad <= 255; intensidad++ ) {
    analogWrite(gled,intensidad);
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
  analogWrite(gled,0);
  analogWrite(bled,0);
  for ( int intensidad = 0; intensidad <= 255; intensidad++ ) {
    analogWrite(rled,intensidad);
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

// ----- EJECUCION ------------------------------------------------------------

void setup() {
  // Inicializamos los pines
  pinMode(pinJAP, OUTPUT);
  pinMode(pinNTSC, OUTPUT);
  pinMode(rled, OUTPUT);
  pinMode(bled, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(resetInput, INPUT);

  // Por defecto la region original al encender el PAL Europeo
  setEUR();
  Serial.println("INICIADO...");
}

void loop() {
  // Interrupcion del bboton RESET
  attachInterrupt(digitalPinToInterrupt(2), pulsarReset, RISING);

  // Entramos en modo sleep hasta la interrupcion
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // Fin de la interrupcion
  detachInterrupt(0); 

  int inicioPulsacion = millis();

  while ( digitalRead(resetInput) == HIGH ) {
    delay(100);
  }

  if ( (millis() - inicioPulsacion) >= 2000 ) {
    cambiarRegion();
  }
  else {
    reinicarMD(); // No implementado
  }
}