/*
  Programa Test Robot Minisumo V2
  Web del proyecto: https://aprendiendoarduino.wordpress.com/2021/04/02/robot-minisumo-v2/
  Repositorio del proyecto: https://github.com/jecrespo/Robot-MiniSumo-V2
*/

#include <Servo.h>

#define pin_buzzer 12
#define pin_ir_izquierdo 2
#define pin_ir_derecho 3
#define pin_ir_trasero 4
#define pin_trigger 5
#define pin_echo 6
#define pin_rueda_izquierda 9
#define pin_rueda_derecha 10

Servo rueda;

boolean estado_tests[] = {false, false, false, false, false, false, false};
String tests[] = {"buzzer", "ir_izquiero", "ir_derecho", "ir_trasero",
                  "ultrasonidos", "rueda_izquierda", "rueda_derecha"
                 };

void setup() {
  Serial.begin(9600);
  pinMode(pin_buzzer, OUTPUT);
  pinMode(pin_ir_izquierdo, INPUT);
  pinMode(pin_ir_derecho, INPUT);
  pinMode(pin_ir_trasero, INPUT);
  pinMode(pin_trigger, OUTPUT);
  pinMode(pin_echo, INPUT);

  secuencia_inicio();
}

void loop()
{
  int respuesta = muestraMenu();

  switch (respuesta) {
    case 1:
      prueba_buzzer(pin_buzzer);
      break;
    case 2:
      prueba_sensor_ir(pin_ir_izquierdo, 1);
      break;
    case 3:
      prueba_sensor_ir(pin_ir_derecho, 2);
      break;
    case 4:
      prueba_sensor_ir(pin_ir_trasero, 3);
      break;
    case 5:
      prueba_sensor_ultrasonidos(pin_trigger, pin_echo);
      break;
    case 6:
      prueba_rueda(pin_rueda_izquierda, 5);
      break;
    case 7:
      prueba_rueda(pin_rueda_derecha, 6);
      break;
    case 8:
      break;
    case 99:
      return;
    default:
      Serial.println(F("Opcion incorrecta."));
      return;
  }

  if (comprueba_tests() == 0) {
    Serial.print(F("Test finalizado con éxito, puedes empezar a programar!!!"));
    secuencia_inicio();
    while (true) { }
  }
  else {
    Serial.println(F("\n########"));
    Serial.println(F("Tests pendientes de pasar: "));
    for (int i = 0; i < 7; i++) {
      if (estado_tests[i] == false)
      {
        Serial.println(" - " + tests[i]);
      }
    }
    Serial.println(F("########"));
    delay(5000);
  }
}

int muestraMenu() {
  Serial.println(F("\n------------------------"));
  Serial.println(F("MENU TEST MINISUMO V2"));
  Serial.println(F("Selecciona el test a ejecutar:"));
  Serial.println(F("  1 - Zumbador/Buzzer"));
  Serial.println(F("  2 - Sensor IR Suelo Izquierdo"));
  Serial.println(F("  3 - Sensor IR Suelo Derecho"));
  Serial.println(F("  4 - Sensor IR Suelo Trasero"));
  Serial.println(F("  5 - Sensor Ultrasonidos"));
  Serial.println(F("  6 - Rueda Izquierda"));
  Serial.println(F("  7 - Rueda Derecha"));
  Serial.println(F("  8 - Estado de los tests"));
  Serial.println(F("------------------------"));

  return recogeRespuesta();
}

void secuencia_inicio()
{
  Serial.println("Secuencia Inicio");
  for (int i = 0; i < 5; i++) {
    digitalWrite(pin_buzzer, HIGH);
    delay(50);
    digitalWrite(pin_buzzer, LOW);
    delay(50);
  }
}

void correcto()
{
  //Serial.println("Test Correcto");
  for (int i = 0; i < 2; i++) {
    digitalWrite(pin_buzzer, HIGH);
    delay(150);
    digitalWrite(pin_buzzer, LOW);
    delay(150);
  }
}

void error()
{
  //Serial.println("ERROR!!!");
  digitalWrite(pin_buzzer, HIGH);
  delay(1000);
  digitalWrite(pin_buzzer, LOW);
}

void prueba_buzzer (int pin)
{
  Serial.println("Test Zumbador/Buzzer en pin " + String(pin));
  correcto();
  Serial.println(F("¿Has escuchado dos pitidos seguidos? (s/n)"));
  int respuesta = recogeRespuesta();
  if (respuesta == 1)
  {
    estado_tests[0] = true;
    Serial.println(F("Test Correcto"));
  }
  else if (respuesta == 99)
    return;
  else {
    Serial.println("Comprueba que has puesto el cable del buzzer (amarillo/azul) en el pin " + String(pin_buzzer));
    Serial.println(F("Prueba a cambiar la polaridad del pin de buzzer, cambia sentido"));
  }
}

