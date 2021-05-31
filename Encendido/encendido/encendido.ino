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
#define DEBUG_INTERVAL 500 // ms
unsigned long lastPrint;
bool val;

// ======== Parámetros del controlador ========

// Estados de carga y descarga
#define CARGA LOW
#define DESCARGA !CARGA

// Velocidad que se asumen en el arranque
#define RPM_0 300

// Velocidad máxima a la que puede trabajar el sistema
#define MAXRPM 6000

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

// Tiempo de Descarga Mínimo(us).
#define Td_min 1500

// Tiempo de Descarga Máximo(us).
#define Td_max 20000

// Parámetros de la recta para relacuionar Td con rpm
const float m = 4*(Td_max - Td_min) / (MAXRPM - RPM_0);
const float T0 = (Td_max * MAXRPM - Td_min * RPM_0) / (MAXRPM - RPM_0);

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
  CORTOCIRCUITO,
  CALCULANDO
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
unsigned long Tfa_E1;

// Tiempo del último flanco descendente de E1
unsigned long Tfd_E1;

// Tiempo del último flanco descendente de E2
unsigned long Tfd_E2;

// Tiempo del último cambio de estado
unsigned long Tce;

// Tiempo de carga de la bobina
int tiempoDeCarga;

// Este es el tiempo que le damos a la bobina para que descargue.
int Td;

// Tiempo actual en us
unsigned long AhoraMicros;

// Período de la señal E1
int periodoE1;

// Diferencia entre el período de la señal E1 y su estimado
int errorPerido;

// Momento estimado en el que se debe descargar la bobina
unsigned long tiempoEstimado;

// Ya ha sido detectado un flanco descendente
// El disparo debe ser lo antes posible
bool flanco_descendente;

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
  flanco_descendente = false;
  Td = Td_max;

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
    val = 0;
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);
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
      // val = !val;
      // digitalWrite(PC13, val);
    }
  }

  // Rutina principal
  fsm();
}

/* Función para convertir de Período (us) de E1 a rpm del motor
   Tener en cuenta de 2 períodos completos de E1 es una vuelta del 
   motor de 4 tiempos. Esta función está hecha para este tipo de motores.
*/
int periodo2rpm(int periodo)
{
  // Calcular las RPMs
  int value = 30e6 / periodo;
  if (value < RPM_0)
  {
    value = RPM_0;
  }
  if (value > MAXRPM)
  {
    value = MAXRPM;
  }
  // Calcular el tiempo de descarga
  Td = T0 - m * value;
  if (Td > Td_max)
  {
    Td = Td_max;
  }
  else if (Td < Td_min)
  {
    Td = Td_min;
  }

  return value;
}

// Pasar al estado cargando
void flancoE1detectado()
{
  // Actualizar el período
  periodoE1 = AhoraMicros - Tfd_E1;
  Tfd_E1 = AhoraMicros;
  flanco_descendente = true;
}

// Pasar al estado cargando
void cargarBobina()
{
  digitalWrite(S, CARGA);
  estado = CARGANDO;
  Tce = AhoraMicros;
}

// Función para controlar el sistema
void fsm()
{
  AhoraMicros = micros();
  switch (estado)
  {
  case ESPERANDO:
    if (detector_E1.detect_falling())
    {
      cargarBobina();
      flancoE1detectado();
    }
    if (regimen_motor == MEDIA)
    {
      if (AhoraMicros > tiempoEstimado)
      {
        cargarBobina();
      }
    }
    break;

  case CARGANDO:
    // if (false) //detector_E2.detect_falling())
    // {
    //   if ((AhoraMicros - Tce) < Tcc)
    //   {                            // Vigilar la carga de la bobina en busca de CC
    //     digitalWrite(S, DESCARGA); // Desconectar
    //     estado = CORTOCIRCUITO;
    //     digitalWrite(CC_BOBINA_PIN, HIGH);
    //     Tce = AhoraMicros;
    //   }
    //   Tfd_E2 = AhoraMicros;
    //   int TiempoDeCargaDetectado = Tfd_E2 - Tce; // asumimos que no hubo CC
    //   actualizarTiempoDeCarga(TiempoDeCargaDetectado);
    // }
    if (((AhoraMicros - Tce) > tiempoDeCarga) && flanco_descendente)
    {
      estado = DESCARGANDO;
      digitalWrite(S, DESCARGA); // Desconectar
      Tce = AhoraMicros;
      flanco_descendente = false;

      if (DEBUG)
      { // Monitoreamos el tiempo de espera
        digitalWrite(PC13, HIGH);
      }
    }
    if (!flanco_descendente)
    {
      if (detector_E1.detect_falling())
      {
        flancoE1detectado();
      }
    }
    break;

  case DESCARGANDO:
    if ((AhoraMicros - Tce) > Td)
    {
      // Calcular las RPMs del motor
      rpm = periodo2rpm(periodoE1);
      // Calcular el próximo inicio de carga
      tiempoEstimado = Tfd_E1 + periodoE1 - tiempoDeCarga;
      // Seleccionar el régimen adecuado a partir de las RPMs
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
      default:
        break;
      }
      // Próximo estado
      estado = ESPERANDO;
      if (DEBUG)
      { // Monitoreamos el tiempo de espera
        digitalWrite(PC13, LOW);
      }
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