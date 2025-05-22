#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "gpio.h"  // Se incluye gpio.h para el uso de GPIO_TypeDef en los prototipos

// Estructura que representa los registros del periférico RCC.
// Define los offsets correspondientes en base a la documentación del STM32.
typedef struct {
    volatile uint32_t CR;            // Control del reloj
    volatile uint32_t ICSCR;         // Reloj interno (HSI tuning)
    volatile uint32_t CFGR;          // Configuración del reloj
    volatile uint32_t PLLCFGR;       // Configuración del PLL
    volatile uint32_t PLLSAI1CFGR;   // Configuración del PLL SAI1
    volatile uint32_t PLLSAI2CFGR;   // Configuración del PLL SAI2
    volatile uint32_t CIER;          // Habilitación de interrupciones del reloj
    volatile uint32_t CIFR;          // Flags de interrupciones del reloj
    volatile uint32_t CICR;          // Banderas de interrupciones claras
    uint32_t RESERVED1;              // Espacio reservado (alineación)
    volatile uint32_t AHB1RSTR;      // Reset de periféricos en bus AHB1
    volatile uint32_t AHB2RSTR;      // Reset de periféricos en bus AHB2
    volatile uint32_t AHB3RSTR;      // Reset de periféricos en bus AHB3
    uint32_t RESERVED2;
    volatile uint32_t APB1RSTR1;     // Reset de periféricos en APB1 (grupo 1)
    volatile uint32_t APB1RSTR2;     // Reset de periféricos en APB1 (grupo 2)
    volatile uint32_t APB2RSTR;      // Reset de periféricos en APB2
    uint32_t RESERVED3;
    volatile uint32_t AHB1ENR;       // Habilitación de reloj para AHB1
    volatile uint32_t AHB2ENR;       // Habilitación de reloj para AHB2
    volatile uint32_t AHB3ENR;       // Habilitación de reloj para AHB3
    uint32_t RESERVED4;
    volatile uint32_t APB1ENR1;      // Habilitación de reloj para APB1 (grupo 1)
    volatile uint32_t APB1ENR2;      // Habilitación de reloj para APB1 (grupo 2)
    volatile uint32_t APB2ENR;       // Habilitación de reloj para APB2
} RCC_TypeDef;

// Dirección base del RCC (extraída del manual del STM32)
#define RCC_BASE (0x40021000U)
// Macro que permite acceder al periférico RCC como una estructura
#define RCC ((RCC_TypeDef *)RCC_BASE)

// Definición de frecuencias base del sistema
#define SYSCLK_FREQ_HZ    4000000UL      // Frecuencia del sistema (4 MHz)
#define HCLK_FREQ_HZ      SYSCLK_FREQ_HZ // Frecuencia del bus AHB (sin división)
#define PCLK1_FREQ_HZ     HCLK_FREQ_HZ   // Frecuencia del bus APB1
#define PCLK2_FREQ_HZ     HCLK_FREQ_HZ   // Frecuencia del bus APB2
#define TIM_PCLK_FREQ_HZ  PCLK1_FREQ_HZ  // Frecuencia del temporizador TIM3 (en APB1)

// Declaración de funciones para habilitar el reloj de distintos periféricos
void rcc_gpio_clock_enable(GPIO_TypeDef *gpio_port);  // Habilita reloj de un puerto GPIO
void rcc_syscfg_clock_enable(void);                   // Habilita reloj para SYSCFG (config. del sistema)
void rcc_usart2_clock_enable(void);                   // Habilita reloj para USART2
void rcc_tim3_clock_enable(void);                     // Habilita reloj para TIM3 (PWM)

#endif // RCC_H
