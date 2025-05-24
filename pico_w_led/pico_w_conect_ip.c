/*
    ***********************************************************************
    Projeto: ALerta
    Autor: Valmir Linhares de Sousa de Mesquita
    Data: 24 de Maio de 2015
    *************************************************************************
    Projeto ALerta com aap via celular e notbook para 
    controle de leds, o mesmo ao clicar no botão ALERTA enviara 
    um sinal de alerta acendendo um lede que piscara 15 vezes e 
    enviara mensagens dno terminal serial mostrando que o alerta foi ligado 
    e ao clicar no botão desligar alerta encerra e apagarar o led de alerta 
    botão sirene enviara um sinal de sonoro do buzzer e brilho do led vermelho 
    indicando o alerta. 
    Link GitHub: 
    Link YouTube: https://youtu.be/BM0-H7W6x4g

 */

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Define o nome e a senha da rede Wi-Fi a ser conectada
#define WIFI_SSID "MESQUITA"      // Nome da rede Wi-Fi
#define WIFI_PASS "DudaLeti2017" // Senha da rede Wi-Fi


#define LED_PIN 12               // Pino GPIO utilizado para o LED
#define LED_PIN2 13               // Pino GPIO utilizado para o LED
#define LED_PIN3 11               // Pino GPIO utilizado para o LED


 // Configuração do pino do buzzer
 #define BUZZER_PIN 21
 
 // Configuração da frequência do buzzer (em Hz)
 #define BUZZER_FREQUENCY 8000

// Definição de uma função para inicializar o PWM no pino do buzzer
 void pwm_init_buzzer(uint pin) {
     // Configurar o pino como saída de PWM
     gpio_set_function(pin, GPIO_FUNC_PWM);
 
     // Obter o slice do PWM associado ao pino
     uint slice_num = pwm_gpio_to_slice_num(pin);
 
     // Configurar o PWM com frequência desejada
     pwm_config config = pwm_get_default_config();
     pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
     pwm_init(slice_num, &config, true);
 
     // Iniciar o PWM no nível baixo
     pwm_set_gpio_level(pin, 0);
 }
 
 // Definição de uma função para emitir um beep com duração especificada
 void beep(uint pin, uint duration_ms) {
     // Obter o slice do PWM associado ao pino
     uint slice_num = pwm_gpio_to_slice_num(pin);
 
     // Configurar o duty cycle para 50% (ativo)
     pwm_set_gpio_level(pin, 2048);
 
     // Temporização
     sleep_ms(duration_ms);
 
     // Desativar o sinal PWM (duty cycle 0)
     pwm_set_gpio_level(pin, 0);
 
     // Pausa entre os beeps
     sleep_ms(100); // Pausa de 100ms
 } 

// Resposta HTML enviada ao navegador após requisição
#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
                      "<!DOCTYPE html><html lang=\"pt-BR\">" \
                      "<head>" \
                      "<meta charset=\"UTF-8\">" \
                      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" \
                      "<title>Controle do LED</title>" \
                      "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css\" rel=\"stylesheet\">" \
                      "</head>" \
                      "<body class=\"bg-light\">" \
                      "<div class=\"container mt-5\">" \
                      "<div class=\"card shadow-lg\">" \
                      "<div class=\"card-body text-center\">" \
                      "<h1 class=\"card-title mb-4\">Controle do LED</h1>" \
                      "<p class=\"lead\"></p>" \
                      "<div class=\"btn-group-vertical\" role=\"group\" aria-label=\"Controles do LED\">" \
                      "<a href=\"/led/on\" class=\"btn btn-success btn-lg mb-3\">ALERTE</a>" \
                      "<a href=\"/led/off\" class=\"btn btn-danger btn-lg mb-3\">DESLIGA ALERTA</a>" \
                      "<a href=\"/led2/on\" class=\"btn btn-success btn-lg mb-3\">SIRENE</a>" \
                      "<a href=\"/led2/off\" class=\"btn btn-danger btn-lg mb-3\">DESLIGA SIRENE</a>" \                    
                      "</div>" \
                      "<div class=\"mt-4\">" \
                      "<small class=\"text-muted\">Valmir Linhares de Sousa de Mesquita.</small>" \
                      "</div>" \
                      "</div>" \
                      "</div>" \
                      "</div>" \
                      "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/js/bootstrap.bundle.min.js\"></script>" \
                      "</body>" \
                      "</html>\r\n"

