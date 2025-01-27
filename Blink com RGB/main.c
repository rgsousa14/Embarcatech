/**
 * Desenvolvido por: Rafael Sousa
 * E-mail: rafael.sousa@acad.ifma.edu.br
 * Monitoria: 06
 * Projeto desenvolvido no curso Embarcatech
 */

#include <stdio.h>
#include "pico/stdlib.h"    // Biblioteca padrão para Raspberry Pi Pico

// Definição dos pinos dos LEDs RGB
#define LED_G 11  // Pino correspondente ao LED verde
#define LED_B 12  // Pino correspondente ao LED azul
#define LED_R 13  // Pino correspondente ao LED vermelho

// Função para configurar o estado dos LEDs RGB
void set_RGB(bool r, bool g, bool b) {
    gpio_put(LED_R, r); // Configura o estado do LED vermelho
    gpio_put(LED_G, g); // Configura o estado do LED verde
    gpio_put(LED_B, b); // Configura o estado do LED azul
}

// Função para Inicializar os pinos dos LEDs
void leds_init(){
  for (int led = LED_G; led <= LED_R; led++){
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);
  }
}

int main() {
    // Inicializa a interface padrão de entrada/saída
    stdio_init_all();

    //Inicializa os leds
    leds_init();
    
    // Loop principal para alternar as cores do LED
    while (true) {
        set_RGB(true, false, false);  // Acende o LED vermelho
        printf("\nLED VERMELHO LIGADO"); // Mensagem no terminal
        sleep_ms(1000);  // Espera 1 segundo

        set_RGB(false, true, false); // Acende o LED verde
        printf("\nLED VERDE LIGADO"); // Mensagem no terminal
        sleep_ms(1000);  // Espera 1 segundo

        set_RGB(false, false, true); // Acende o LED azul
        printf("\nLED AZUL LIGADO"); // Mensagem no terminal
        sleep_ms(1000);  // Espera 1 segundo

        set_RGB(true, true, true); // Acende todos os LEDs (cor branca)
        printf("\nLED BRANCO LIGADO"); // Mensagem no terminal
        sleep_ms(1000);  // Espera 1 segundo
    }
}
