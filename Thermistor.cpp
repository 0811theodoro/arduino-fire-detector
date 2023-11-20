/**************************************************************/
/*				max mayfield								  */
/*				mm systems									  */
/*				max.mayfield@hotmail.com					  */
/*															  */
/*	code based from code on Arduino playground found here:	  */
/*	http://www.arduino.cc/playground/ComponentLib/Thermistor2 */
/**************************************************************/

/* ======================================================== */

#include "Arduino.h"
#include "Thermistor.h"

const float BETA = 3950;

//--------------------------
Thermistor::Thermistor(int pin) {
  _pin = pin;
}

//--------------------------
double Thermistor::getTemp() {
  int analogValue = analogRead(_pin);

  double Temp;

  // Usando as orientacoes do link a seguir para medir a temperatura em graus celsius
  // https://docs.wokwi.com/pt-BR/parts/wokwi-ntc-temperature-sensor
  Temp = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15);
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius

  return Temp;  // Return the Temperature
}

/* ======================================================== */