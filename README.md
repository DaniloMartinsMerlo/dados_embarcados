# dados_embarcados
Firmware embarcado para Raspberry Pi Pico W que lê sensores e envia telemetria para um backend HTTP. Desenvolvido em C puro com o Pico SDK (Toolchain Nativo).

---

## Vídeo Demonstrativo

> 📺 [Assista a demonstração aqui](https://youtu.be/sBEYk1XRtx4)

---

## O que esse firmware faz

- Lê um sensor LDR (luminosidade) via ADC com média de 8 amostras
- Detecta pressão de um botão via interrupção de hardware com debouncing
- Conecta ao Wi-Fi automaticamente, reconectando se cair
- Envia os dados como JSON via HTTP POST para o backend a cada 5 segundos (ou imediatamente quando o botão é pressionado)

---

## Arquitetura dos Arquivos

```
Dados_embarcados/
│
├── main.c                  # Loop principal — orquestra leitura e envio
│
├── inc/                    # Headers (.h)
│   ├── button_handler.h    # Constantes e funções do botão
│   ├── ldr_handler.h       # Constantes e funções do LDR
│   └── wifi_handler.h      # Constantes de rede e funções Wi-Fi/HTTP
│
├── src/
│   ├── button_handler.c    # Lógica do botão com IRQ e debounce
│   ├── ldr_handler.c       # Leitura ADC do LDR
│   └── wifi_handler.c      # Conexão Wi-Fi e cliente HTTP sobre TCP (lwIP)
│
├── lwipopts.h              # Configurações da pilha de rede lwIP
└── CMakeLists.txt          # Script de compilação
```

Cada módulo é independente: o `.h` declara o que existe, o `.c` implementa como funciona.

---

## Sensores e Pinos

| Sensor  | Pino GPIO | Tipo     | Valores              |
|---------|-----------|----------|----------------------|
| LDR     | GPIO 26   | Analógico (ADC) | 0% (escuro) a 100% (claro) |
| Botão   | GPIO 15   | Digital (IRQ)   | 0 = solto, 1 = pressionado |

---

## Configuração de Rede

Abra o arquivo `inc/wifi_handler.h` e edite as linhas:

```c
#define WIFI_SSID      "nome_da_sua_rede"
#define WIFI_PASSWORD  "senha_da_rede"
#define BACKEND_IP     "ip_do_seu_backend"
#define BACKEND_PORT   Porta_do_backend
#define BACKEND_PATH   "/dados"
```

---

## Como Compilar

Com o Pico SDK instalado e configurado, rode na raiz do projeto:

```bash
mkdir build
cd build
cmake .. -DPICO_BOARD=pico_w
make
```

O arquivo gerado será `build/firmware.uf2`. Para gravar no Pico W, segure o botão BOOTSEL ao conectar o cabo USB, ele aparecerá como um pendrive. Copie o `.uf2` para dentro dele.

---

## Lendo a Porta Serial

Com o Pico W conectado via USB, rode no terminal:

```bash
minicom -b 115200 -o -D /dev/ttyACM0
```

Você verá os logs em tempo real:

```
[BOOT] Iniciando firmware...
[WIFI] Conectado!
[LDR] 73.45% → enviando...
[LDR] OK
[BTN] PRESSIONADO → enviando...
[BTN] OK
```

Para sair do minicom pressione `Ctrl+A` e depois `Q`.

---

## Backend (Atividade 1)

Este firmware envia dados para o backend desenvolvido na Atividade 1.  
Repositório do backend: [link_para_atividade_1](https://github.com/DaniloMartinsMerlo/ponderada_fila)