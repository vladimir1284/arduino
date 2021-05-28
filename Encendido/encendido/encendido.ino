/*
  Programa para el control del encendido electrónico de un motor
  de combustión interna. 
        ____________
  E1   |            | 
  ---->|            | S
  E2   | Controller |---->
  ---->|            |
       |____________|

  Entradas:
  E1 - Señal del delco (Disparo en flanco descendente).
  E2 - Salida del comparador de la corriente en la bobina.
       Bobina Totalmente cargada en flanco descendente.
       Se considera CC si este flanco ocurre en un tiempo < Tcc 

  Salida:
  S  - Señal del control de carga (LOW) y descarga (HIGH) de la bobina
*/
#include <Arduino.h>
#include "edge_detector.h"

// Con propositos de depuración
#define DEBUG true
// Tiempo de espera entre las actualizaciones de estado
#define DEBUG_INTERVAL 2000 // ms
unsigned long lastPrint;
int val = 0;

// ======== Parámetros del controlador ========

// Estados de carga y descarga
#define CARGA LOW
#define DESCARGA !CARGA

// Velocidad que se asumen en el arranque
#define RPM_0 300

// Máxima velocidad del motor (rpm) en BAJA.
// Con velocidades mayores se asume el próximo régimen (MEDIA)
#define LIMITE_BAJA 500

// Máxima velocidad del motor (rpm) en MEDIA.
// Con velocidades mayores se asume el próximo régimen (ALTA)
#define LIMITE_MEDIA 1500

// Brecha de velocidad (rpm) para la histéresis de cambio de régimen
#define BRECHA_rpm 50

// Tiempo de Corto Circuito (us)
// Si la bobina carga en un tiempo menor a este se considera CC.
#define Tcc 1000

// Tiempo de Carga Inicial (us).
// Este es el tiempo de carga de la bobina que se usa al inicio.
#define Tci 3500

// Tiempo de Carga Mínimo (us).
// Este es el menor tiempo de carga de la bobina.
#define Tcm 1500

// Tiempo de Descarga (us).
// Este es el tiempo que le damos a la bobina para que descargue.
#define Td 1000

// ======== Configuración de patas ========

// Pîn de la entrada E1 (delco)
#define E1 PB12

// Pîn de la entrada E2 (Bobina cargada)
#define E2 PB14

// Pîn de la salida S (Control de la Bobina)
#define S PB15

// Pîn de salida indicado una carga demasiado rápida de la bobina
#define ERROR_BOBINA_PIN PB0

// Pîn de salida indicado CC la bobina
#define CC_BOBINA_PIN PB1

// ========      Variables globales    ========

// Régimen de trabajo del motor
enum regimenes_motor
{
  BAJA,
  MEDIA,
  ALTA
};

regimenes_motor regimen_motor = BAJA;

// Estados de la FSM del controlador
enum estados_FSM
{
  ESPERANDO,
  CARGANDO,
  DESCARGANDO,
  CORTOCIRCUITO
};

estados_FSM estado = ESPERANDO;

// Velocidad de rotación del motor
// Se asume RPM_0 en el arranque
int rpm = RPM_0;

// Detector de flancos de E1
EdgeDetector detector_E1 = EdgeDetector(E1);

// Detector de flancos de E2
//EdgeDetector detector_E2 = EdgeDetector(E2);

// Tiempo del último flanco ascendente de E1
unsigned long Tfa_E1 = 0;

// Tiempo del último flanco descendente de E1
unsigned long Tfd_E1 = 0;

// Tiempo del último flanco descendente de E2
unsigned long Tfd_E2 = 0;

// Tiempo del último cambio de estado
unsigned long Tce = 0;

// Tiempo de carga de la bobina
int tiempoDeCarga = Tci;

// Ésta es la función donde se establecen los valores iniciales
void setup()
{
  // inicialización de las patas
  pinMode(E1, INPUT);

  pinMode(E2, INPUT);

  pinMode(S, OUTPUT);
  digitalWrite(S, DESCARGA);

  pinMode(ERROR_BOBINA_PIN, OUTPUT);
  digitalWrite(ERROR_BOBINA_PIN, HIGH);

  pinMode(CC_BOBINA_PIN, OUTPUT);
  digitalWrite(CC_BOBINA_PIN, LOW);

  // Valores iniciales de las Variables
  rpm = RPM_0;
  regimen_motor = BAJA;
  estado = ESPERANDO;
  tiempoDeCarga = Tci;

  // Depuración
  if (DEBUG)
  {
    Serial.begin(115200);
    Serial.println("Iniciando el controlador\n === Parámetros ===");
    Serial.print("RPM_0: ");
    Serial.print(RPM_0);
    Serial.println("rpm");
    Serial.print("LIMITE_BAJA: ");
    Serial.print(LIMITE_BAJA);
    Serial.println("rpm");
    Serial.print("LIMITE_MEDIA: ");
    Serial.print(LIMITE_MEDIA);
    Serial.println("rpm");
    Serial.print("BRECHA_rpm: ");
    Serial.print(BRECHA_rpm);
    Serial.println("rpm");
    Serial.print("Tiempo de Corto Circuito: ");
    Serial.print(Tcc);
    Serial.println("us");
    Serial.print("Tiempo de Carga Inicial: ");
    Serial.print(Tci);
    Serial.println("us");
    Serial.print("Tiempo de Descarga: ");
    Serial.print(Td);
    Serial.println("us");
    Serial.print("Pin de E1: ");
    Serial.println(E1);
    Serial.print("Pin de E2: ");
    Serial.println(E2);
    Serial.print("Pin de S: ");
    Serial.println(S);
    Serial.println(" =================== ");

    lastPrint = 0;
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, val);
  }
}

