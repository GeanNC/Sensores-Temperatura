============================================================
   LEITOR DE TEMPERATURA COM MULTIPLEXADOR 74HC4052 E OLED
============================================================

DESCRIÇÃO:
-----------
Este projeto utiliza um NodeMCU (ESP8266), um multiplexador analógico 74HC4052
e um display OLED SSD1306 (128x32) via I2C para realizar a leitura de múltiplos
sensores de temperatura analógicos e exibir os valores em tempo real.

O sistema lê três sensores de temperatura conectados ao multiplexador,
calcula a média das leituras e mostra todas as temperaturas no display OLED.

------------------------------------------------------------
COMPONENTES UTILIZADOS:
------------------------------------------------------------
- NodeMCU (ESP8266)
- Multiplexador 74HC4052
- 3 Sensores de temperatura analógicos (ex: LM35)
- Display OLED SSD1306 (128x32) I2C
- Protoboard e jumpers para conexão

------------------------------------------------------------
CONEXÕES:
------------------------------------------------------------

MULTIPLEXADOR 74HC4052
 A (Select 0)  -> D6
 B (Select 1)  -> D5
 Z (Saída comum) -> A0 (entrada analógica)
 X0, X1, X2...  -> Sensores de temperatura
 VCC / GND      -> 3.3V / GND

DISPLAY OLED SSD1306 (I2C)
 VCC -> 3.3V
 GND -> GND
 SCL -> D1
 SDA -> D2

------------------------------------------------------------
FUNCIONAMENTO:
------------------------------------------------------------
1. O multiplexador seleciona qual sensor será lido através dos pinos D6 e D5.
2. O NodeMCU lê o valor analógico no pino A0.
3. O valor é convertido para temperatura usando a fórmula:
   temp[i] = ((100.0 * valor_analog[i] * 3.3) / 1023) - 5;
   (O "-5" é um ajuste para calibrar a leitura próxima à temperatura real.)
4. É calculada a média das três temperaturas.
5. As temperaturas individuais e a média são exibidas no display OLED.
6. Atualização ocorre a cada 1 segundo.

------------------------------------------------------------
FUNÇÕES PRINCIPAIS:
------------------------------------------------------------
ativa_porta_analog() - Seleciona o canal ativo no multiplexador.
calcular_temp()      - Lê sensores, converte e calcula média.
display_print()      - Exibe temperaturas no display OLED.
setup()              - Inicializa display e configura pinos.
loop()               - Atualiza leituras continuamente.

------------------------------------------------------------
EXEMPLO DE SAÍDA NO DISPLAY:
------------------------------------------------------------
Temp AVG: 25.3 °C
Temp 1:   24.9 °C
Temp 2:   25.6 °C
Temp 3:   25.4 °C

------------------------------------------------------------
BIBLIOTECAS NECESSÁRIAS:
------------------------------------------------------------
- U8g2 (por olikraus) -> para controle do display OLED
- Wire.h (já inclusa na IDE Arduino)