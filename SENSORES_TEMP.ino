#include <U8g2lib.h> // Biblioteca para utilizaçao do I2C
#include <Wire.h>    // Biblioteca para utilizaçao do I2C
#include <ESP8266WiFi.h> //Biblioteca que contém as funcionalidades de Wi-Fi
#include <ESP8266WebServer.h> //Biblioteca que permite a criação de um servidor HTTP via página web;

const char* ssid = "NodeMCU - Teste AP"; // Nome da rede Access Point (Semelhante a um roteador)
const char* password = "Teste123"; //Senha da rede Access Point

const int SENSOR_TEMP[] = { 0, 0, 2, 3 };// CRIA UM ARRAY QUE DEFINE CADA PORTA DO SENSOR[1], SENSOR[2], SENSOR[3], IGNORANDO O 0 PARA FINS DE SIMPLIFICAÇAO.
const int pinos_selecao_canal[] = {D6, D5}; // vetor representa os pinos de seleção do canal no multiplexador.

float temp[4], valor_analog[4];// VARIAVEIS GLOBAIS PARA FAZER OS CALCULOS DE TEMPERATURA

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C displayLED(U8G2_R0, U8X8_PIN_NONE); // Display OLED 128x32 I2C

ESP8266WebServer server(80); //Instanciação do servidor web na porta padrão HTTP (porta 80);

String sensor_avg = ""; //String vazia para receber os valores de "contador" e posteriormente enviá-los à página via HTTP
String sensor_1 = "";
String sensor_2 = "";
String sensor_3 = "";

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

void paginaPrincipal() 
{
  String pagina = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset='UTF-8'>
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>
      <title>NodeMCU - Access Point</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          padding: 20px;
        }
        .caixa {
          border: 2px solid #333;
          border-radius: 10px;
          padding: 16px;
          width: 300px;
          margin: 0 auto 20px auto;
        }
        .linha {
          margin: 10px 0;
          font-size: 1.2em;
        }
        h2 {
          color: #004F92;
          margin: 0;
        }
      </style>

      <script>
        function atualizarDados() {
          fetch('/dados')
            .then(response => response.json())
            .then(data => {
              document.getElementById('sensor_avg').textContent = data.valorCont;
              document.getElementById('sensor_1').textContent = data.s1;
              document.getElementById('sensor_2').textContent = data.s2;
              document.getElementById('sensor_3').textContent = data.s3;
            });
        }
        setInterval(atualizarDados, 500);
        window.onload = atualizarDados;
      </script>

    </head>
    <body>
      <div class="caixa">
        <h2>Código Teste - Envio de Dados (NodeMCU para o servidor)</h2>
      </div>
      <div class="caixa">
        <div class="linha"><strong>Sensor AVG:</strong> <span id="sensor_avg">---</span></div>
        <div class="linha"><strong>Sensor 1:</strong> <span id="sensor_1">---</span></div>
        <div class="linha"><strong>Sensor 2:</strong> <span id="sensor_2">---</span></div>
        <div class="linha"><strong>Sensor 3:</strong> <span id="sensor_3">---</span></div>
      </div>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", pagina);
}

void setup() 
{
  Serial.begin(9600);

  displayLED.begin(); // inicializa display
  displayLED.setPowerSave(0); // desativa economia de energia
  displayLED.setFont(u8g2_font_5x7_tf);

  IPAddress local_ip(192, 168, 10, 1); // Endereço IP local do Access Point
  IPAddress gateway(192, 168, 10, 1);  // Mesmo IP se o ESP for o "roteador"
  IPAddress subnet(255, 255, 255, 0);  // Máscara padrão

  WiFi.softAPConfig(local_ip, gateway, subnet); // Configura o endereçamento da rede Wi-Fi do Access Point com as informações citadas anteriormente
  WiFi.softAP(ssid, password); // Cria a rede Wi-Fi do Access Point com os dados "ssid" (nome da rede) e "password" (senha da rede)
  Serial.println("IP do AP: " + WiFi.softAPIP().toString()); // Imprime na Serial o endereço IP configurado

  server.on("/dados", []() 
  { // Define uma rota HTTP para responder à requisição GET da função de JavaScript "atualizarDados()" declarada acima
    String json = "{"; // Definição da String que armazena o conteúdo do objeto JSON, e inicialização da mesma com "{"
    json += "\"valorCont\":\"" + sensor_avg + "\","; // Forma o par de chave "\"valorCont\":\"" com o valor da variável "ultimoValor"
    json += "\"s1\":\"" + sensor_1 + "\",";
    json += "\"s2\":\"" + sensor_2 + "\",";
    json += "\"s3\":\"" + sensor_3 + "\"";
    json += "}"; // Termina o objeto JSON com "}"
    server.send(200, "application/json", json); 
  });
  server.on("/", paginaPrincipal); // Inicializa, na rota padrão da página (ou seja, http://192.168.10.1/), todo o conteúdo encontrado na string "pagina" que está declarada na função "paginaPrincipal"
  server.begin(); // Inicialização do servidor "server"
  Serial.println("Servidor iniciado");

  pinMode(pinos_selecao_canal[0], OUTPUT); //coloca as portas digitais para controlar os pinos A e B do multiplexador.
  pinMode(pinos_selecao_canal[1], OUTPUT);
}

void loop() 
{
  calcular_temp(); // Calcula a temperatura
  display_print(); // Printa no Display

  sensor_avg = String(temp[0], 1); //A cada atualização do valor da variável, converte esse valor em uma String
  sensor_1 = String(temp[1], 1);
  sensor_2 = String(temp[2], 1);
  sensor_3 = String(temp[3], 1);

  server.handleClient();
  delay(1000);  // Aguarda 1 segundo para atualizar o valor da contagem
}