// Ésta es la función principal que itera indefinidamente
void loop()
{
  // Depuración
  if (DEBUG)
  {
    if (millis() - lastPrint > DEBUG_INTERVAL)
    {
      lastPrint = millis();
      Serial.print("Velocidad del motor: ");
      Serial.print(rpm);
      Serial.print(". Régimen: ");
      Serial.print(regimen_motor);
      Serial.print(". Tiempo de carga: ");
      Serial.print(tiempoDeCarga);
      Serial.println("us");
      val = !val;
      digitalWrite(PC13, val);
    }
  }

  // Gestionar el régimen de operación
  gestionar_regimen();

  // Rutina principal
  switch (regimen_motor)
  {
  case BAJA:
    fsm_baja();
    break;
  //TODO Gestionar los regímenes de MEDIA y ALTA
  default:
    break;
  }
}

/* Función para convertir de Período (us) de E1 a rpm del motor
   Tener en cuenta de 2 períodos completos de E1 es una vuelta del 
   motor de 4 tiempos. Esta función está hecha para este tipo de motores.
*/
int periodo2rpm(int periodo)
{
  return 120e6 / periodo;
}

// Función para seleccionar el régimen de operación del controlador
void gestionar_regimen()
{
  switch (regimen_motor)
  {
  case BAJA:
    if (rpm > LIMITE_BAJA)
    {
      regimen_motor = MEDIA;
    }
    break;

  case MEDIA:
    if (rpm > LIMITE_MEDIA)
    {
      regimen_motor = ALTA;
    }
    else if (rpm < (LIMITE_BAJA - BRECHA_rpm))
    {
      regimen_motor = BAJA;
    }
    break;

  case ALTA:
    if (rpm < (LIMITE_MEDIA - BRECHA_rpm))
    {
      regimen_motor = MEDIA;
    }
    break;

  default:
    break;
  }
}

void fsm_baja()
{
  switch (estado)
  {
  case ESPERANDO:
    if (detector_E1.detect_falling())
    {
      digitalWrite(S, CARGA);
      estado = CARGANDO;
      Tfd_E1 = micros();
      Tce = Tfd_E1;
    }
    break;

  case CARGANDO:
    if (false)//detector_E2.detect_falling())
    {
      if ((micros() - Tce) < Tcc)
      {                            // Vigilar la carga de la bobina en busca de CC
        digitalWrite(S, DESCARGA); // Desconectar
        estado = CORTOCIRCUITO;
        digitalWrite(CC_BOBINA_PIN, HIGH);
        Tce = micros();
      }
      Tfd_E2 = micros();
      int TiempoDeCargaDetectado = Tfd_E2 - Tce; // asumimos que no hubo CC
      actualizarTiempoDeCarga(TiempoDeCargaDetectado);
    }
    if ((micros() - Tce) > tiempoDeCarga)
    {
      estado = DESCARGANDO;
      digitalWrite(S, DESCARGA); // Desconectar
      Tce = micros();
    }
    break;

  case DESCARGANDO:
    if ((micros() - Tce) > Td)
    {
      estado = ESPERANDO;
    }
    break;

  default:
    break;
  }
}

// Función para corregir el tiempo de carga de la bobina de acuerdo con E2
void actualizarTiempoDeCarga(int TiempoDeCargaDetectado)
{
  if (TiempoDeCargaDetectado < tiempoDeCarga)
  {
    tiempoDeCarga = (TiempoDeCargaDetectado + tiempoDeCarga) / 2;
  }
  if (tiempoDeCarga < Tcm)
  {
    // Esta condición es errónea, la bobina carga antes de lo esperado
    tiempoDeCarga = Tcm;
    digitalWrite(ERROR_BOBINA_PIN, LOW);
  }
  else
  {
    digitalWrite(ERROR_BOBINA_PIN, HIGH);
  }
}