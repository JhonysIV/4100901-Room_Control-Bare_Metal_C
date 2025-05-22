#include "gpio.h"            // Control de pines GPIO
#include "systick.h"         // Temporizador SysTick para delays y ticks
#include "nvic.h"            // Configuración de interrupciones externas (NVIC)
#include "uart.h"            // Comunicación UART (USART2)
#include "tim.h"             // Configuración del temporizador TIM3 para PWM
#include "room_control.h"    // Lógica de la aplicación (control de LEDs externos por botón/UART)

/**
 * @brief Parpadeo del LED LD2 (PA5) como "heartbeat" del sistema.
 * La función invierte el estado del LED cada 200 ms para indicar que el sistema está vivo.
 */
void heartbeat_led_toggle(void)
{
    static uint32_t last_tick = 0;  // Guarda el último tick en que se cambió el LED
    if (systick_get_tick() - last_tick >= 200) { // Si han pasado al menos 200 ms
        gpio_toggle_pin(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN); // Cambia el estado del LED
        last_tick = systick_get_tick(); // Actualiza el tick
    }
}

int main(void)
{
    // === Inicializaciones del sistema ===

    // Inicializa el SysTick para tener un tick cada 1 ms
    systick_init_1ms();

    // Configura el pin PA5 (LED LD2 del Nucleo) como salida
    gpio_setup_pin(GPIOA, 5, GPIO_MODE_OUTPUT, 0);

    // Configura el pin PA7 (LED externo ON/OFF) como salida
    gpio_setup_pin(GPIOA, 7, GPIO_MODE_OUTPUT, 0);

    // Configura el pin PC13 (botón B1 del Nucleo) como entrada
    gpio_setup_pin(GPIOC, 13, GPIO_MODE_INPUT, 0);

    // Habilita la interrupción externa para el botón en PC13
    nvic_exti_pc13_button_enable();

    // Inicializa el USART2 con baudrate de 115200 bps (PA2 = TX, PA3 = RX)
    uart2_init(115200);

    // Habilita la interrupción de recepción por USART2
    nvic_usart2_irq_enable();

    // Inicializa el PWM en TIM3 canal 1 (PA6), frecuencia = 1000 Hz
    tim3_ch1_pwm_init(1000);

    // Establece el ciclo de trabajo inicial del PWM en 70%
    tim3_ch1_pwm_set_duty_cycle(70);

    // Inicializa la lógica de la aplicación (room_control.c)
    room_control_app_init();

    // Mensaje inicial enviado al PC por UART
    uart2_send_string("\r\nSistema Inicializado. Esperando eventos...\r\n");
    
    // === Bucle principal ===
    while (1) {
        heartbeat_led_toggle();  // Mantiene el LED LD2 parpadeando cada 200 ms
        room_control_process();  // Ejecuta la lógica principal de la aplicación (interrupciones, UART, etc.)

        // Pequeño delay para evitar uso excesivo de CPU
        systick_delay_ms(10);
    }
}
