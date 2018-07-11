#include <SPI.h>
#include <Ethernet.h>
String readString;

int x = 0, y = 0;
int a = 0, b = 0, v = 0;
const int ledPin = 13; // Pino ao qual o LED será conectado
// LEMBRE DE ALTERAR ESSA VARIAVE TAMBEM NO PIC

byte mac[] = { 0x48, 0xC2, 0xA1, 0xF3, 0x8D, 0xB7 };
EthernetServer server(80);

void setup() {
  Serial.begin(9600);              //Inicializa a comunicação serial com o TX0 e RX0
  Serial1.begin(9600);             //Inicializa a comunicação serial com o TX1 e RX1
  pinMode(ledPin, OUTPUT); // Pino 13 será de saída de sinais

  if (Ethernet.begin(mac) == 0) {
    //Serial.println("Failed to configure Ethernet using DHCP");
    for (;;);
  }
  printIPAddress();                                           //Imprime o IP alocado para o arduino na rede
  server.begin();
}

void loop()
{
  char d;
  digitalWrite(ledPin, HIGH);

  EthernetClient client = server.available();                 //Inicia um servidor no IP mostrado do Serial
  if (client) {
    while (client.connected())                                //Verifica se algum cliente está conectado
    {
      if (client.available())                                 //Verifica se o cliente enviou algum parametro para o servidor
      {
        char c = client.read();                               //Le cada caracter do parametro
        if (readString.length() < 100) {
          readString += c;                                    //Armazena a String enviada na variavel readString
        }
        if (c == '\n')                                       
        {
          Serial.println(readString);

          if (readString.indexOf("?ligar") > 0)              //Se a String enviada estiver a string '?ligar' o arduino enviará o compando para acionar o componente correspondente
          {
            x = readString.substring(13, 14).toInt();        //Lê o número do componente mandado no parametro 
            if(x == 1) d = 'A';
            if(x == 2) d = 'B';
            if(x == 3) d = 'C';
            //Serial.println(d);
            
            y = controlaRelay('L',d);                        //Envia para a função contralaRelay o parametro 'L' indicando que deve desligar o componente e a variável 'd' que corresponde aos caracteres 'A', 'B', ou 'C'
            if(y == 1 && x == 1) a = 0;                      //Modifica o estado do botão 
            if(y == 1 && x == 2) b = 0;
            if(y == 1 && x == 3) v = 0;
          }
          else if (readString.indexOf("?desligar") > 0)       //Se a String enviada estiver a string '?desligar' o arduino enviará o compando para desabilitar o componente correspondente
          {
            x = readString.substring(16, 17).toInt();         //Lê o número do componente mandado no parametro 
            if(x == 1) d = 'A';                               //
            if(x == 2) d = 'B';
            if(x == 3) d = 'C';
            //Serial.println(d);
            
            y = controlaRelay('H',d);                          //Envia para a função contralaRelay o parametro 'H' indicando que deve ligar o componente e a variável 'd' que corresponde aos caracteres 'A', 'B', ou 'C'
            if(y == 1 && x == 1) a = 1;                        //Modifica o estado do botão
            if(y == 1 && x == 2) b = 1;
            if(y == 1 && x == 3) v = 1;
          }

          readString = "";                  //Envia para o navegador a página HTML

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.print("<!DOCTYPE html><html lang='pt-br'><head>    <meta charset='UTF-8'>    <title>Taveira's House</title>    <link rel='stylesheet' type='text/css' href='https://cdn.rawgit.com/denistaveira/MulitDeviceControl/6a38d3f0/styles.css'></head><body><H1 align='center'>Taveira's House</H1><div id='botao_01'></div><div id='estado_01' style='visibility: hidden;'>");
          client.print(a);                  //Modifica o estado do botão caso a variável mude o valor (0 ou 1)
          client.print("</div><div id='botao_02'></div><div id='estado_02' style='visibility: hidden;'>");
          client.print(b);
          client.print("</div><div id='botao_03'></div><div id='estado_03' style='visibility: hidden;'>");
          client.print(v);
          client.print("</div><script src='https://cdn.rawgit.com/denistaveira/MulitDeviceControl/b03f5c8f/controle.js' charset='utf-8'></script></body></html>");
          delay(1);
          client.stop();
        }
      }
    }
  }
}

int controlaRelay (char acao, char relay) {                     //Acao 'H' ou 'L' e  relay 'A', 'B' ou 'C'
  //if (Control == 1) {
    
    Serial1.write(acao);                                          // Envia 'H' pela comunicação serial para o PIC quando desejar acionar um componente e envia 'L' quando desejar desligar um componente 
    delay(500);
    Serial1.write(relay);                                         // Envia o caracter correspondente ao componente que se deseja realizar a ação ('A' - 1 | 'B' - 2 | 'C' - 3)
    
    if (Serial1.available()) {                                    // Verificar se há caracteres disponíveis
      char caractere = Serial1.read();                            // Armazena caracter lido caso '1', '2' ou '3' a ação foi executada no componente correspondente, caso '0' ocorreu um erro ao executar a ação.
      if (caractere == '1' && acao == 'H') {                      //Imprime a mensagem no serial do arduino que rele correspondente foi acesso ou apagado, de acordo com a ação enviada pela comunicação serial com o PIC
        Serial.println("Rele 1 acesso");
        return 1;
      }
      if (caractere == '2' && acao == 'H') {
        Serial.println("Rele 2 acesso");
        return 1;
      }
      if (caractere == '3' && acao == 'H') {
        Serial.println("Rele 3 acesso");
        return 1;
      }
      if (caractere == '1' && acao == 'L') {
        Serial.println("Rele 1 apagado");
        return 1;
      }
      if (caractere == '2' && acao == 'L') {
        Serial.println("Rele 2 apagado");
        return 1;
      }
      if (caractere == '3' && acao == 'L') {
        Serial.println("Rele 3 apagado");
        return 1;
      }
      if (caractere == '0') {                               //Imprime no serial que ocorreu um erro ao executar a ação
        Serial.println("Erro ao apagar");
        //Serial.println(caractere);
        return 0;
      }
    }
  //}
}
void printIPAddress()
{
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
}