// Callback para lidar com requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Extrai e interpreta a requisição HTTP
    char *request = (char *)p->payload;

    if (strstr(request, "GET /led/on")) {
        for (int i = 0; i < 15; i++)
        {
            gpio_put(LED_PIN, 1);  // Liga o LED
            sleep_ms(500);
            gpio_put(LED_PIN, 0);  // Liga o LED
            sleep_ms(500);
            printf("LED DO ALERTA LIGADO\n"); 
            
            
        }    
          
        
        //gpio_put(LED_PIN, 1);  // Liga o LED
        // Inicializar o PWM no pino do buzzer
        // pwm_init_buzzer(BUZZER_PIN);
        // beep(BUZZER_PIN, 500); // Bipe de 500ms

    } else if (strstr(request, "GET /led/off")) {
        gpio_put(LED_PIN, 0);  // Desliga o LED
        printf("ALERTA DESLIGADO\n");
    }

     if (strstr(request, "GET /led2/on")) {

        for (int i = 0; i < 10; i++)
        {
            /* code */
            gpio_put(LED_PIN2, 1);  // Liga o LED
            sleep_ms(500);
            gpio_put(LED_PIN2, 0);  // Desliga o LED
            sleep_ms(500);

            pwm_init_buzzer(BUZZER_PIN);
            beep(BUZZER_PIN, 500); // Bipe de 500ms
            printf("SIRENE LIGADA\n");
        }
        
        

    } else if (strstr(request, "GET /led2/off")) {
        gpio_put(LED_PIN2, 0);  // Desliga o LED
        printf("SIRENE DESLIGADA\n");
    }

    // if (strstr(request, "GET /led3/on")) {
    //     gpio_put(LED_PIN3, 1);  // Liga o LED
    //     pwm_init_buzzer(BUZZER_PIN);
    //     beep(BUZZER_PIN, 500); // Bipe de 500ms
    // } else if (strstr(request, "GET /led3/off")) {
    //     gpio_put(LED_PIN3, 0);  // Desliga o LED
    // }

    // Envia a resposta HTML para o cliente
    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);

    // Libera o buffer utilizado
    pbuf_free(p);

    return ERR_OK;
}

// Callback de conexão: define o callback HTTP para conexões novas
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Inicializa o servidor HTTP na porta 80
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();  // Inicializa comunicação USB
    sleep_ms(10000);   // Aguarda a conexão da porta serial

    // Inicializar o PWM no pino do buzzer
     //pwm_init_buzzer(BUZZER_PIN);

    printf("Iniciando servidor HTTP\n");

    // Inicializa o módulo Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando à rede Wi-Fi...\n");

    // Conecta à rede definida
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar à rede Wi-Fi\n");
        return 1;
    } else {
        // Conexão bem-sucedida
        printf("Conectado à rede Wi-Fi: %s\n", WIFI_SSID);  // Imprime o SSID

        // Lê e imprime o endereço IP atribuído
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP atribuído: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado com sucesso!\n");
    printf("Para ligar ou desligar o LED, acesse o endereço IP seguido de /led/on ou /led/off\n");

    // Inicializa o LED como saída
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

     gpio_init(LED_PIN2);
    gpio_set_dir(LED_PIN2, GPIO_OUT);

     gpio_init(LED_PIN3);
    gpio_set_dir(LED_PIN3, GPIO_OUT);

    // Inicia o servidor web
    start_http_server();

    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Mantém a pilha de rede ativa
        sleep_ms(100);      // Pequeno delay para não sobrecarregar a CPU
    }

    // (Este trecho nunca será alcançado, mas está aqui por completude)
    cyw43_arch_deinit();
    return 0;
}