void prueba_sensor_ir (int pin, int test)
{
  Serial.println("Test sensor " + tests[test] + " en pin " + String(pin));
  Serial.println(F("Pon el robot en una superficie negra"));

  int segundos = 0;
  while (digitalRead(pin) == false) {
    //Blanco
    if (segundos > 10) {
      Serial.println(F("\nNo detecta superficie negra."));
      Serial.println(F("Comprueba la calibración del sensor IR"));
      Serial.println("Comprueba que has puesto el cable en el pin " + String(pin) + "según el esquema de conexiones");
      error();
      return;
    }
    segundos++;
    delay(1000);
  }

  correcto();
  Serial.println(F("Superficie Negra"));
  Serial.println(F("Mueve a una superficie blanca el sensor infrarrojos"));

  segundos = 0;
  while (digitalRead(pin) == true) {
    //Negro
    if (segundos > 10) {
      Serial.println(F("\nNo detecta superficie blanca."));
      Serial.println(F("Comprueba la calibración del sensor IR"));
      Serial.println("Comprueba que has puesto el cable en el pin " + String(pin) + " según el esquema de conexiones");
      error();
      return;
    }
    segundos++;
    delay(1000);
  }

  correcto();
  Serial.println(F("Superficie Blanca"));
  Serial.println(F("Test Correcto"));
  estado_tests[test] =  true;

}

void prueba_sensor_ultrasonidos(int trigger, int echo)
{
  Serial.println("Test sensor Ultrasonidos:");
  Serial.println(" - pin trigger en " + String(trigger));
  Serial.println(" - pin echo  en " + String(echo));

  Serial.println(F("\nPon un obstaculo frente a robot a unos 40 cms y acercalo poco a poco hasta 2 cms"));
  Serial.println(F("Comprueba que detecta la distancia y que suena en buzzer según la distancia"));

  int cm = distancia_HC_SR04(trigger, echo);
  int segundos = 0;

  while (cm < 2 || cm > 100) {
    if (segundos > 10) {
      Serial.println(F("\nNo detecta objeto."));
      Serial.println(F("Comprueba que has puesto el cable en:"));
      Serial.println(" - pin echo: " + String(echo));
      Serial.println(" - pin trigger: " + String(trigger));
      Serial.println(F("según el esquema de conexiones"));
      error();
      return;
    }
    segundos++;
    delay(1000);
    cm = distancia_HC_SR04(trigger, echo);
  }

  while (cm > 2) {
    cm = distancia_HC_SR04(trigger, echo);
    if (cm > 200) {
      Serial.println(F("\nError al detectar objeto"));
      Serial.println(F("\nComprueba según el esquema de conexiones"));
      error();
      return;
    }
    Serial.println("Objeto detectado a " + String(cm) + " cms");
    digitalWrite(pin_buzzer, HIGH);
    delay(50);
    digitalWrite(pin_buzzer, LOW);
    delay(cm * 10);
  }

  correcto();
  Serial.println(F("Test Correcto"));
  estado_tests[4] =  true;
}

void prueba_rueda(int pin, int test)
{
  Serial.println("Test " + tests [test] + " en pin " + String(pin));
  rueda.attach(pin);
  //servo parado
  int valor_parado = 90;
  int valor_adelante = 180; //servo 100% CW
  int valor_atras = 0; //servo 100% CCW
  rueda.write(valor_parado);
  delay(5000);

  if (test == 5)
  {
    //Rueda izquierda
    rueda.write(valor_adelante);
    delay(1500);

    rueda.write(valor_atras);
    delay(1500);
  }
  else
  {
    //Rueda derecha
    rueda.write(valor_atras);
    delay(1500);

    rueda.write(valor_adelante);
    delay(1500);
  }

  rueda.detach();

  Serial.println(F("¿Se ha movido la rueda adelante y luego atras? (s/n)"));
  int respuesta = recogeRespuesta();
  if (respuesta == 1)
  {
    estado_tests[test] = true;
    correcto();
    Serial.println(F("Test Correcto"));
  }
  else if (respuesta == 99)
    return;
  else {
    Serial.println("Comprueba que has puesto el cable en el pin " + String(pin));
    Serial.println(F("Segun esquema de conexiones"));
  }
}

int comprueba_tests()
{
  int errores = 0;

  for (int i = 0; i < 7; i++) {
    if (estado_tests[i] == false)
      errores++;
  }

  return errores;
}

int recogeRespuesta() {
  int segundos = 0;
  String cadena_leida = "";

  while (Serial.available() == 0) {
    if (segundos > 10) {
      Serial.println(F("\nNo hay respuesta..."));
      return 99;
    }
    //Serial.print(".");
    segundos++;
    delay(1000);
  }

  if (Serial.available() > 0) {
    do {
      char caracter_leido = Serial.read();
      cadena_leida += caracter_leido;
      delay(5);
    }  while (Serial.available() > 0);

    //Serial.println("\nHe Leido la cadena: " + cadena_leida);

    if (cadena_leida.startsWith("s"))
      return 1;
    else if (cadena_leida.startsWith("n"))
      return 0;
    else {
      int respuesta = cadena_leida.toInt();
      return respuesta;
    }
  }
}

int distancia_HC_SR04(int TriggerPin, int EchoPin) {
  long duration, distanceCm;

  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos

  distanceCm = duration * 10 / 292 / 2;  //convertimos a distancia, en cm
  return distanceCm;
}
