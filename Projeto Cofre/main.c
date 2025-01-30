// ---------------------------------------------------------------
// Desenvolvido por: Rafael Gonçalves Sousa
// Monitoria: 06
// E-mail: rafael.sousa@acad.ifma.edu.br
// ---------------------------------------------------------------

//Bibliotecas utilizadas
#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <time.h>
#include <ctype.h>


#define LED_R 13 // Pino do LED Vermelho
#define LED_G 11 // Pino do LED Verde
#define password_length 4 // Tamanho da senha (4 dígitos)

// Senha cadastrada no sistema
char reg_password[password_length + 1] = {0}; 

// Definição dos pinos para o display de 7 segmentos e multiplexação
const uint8_t seg_pins[] = {14, 15, 16, 17, 18, 19, 20}; // Pinos para o display de 7 segmentos
const uint8_t mux_display_pins[] = {21, 22, 26, 27};  // Pinos de multiplexação (para 4 dígitos)

// Definição dos pinos do teclado matricial
const uint8_t row_pins[4] = {9, 8, 7, 6}; // Pinos das linhas (R1 a R4)
const uint8_t col_pins[4] = {5, 4, 3, 2}; // Pinos das colunas (C1 a C4)

// Mapeamento dos dígitos do display de 7 segmentos
const uint8_t display_map[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 0, 0, 1, 1}  // 9
};

// Mapeamento do teclado matricial
const char key_map[4][4] = {
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'}, 
  {'*', '0', '#', 'D'} 
};

// Desativa os displays de 7 segmentos
void disable_display() {
  for (int i = 0; i < 4; i++) {
    gpio_put(mux_display_pins[i], 1); // Desliga todos os displays
  }
}

// Ativa um display específico (exibe um dígito em um display multiplexado)
void show_display(int digit, int mux) {
  disable_display();
  for (int i = 0; i < 7; i++) {
    gpio_put(seg_pins[i], display_map[digit][i]); // Exibe o dígito correspondente no display
  }
  gpio_put(mux_display_pins[mux], 0); // Ativa o display correspondente
}

// Confirmação da senha digitada com piscadas dos displays (somente para cadastro e acesso permitido realizados com sucesso)
void confirm_pass_blinks(char *password, int time_ms) {
  uint32_t start_time = to_ms_since_boot(get_absolute_time());
  while (to_ms_since_boot(get_absolute_time()) - start_time < time_ms) {
    for (int i = 0; i < 4; i++) {
      show_display(password[i] - '0', i); // Exibe cada dígito da senha
      sleep_ms(5);
    }
    disable_display();
    sleep_ms(750); // Intervalo entre piscadas
  }
}

// Confirmação da senha digitada sem piscadas
void confirm_pass(char *password, int time_ms) {
  uint32_t start_time = to_ms_since_boot(get_absolute_time());
  while (to_ms_since_boot(get_absolute_time()) - start_time < time_ms) {
    for (int i = 0; i < 4; i++) {
      show_display(password[i] - '0', i); // Exibe cada dígito da senha
      sleep_ms(5);
    }
  }
}

// Controla o estado dos LEDs (vermelho e verde)
void set_leds(bool r, bool g) {
  if (r) {
    gpio_put(LED_R, r);
    sleep_ms(1000); // LED vermelho acende por 1 segundo
    r = !r;
  }
  gpio_put(LED_R, r);
  gpio_put(LED_G, g); // Acende o LED verde
}

// Inicializa os pinos do display de 7 segmentos
void display_init() {
  // Inicializa os pinos dos segmentos
  for (int i = 0; i < 7; i++) {
    gpio_init(seg_pins[i]);
    gpio_set_dir(seg_pins[i], GPIO_OUT);
  }
  // Inicializa os pinos de multiplexação
  for (int i = 0; i < 4; i++) {
    gpio_init(mux_display_pins[i]);
    gpio_set_dir(mux_display_pins[i], GPIO_OUT);
    gpio_put(mux_display_pins[i], 1); // Desliga os displays
  }
}

// Inicializa os LEDs (vermelho e verde)
void leds_init() {
  for (int led_pin = LED_G; led_pin <= LED_R; led_pin = led_pin + 2) {
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
  }
}

