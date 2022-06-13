#include <ESP8266WiFi.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 8266 E WEB SERVER
#include <WiFiClient.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 8266 E WEB SERVER
#include <ESP8266WebServer.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 9266 E WEB SERVER
#include <ESP8266mDNS.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 8266 E WEB SERVER
#include <Wire.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO RELÓGIO
#include "RTClib.h" //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO RELÓGIO

RTC_DS3231 rtc; // IDENTIFICAÇÃO DO OBJETO DO TIPO RTC_DS3231
DateTime now;

int horaAtual, minutoAtual; //CRIAÇÃO DAS VARIAVEIS PARA IDENTIFICAR AS HORAS
int horaLigar, horaDesligar; //CRIAÇÃO DAS VARIAVEIS QUE DETERMINARÃO O FUNCIONAMENTO DO DISPOSITIVO
int condicao; //CRIAÇÃO DA VARIAVEL PARA IDENTIFICAR SE O DISPOSITIVO ESTÁ LIGADO OU DESLIGADO

#define LDR A0 //DEFINE A0 COMO SAIDA DE INFORMAÇÕES DO SENSOR DE LUMINOSIDADE
#define lampada D5 //DEFINE A5 COMO A PORTA DA LÃMPADA

int limiteSensibilidade = 800; //DEFINE A SENSIBILIDADE ACEITÁVEL PARA SER DETERMINADA COMO LIGADA, ENTRE 0 E 1024

const char* ssid = "********"; //USUÁRIO DA REDE
const char* password = "********"; //SENHA DA REDE

ESP8266WebServer server(80); //DEFINE A PORTA QUE O WEB SERVER IRÁ RESPONDER

const int led = 2; //DEFINIÇÃO DO LED DO ESP8266

void handleRoot() {
    digitalWrite(led, 1);
    
    String texto1 = "Quantidade de luminosidade interna = ";
    
    String ligar;
    if (condicao == false && horaAtual >= horaLigar || horaAtual <= horaDesligar)
    {
      ligar = "; Foi enviado sinal para ligar; ";
      condicao = true;
    }
  
    if (condicao == true && horaAtual >= horaDesligar)
    {
      ligar = "; Foi enviado sinal para desligar; ";
      condicao = false;
    }
    
    String checagem;
    if (analogRead (A0) >= limiteSensibilidade && condicao == true)
    {
      checagem = " CHECAGEM - CONFIRMADO = Ligado.";
    }
    if (analogRead (A0) <= limiteSensibilidade && condicao == true)
    {
      checagem = " CHECAGEM - ERRO = Desligado.";
    }

    if (analogRead (A0) <= limiteSensibilidade && condicao == false)
    {
      checagem = " CHECAGEM - CONFIRMADO = Desligado.";
    }
    if (analogRead (A0) >= limiteSensibilidade && condicao == false)
    {
      checagem = " CHECAGEM - ERRO = Ligado.";
    }
  
  String textoHTML;

  textoHTML = texto1;
  textoHTML += analogRead(A0);
  textoHTML += ligar; // DESCOMENTAR QUANDO FUNCIONAR O RELÓGIO
  textoHTML += checagem;
   
  server.send(200, "text/html", textoHTML);
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(lampada, OUTPUT); //DEFINE A LÂMPADA COMO SAÍDA
  pinMode(led, OUTPUT); //DEFINE O LED DO ESP 8266 COMO SAÍDA
  digitalWrite(led, 0); //DEFINE O LED DO ESP 8266 COMO DESLIGADO INICIALMENTE
  Serial.begin(115200); //DEFINE A VELOCIDADE DO MONITOR SERIAL
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  if(! rtc.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ
  while(1); //SEMPRE ENTRE NO LOOP
  }
  now = rtc.now(); //CHAMADA DE FUNÇÃO
  
  horaLigar = 18; //DEFINE O HORÁRIO EM QUE A LÂMPADA SERÁ LIGADA
  horaDesligar = 6; //DEFINE O HORÁRIO EM QUE A LÂMPADA SERÁ DESLIGADA
  condicao = 0; //CONDIÇÃO DA LÂMPADA POR PADRÃO
  //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO 
}

void loop(void){
  server.handleClient();
  //
  now = rtc.now();
  horaAtual = now.hour(); //CAPTURA A HORA ATUAL DENTRO DESTA VARIAVEL, SENDO CONSTANTEMENTE ATUALIZADA
  minutoAtual = now.minute(); //CAPTURA O MINUTO ATUAL DENTRO DESTA VARIAVEL, SENDO CONSTANTEMENTE ATUALIZADA
  
  if (horaAtual >= horaLigar || horaAtual <= horaDesligar && condicao == 0) //SE A HORA ATUAL FOR MAIOR QUE A HORA DE LIGAR OU MENOR QUE A HORA DE DESLIGAR E A LÂMPADA ESTIVER DESLIGADA, FAZ
  {
    digitalWrite (lampada, HIGH); //LIGA A LÂMPADA
    condicao = 1; //DEFINE A CONDIÇÃO DA LÂMPADA COMO LIGADA
  }
  
  if (horaAtual >= horaDesligar && condicao == 1) //SE A HORA ATUAL FOR MAIOR QUE A HORA DE DESLIGAR E A LÂMPADA ESTIVER LIGADA, FAZ
  {
    digitalWrite (lampada, LOW); //DESLIGA A LÂMPADA
    condicao = 0; //DEFINE A CONDIÇÃO DA LÂMPADA COMO DESLIGADA
  }

  delay(1000); //INTERVALO DE 1 SEGUNDO PARA REFAZER A ROTINA
}
