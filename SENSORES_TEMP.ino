#include <U8g2lib.h> // Biblioteca para utilizaçao do I2C
#include <Wire.h>    // Biblioteca para utilizaçao do I2C

const int SENSOR_TEMP[] = { 0, 0, 2, 3 };// CRIA UM ARRAY QUE DEFINE CADA PORTA DO SENSOR[1], SENSOR[2], SENSOR[3], IGNORANDO O 0 PARA FINS DE SIMPLIFICAÇAO.
const int pinos_selecao_canal[] = {D6, D5}; // vetor representa os pinos de seleção do canal no multiplexador.

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C displayLED(U8G2_R0, U8X8_PIN_NONE); // Display OLED 128x32 I2C

float temp[4], valor_analog[4];// VARIAVEIS GLOBAIS PARA FAZER OS CALCULOS DE TEMPERATURA

void ativa_porta_analog(int porta) // Esta função abre a porta do multiplexador que desejamos. E definimos as constantes no inicio do programa
{
  if(porta == SENSOR_TEMP[1]) // De acordo com a tabela de portas do 74HC4052, a porta 0 (pot) é aberta com o pino A = 0 e o pino B = 0.
  {
      digitalWrite(pinos_selecao_canal[0], 0);
      digitalWrite(pinos_selecao_canal[1], 0);
  }
  else if(porta == SENSOR_TEMP[2]) // De acordo com a tabela de portas do 74HC4052, a porta 2 (pot_2) é aberta com o pino A = 0 e o pino B = 1.
  {
      digitalWrite(pinos_selecao_canal[0], 0);
      digitalWrite(pinos_selecao_canal[1], 1);

  } 
  else // De acordo com a tabela de portas do 74HC4052, a porta 3 (ldr) é aberta com o pino A = 1 e o pino B = 1.
  {
      digitalWrite(pinos_selecao_canal[0], 1);
      digitalWrite(pinos_selecao_canal[1], 1);
  }
}

void calcular_temp()
{
  float soma = 0;
  for (int i = 1; i <= 3; i++)
  {
    ativa_porta_analog(SENSOR_TEMP[i]); 
    valor_analog[i] = analogRead(A0);
    temp[i] = ((100.0 * valor_analog[i] * 3.3)/(float)1023)-5; // (-5) PARA FICAR MAIS PRÓXIMO A TEMPERATURA AMBIENTAL REAL
    soma += temp[i];
  }
  temp[0] = soma/3;
}

void display_print()
{  
  displayLED.clearBuffer();
  for (int j = 0; j <= 3; j++)
  {
    if (j == 0) {displayLED.setCursor(0, 7); displayLED.print("Temp AVG"); }
    if (j == 1) {displayLED.setCursor(0, 16); }
    if (j == 2) {displayLED.setCursor(0, 23); }
    if (j == 3) {displayLED.setCursor(0, 30); }
    if (j > 0 && j <= 3) {displayLED.print("Temp "); displayLED.print(j); }
    displayLED.print(": ");
    displayLED.print(temp[j], 1);
    displayLED.print(" ");
    displayLED.write(0xB0); // Imprime o símbolo de grau
    displayLED.print("C");
  }  
  displayLED.sendBuffer();
}

void setup() 
{  
  displayLED.begin(); // inicializa display
  displayLED.setPowerSave(0); // desativa economia de energia
  displayLED.setFont(u8g2_font_5x7_tf);

  pinMode(pinos_selecao_canal[0], OUTPUT); //coloca as portas digitais para controlar os pinos A e B do multiplexador.
  pinMode(pinos_selecao_canal[1], OUTPUT);
}

void loop() 
{
  calcular_temp(); // Calcula a temperatura
  display_print(); // Printa no Display
  delay(1000);
}