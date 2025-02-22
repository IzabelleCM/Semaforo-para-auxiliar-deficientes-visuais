#include <stdio.h>
#include "pico/stdlib.h"

const int ledVerde = 10;
const int ledAmarelo = 11;
const int ledVermelho = 12;
const int botao = 16;
const int ledPedestre = 18;
const int buzzer = 3;

const unsigned long tempoVerde = 8000;
const unsigned long tempoAmarelo = 2000;
const unsigned long tempoVermelho = 10000;

unsigned long tempoUltimaAcao = 0;
bool pedestreAtivo = false;

int main() {
    gpio_init(ledVerde);
    gpio_set_dir(ledVerde, GPIO_OUT);
    gpio_init(ledAmarelo);
    gpio_set_dir(ledAmarelo, GPIO_OUT);
    gpio_init(ledVermelho);
    gpio_set_dir(ledVermelho, GPIO_OUT);
    gpio_init(botao);
    gpio_set_dir(botao, GPIO_IN);
    gpio_pull_up(botao);
    gpio_init(ledPedestre);
    gpio_set_dir(ledPedestre, GPIO_OUT);
    gpio_init(buzzer);
    gpio_set_dir(buzzer, GPIO_OUT);

    while (true) {
        bool botaoPressionado = gpio_get(botao) == 0;

        if (botaoPressionado && !pedestreAtivo) {
            pedestreAtivo = true;
            acionarSemaforoPedestre();
        } else if (!pedestreAtivo) {
            executarSemaforoNormal();
        }

        sleep_ms(50);
    }
}

void executarSemaforoNormal() {
    unsigned long tempoAtual = to_ms_since_boot(get_absolute_time());
    unsigned long intervalo = tempoAtual - tempoUltimaAcao;

    if (intervalo < tempoVerde) {
        gpio_put(ledVerde, true);
        gpio_put(ledAmarelo, false);
        gpio_put(ledVermelho, false);
    } else if (intervalo < tempoVerde + tempoAmarelo) {
        gpio_put(ledVerde, false);
        gpio_put(ledAmarelo, true);
        gpio_put(ledVermelho, false);
    } else if (intervalo < tempoVerde + tempoAmarelo + tempoVermelho) {
        gpio_put(ledVerde, false);
        gpio_put(ledAmarelo, false);
        gpio_put(ledVermelho, true);
    } else {
        tempoUltimaAcao = to_ms_since_boot(get_absolute_time());
    }

    gpio_put(ledPedestre, false);
    gpio_put(buzzer, false);
}

void acionarSemaforoPedestre() {
    gpio_put(ledVerde, false);
    gpio_put(ledAmarelo, true);
    gpio_put(ledVermelho, false);
    sleep_ms(5000);

    gpio_put(ledAmarelo, false);
    gpio_put(ledVermelho, true);
    gpio_put(ledPedestre, true);

    for (int i = 0; i < 100; i++) {
        gpio_put(buzzer, true);
        sleep_ms(150);
        gpio_put(buzzer, false);
    }

    gpio_put(ledPedestre, false);
    gpio_put(buzzer, false);
    pedestreAtivo = false;
    tempoUltimaAcao = to_ms_since_boot(get_absolute_time());
}