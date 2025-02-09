#include <ESP8266WiFi.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 8266 E WEB SERVER
#include <WiFiClient.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 8266 E WEB SERVER
#include <ESP8266WebServer.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 9266 E WEB SERVER
#include <ESP8266mDNS.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO ESP 8266 E WEB SERVER
#include <Wire.h> //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO RELÓGIO
#include "RTClib.h" //INCLUSÃO DA BIBLIOTECA PARA FUNCIONAMENTO DO RELÓGIO

RTC_DS3231 rtc; //IDENTIFICAÇÃO DO OBJETO DO TIPO RTC_DS3231
DateTime now; //VERIFICA A DATA E HORA ATUAL DO COMPUTADOR LOCAL

int horaAtual, minutoAtual; //CRIAÇÃO DAS VARIAVEIS PARA IDENTIFICAR AS HORAS
int horaLigar = 18, horaDesligar = 6; //CRIAÇÃO DAS VARIAVEIS QUE DETERMINARÃO O FUNCIONAMENTO DO DISPOSITIVO
int condicao = 0; //CRIAÇÃO DA VARIAVEL PARA IDENTIFICAR SE O DISPOSITIVO ESTÁ LIGADO OU DESLIGADO

#define LDR A0 //DEFINE A0 COMO SAIDA DE INFORMAÇÕES DO SENSOR DE LUMINOSIDADE
#define lampada D5 //DEFINE A5 COMO A PORTA DA LÃMPADA

int limiteSensibilidade = 800; //DEFINE A SENSIBILIDADE ACEITÁVEL PARA SER DETERMINADA COMO LIGADA, ENTRE 0 E 1024

const char* ssid = "********"; //USUÁRIO DA REDE
const char* password = "********"; //SENHA DA REDE

ESP8266WebServer server(80); //DEFINE A PORTA QUE O WEB SERVER IRÁ RESPONDER

const int led = 2; //DEFINIÇÃO DO LED DO ESP8266

void handleRoot() { //RESPOSTA EXECUTADA QUANDO O HTML SOLICITAR INFORMAÇÕES
    digitalWrite(led, 1); //QUANDO A ROTINA FOR SOLICITADA, LIGAR O LED DO ESP 8266
    
    String texto1 = "Quantidade de luminosidade interna = "; //TEXTO PARA SER INSERIDO NA PÁGINA HTML
    
    String ligar; //VARIÁVEL QUE INFORMA O COMANDO ATUAL DA LÂMPADA, LIGADO OU DESLIGADO
    if (horaAtual >= horaLigar || horaAtual <= horaDesligar)
    {
      ligar = "; Foi enviado sinal para ligar; ";
    }
  
    if (horaAtual >= horaDesligar)
    {
      ligar = "; Foi enviado sinal para desligar; ";
    }
    
    String checagem; //VARIÁVEL QUE CHECA A CONDIÇÃO DA LÂMPADA, LIGADA OU DESLIGADA, COM O SENSOR DE LUMINOSIDADE E A INFORMAÇÃO DO COMANDO ATUAL ENVIADO PELA LÂMPADA
    if (analogRead (A0) >= limiteSensibilidade && condicao == 1)
    {
      checagem = " CHECAGEM - CONFIRMADO = Ligado.";
    }
    if (analogRead (A0) <= limiteSensibilidade && condicao == 1)
    {
      checagem = " CHECAGEM - ERRO = Desligado.";
    }

    if (analogRead (A0) <= limiteSensibilidade && condicao == 0)
    {
      checagem = " CHECAGEM - CONFIRMADO = Desligado.";
    }
    if (analogRead (A0) >= limiteSensibilidade && condicao == 0)
    {
      checagem = " CHECAGEM - ERRO = Ligado.";
    }
  
  String textoHTML; //DEFINE O QUE IRÁ APARECER NA PÁGINA HTML

  textoHTML = texto1; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  textoHTML += analogRead(A0); //A SAÍDA DA PORTA A0 IRÁ APARECER NO TEXTO DA HTML
  textoHTML += ligar; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  textoHTML += checagem; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
   
  server.send(200, "text/html", textoHTML); //FUNÇÃO QUE ENVIA A PÁGINA HTML COM AS INFORMAÇÕES DEFINIDAS
  digitalWrite(led, 0); //QUANDO ACABAR A ROTINA DESLIGAR O LED
}

