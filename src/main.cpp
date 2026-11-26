#include <Arduino.h>

// Definición de estados para la máquina de estados//
#define COMBINACION_1 0
#define COMBINACION_2 1
#define COMBINACION_3 2

// Variables globales para el control de tiempo y estados//
volatile uint32_t contador_ms = 0; // Cuenta milisegundos en la ISR//
uint8_t estado_leds = COMBINACION_1; // Estado inicial de la máquina//
uint8_t boton_presionado = 0; // Bandera/candado para el pulsador//nn
uint32_t tiempo_ultimo_pulso = 0; // Almacena el tiempo para el antirrebote//



void config_TIMER0()
{
TCCR0A = (1 << WGM01); // Modo CTC//
OCR0A = 62; // Interrupción cada ~1ms//
TCCR0B = (1 << CS02); // Prescaler de 256//
TIMSK0 = (1 << OCIE0A); // Habilita interrupción por comparación//
}


int main()
{
// Configura pines D2, D3, D4 y D5 como salidas (0b00111100)//
DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);

// Configura el pin A4 (PC4) como entrada con Pull-up activa//
DDRC &= ~(1 << PC4);
PORTC |= (1 << PC4);

config_TIMER0(); // Inicializa el temporizador//
sei(); // Habilita interrupciones globales//

while (1)
{
// 1. CONTROL DEL PULSADOR (Lógica del Ejercicio 3)//
if (bit_is_clear(PINC, PC4))
{
// Control de rebotes y flanco
if (boton_presionado == 0 && (contador_ms - tiempo_ultimo_pulso) > 50)
{
estado_leds++; // Salta al siguiente estado de luces//

// Si se pasa del último estado, vuelve al primero//
if (estado_leds > COMBINACION_3)
{
estado_leds = COMBINACION_1;
}

tiempo_ultimo_pulso = contador_ms; // Guarda el tiempo del pulso//
boton_presionado = 1; // Traba hasta que se suelte el botón//
}
}
else
{
boton_presionado = 0; // Al soltar el botón, se libera el candado//
}

// 2. MÁQUINA DE ESTADOS (Controla qué combinación de LEDs se muestra)//
switch (estado_leds)
{
case COMBINACION_1:
// Prenden solo D2 y D4 (Intercalados 1)//
PORTD = (PORTD & 0b11000011) | (1 << PD2) | (1 << PD4);
break;

case COMBINACION_2:
// Prenden solo D3 y D5 (Intercalados 2)//
PORTD = (PORTD & 0b11000011) | (1 << PD3) | (1 << PD5);
break;

case COMBINACION_3:
// Prenden todos los LEDs juntos (D2, D3, D4 y D5)//
PORTD = (PORTD & 0b11000011) | (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);
break;
}
}
}

//incrementa el contador general cada 1ms//
ISR(TIMER0_COMPA_vect)
{
contador_ms++;
}