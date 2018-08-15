# Switchless MD2
Cambio de region para MegaDrive 2 sin interruptores, se utiliza el botón de reset de la propia consola

## Dependencias

### Librería LowPower
Debemos descargarla de su proyecto oficial en Github ([rocketscream/Low-Power](https://github.com/rocketscream/Low-Power)) y situarla en el directorio de librerías (lib) dentro de la carpeta LowPower

## Funcionamiento
El arduino pasa a tomar el conrol del reset de la consola, permitiéndo su uso para controlarlas regiones de la misma, teniendo 2 posibles acciones:

  - Si pulsamos reset durante menos de 2 segundos, la consola se reiniciará de forma normal
  - Si pulsamos reset durante 2 segundos o más, la consola rotará entre las siguientes regiones
    - EUR: No japonesa a 50Hz (por defecto al encender)
    - USA: No japonesa a 60Hz (NTSC)
    - JAP: Japonesa a 60Hz