// Inicializa o teclado matricial
void keypad_init() {
  for (int i = 0; i < 4; i++) {
    // Inicializa os pinos das linhas
    gpio_init(row_pins[i]);
    gpio_set_dir(row_pins[i], GPIO_OUT);
    gpio_put(row_pins[1], 0);

    // Inicializa os pinos das colunas
    gpio_init(col_pins[i]);
    gpio_set_dir(col_pins[i], GPIO_IN);
    gpio_pull_down(col_pins[i]);
  }
}

// Lê uma tecla pressionada no teclado matricial
char read_keypad() {
  for (int row = 0; row < 4; row++) {
    gpio_put(row_pins[row], 1); // Coloca a linha atual em nível lógico alto
    for (int col = 0; col < 4; col++) {
      if (gpio_get(col_pins[col])) {
        sleep_ms(100); // Tempo de estabilização da tecla pressionada
        while (gpio_get(col_pins[col])); // Espera a liberação da tecla
        gpio_put(row_pins[row], 0); // Reseta a linha atual (nível lógico baixo)
        return key_map[row][col]; // Retorna o caractere correspondente à tecla pressionada
      }
    }
    gpio_put(row_pins[row], 0); // Coloca a linha atual em nível lógico baixo
  }
  return 0;
}

// Função para capturar a senha digitada pelo usuário
void get_password(char *password, int length) {
  printf("Senha: ");
  int index = 0; //variável de controle do número de teclas digitadas
  while (index < length) {
    char key = read_keypad();
    if (key != 0 && isdigit(key)) { //Verifica se alguma tecla foi pressionada e se a mesma é um número.
      password[index] = key;
      printf("%c", key);
      index++;
      sleep_ms(5);
    }
    for (int i = 0; i <= index - 1; i++) {
      show_display(password[i] - '0', i); // Exibe os dígitos já digitados
      sleep_ms(5);
    }
  }
  password[length] = '\0'; // Finaliza a senha com o caractere nulo
}

// Função para comparar duas senhas
bool compare_passwords(const char *reg_password, const char *pass_2) {
  return strcmp(reg_password, pass_2) == 0; // Retorna verdadeiro se as senhas forem iguais
}

// Função para autenticar o usuário
bool authenticate(bool acess, int cnt) {
  printf("\n--------------------------------------\n");
  printf("Digite a senha do cofre com 4 digitos.\n");
  char input[password_length + 1] = {0}; // Variável para armazenar a senha digitada
  get_password(input, password_length);
  confirm_pass(input, 1000);
  if (compare_passwords(input, reg_password)) {
    printf("\nSenha correta, acesso permitido!\n");
    set_leds(false, true); // Acende o LED verde
    acess = !acess; // Altera o estado de acesso
    confirm_pass_blinks(input, 3000); // Exibe a senha no display com piscadas
  } else {
    disable_display(); // Desliga os displays
    cnt++;
    if (cnt < 3) {
      printf("\nSenha incorreta, acesso negado!\n");
      printf("Você só tem mais %d tentativas!\n", 3 - cnt);
      set_leds(true, false); // Acende o LED vermelho
    } else {
      printf("\nVocê excedeu o número de tentativas");
      printf("\nSISTEMA TEMPORARIAMENTE BLOQUEADO (5 SEGUNDOS)!");
      cnt = 0;
      set_leds(true, false); // Acende o LED vermelho
      sleep_ms(5000); // Bloqueio por 5 segundos
    }
  }
  return acess;
}

// Função para controlar o número de tentativas de autenticação
int attempts_control(int cnt) {
  cnt++;
  if (cnt == 3) {
    cnt = 0; // Reinicia o contador após 3 tentativas
  }
  return cnt;
}

// Função principal
int main() {
  stdio_init_all(); // Inicializa a biblioteca padrão
  keypad_init();    // Inicializa o teclado
  leds_init();      // Inicializa os LEDs
  display_init();   // Inicializa os displays

  printf("******************* SISTEMA DE COFRE ***********************\n");
  printf("Cadastre a senha do cofre com 4 digitos!\n");
  get_password(reg_password, password_length); // Registra a senha do cofre
  printf("\nSenha cadastrada com sucesso!\n");
  confirm_pass(reg_password, 1000); // Confirma a senha cadastrada

  confirm_pass_blinks(reg_password, 3000); // Exibe a senha com piscadas

  int cnt = 0; // Contador de tentativas
  bool acess = true; // Variável de controle de acesso

  while (acess) {
    disable_display(); // Desliga os displays
    acess = authenticate(acess, cnt); // Tenta autenticar o usuário
    cnt = attempts_control(cnt); // Controla o número de tentativas
  }
}