void handleNotFound(){ //FUNÇÃO EM CASO DE PROBLEMAS NA PÁGINA HTML
  digitalWrite(led, 1); //LIGAR O LED DO ESP 8266 NO INICIO DA ROTINA
  String message = "File Not Found\n\n"; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  message += "URI: "; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  message += server.uri(); //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  message += "\nMethod: "; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  message += (server.method() == HTTP_GET)?"GET":"POST"; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  message += "\nArguments: "; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  message += server.args(); //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  message += "\n"; //ADICIONA VARIÁVEIS PARA O TEXTO QUE IRÁ APARECER NA HTML
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message); //FUNÇÃO QUE ENVIA A PÁGINA HTML COM AS INFORMAÇÕES DEFINIDAS
  digitalWrite(led, 0); //DESLIGAR O LED DO ESP 8266 NO INICIO DA ROTINA
}

void setup(void){
  pinMode(lampada, OUTPUT); //DEFINE A LÂMPADA COMO SAÍDA
  pinMode(led, OUTPUT); //DEFINE O LED DO ESP 8266 COMO SAÍDA
  digitalWrite(led, 0); //DEFINE O LED DO ESP 8266 COMO DESLIGADO INICIALMENTE
  Serial.begin(115200); //DEFINE A VELOCIDADE DO MONITOR SERIAL
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //EXECUTA O USUÁRIO E SENHA DA REDE
  Serial.println(""); //QUEBRA DE LINHA NO MONITOR SERIAL

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) { //ENQUANTO NÃO CONECTA NO WIFI
    delay(500); //A CADA MEIO SEGUNDO
    Serial.print("."); //IMPRIME NO MONITOR SERIAL
  }
  Serial.println(""); //QUEBRA DE LINHA NO MONITOR SERIAL
  Serial.print("Connected to "); //QUANDO CONECTAR IMPRIME NO MONITOR SERIAL
  Serial.println(ssid); //QUANDO CONECTAR IMPRIME NO MONITOR SERIAL O USUÁRIO DA REDE
  Serial.print("IP address: "); //QUANDO CONECTAR IMPRIME NO MONITOR SERIAL
  Serial.println(WiFi.localIP()); //QUANDO CONECTAR IMPRIME NO MONITOR SERIAL O IP DE CONEXÃO NA WEB

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started"); //IMPRIME NO MONITOR SERIAL
  }

  server.on("/", handleRoot); //MANTÉM O WEB SERVER ONLINE

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well"); //IMPRIME, QUANDO O WEB SERVER ESTIVER FUNCIONANDO
  });

  server.onNotFound(handleNotFound); //CASO DE ERRO NO WEB SERVER

  server.begin(); //MANTÉM O SERVER ATIVO
  Serial.println("HTTP server started"); //IMPRIME NO MONITOR SERIAL

  if(! rtc.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ
  while(1); //SEMPRE ENTRE NO LOOP
  }
  now = rtc.now(); //CHAMADA DE FUNÇÃO
  
  //REMOVA O COMENTÁRIO DA LINHA ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO 
}

void loop(void){
  server.handleClient(); //PREPARA, EM LOOP, PARA RECEBER SOLICITAÇÕES DA PÁGINA HTML

  now = rtc.now();
  horaAtual = now.hour(); //CAPTURA A HORA ATUAL DENTRO DESTA VARIÁVEL, SENDO CONSTANTEMENTE ATUALIZADA
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
