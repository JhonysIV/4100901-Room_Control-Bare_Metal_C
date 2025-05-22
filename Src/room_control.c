#include "room_control.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "tim.h"

// === Variables de estado internas del módulo ===
// Estas variables controlan el temporizador del LED encendido por botón.
static volatile uint32_t led_onoff_start_time = 0;  // Marca de tiempo cuando se encendió el LED
static volatile uint8_t led_onoff_active = 0;       // Bandera que indica si el LED está encendido temporalmente
static volatile uint32_t last_button_press_time = 0;// Guarda el tiempo de la última pulsación para evitar rebotes

/**
 * @brief Inicializa el estado de los LEDs y el PWM.
 */
void room_control_app_init(void)
{
    // Apaga el LED externo (PA7)
    gpio_write_pin(GPIOA, 7, GPIO_PIN_RESET);

    // Configura el PWM al 70% de ciclo de trabajo (PA6)
    tim3_ch1_pwm_set_duty_cycle(70);
}

/**
 * @brief Maneja el evento de pulsación del botón B1.
 *        Esta función debe ser llamada desde la interrupción EXTI de PC13.
 */
void room_control_on_button_press(void)
{
    uint32_t current_time = systick_get_tick();  // Obtiene el tiempo actual

    // === Antirrebote ===
    // Ignora la pulsación si ocurrió hace menos de 200 ms (rebote mecánico)
    if ((current_time - last_button_press_time) < 200) {
        return;
    }

    last_button_press_time = current_time;  // Actualiza el tiempo de la última pulsación

    // === Acciones cuando el botón es válido ===

    // Enciende el LED externo (PA7)
    gpio_write_pin(GPIOA, 7, GPIO_PIN_SET);

    // Guarda el tiempo de encendido
    led_onoff_start_time = current_time;

    // Activa la bandera de que el LED está encendido
    led_onoff_active = 1;

    // Envía mensaje por UART al PC
    uart2_send_string("Boton B1: Presionado. LED encendido por 3 segundos.\r\n");
}

/**
 * @brief Procesa caracteres recibidos por UART2.
 *        Se espera que sea llamada desde el handler de USART2.
 * 
 * @param received_char El carácter recibido.
 */
void room_control_on_uart_receive(char received_char)
{
    // Procesamiento de comandos por UART
    switch(received_char) {

        case 'h': // HIGH PWM
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100); // PWM al 100%
            uart2_send_string("PWM al 100%\r\n");
            break;

        case 'l': // LOW PWM
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0); // PWM al 0%
            uart2_send_string("PWM al 0%\r\n");
            break;

        case 't': // Toggle LED
            gpio_toggle_pin(GPIOA, 7); // Cambia el estado del LED externo
            uart2_send_string("Toggle LED ON/OFF\r\n");
            break;

        default:
            // Aquí se podría enviar un mensaje de "comando no reconocido"
            break;
    }
}

/**
 * @brief Función que debe llamarse de forma periódica (por ejemplo cada 10 ms en el main).
 *        Se encarga de apagar el LED si han pasado 3 segundos desde su encendido por botón.
 */
void room_control_process(void)
{
    // Verifica si el LED fue encendido por botón y si han pasado 3 segundos
    if (led_onoff_active && (systick_get_tick() - led_onoff_start_time >= 3000)) {
        // Apaga el LED
        gpio_write_pin(GPIOA, 7, GPIO_PIN_RESET);

        // Limpia la bandera
        led_onoff_active = 0;

        // Informa al usuario por UART
        uart2_send_string("LED apagado después de 3 segundos.\r\n");
    }
}
