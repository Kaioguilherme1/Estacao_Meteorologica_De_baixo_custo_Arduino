/*  Titulo: Programa de estação meteorologica baseada em arduino
    Auth: kaio guilherme Ferraz de sousa silva
    Data: 20/10/2020
*/
#include "DHT.h"
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

//define pinos analogicos
#define ldrP    A0 //LDR no pino analígico 0
#define DHTPIN  A1 //DHT no pino analogico 1
#define sdcP    A2 //sdc no pino analogico 2

//difine pinos digitais
#define botaoM  25 //botão memory pino digital 25
#define ledaP   22 //led azul pino digital 22
#define ledAP   24 //led vermelho pino digital 24   
#define pinoSS  53 //pino 53 para Mega/ pino 10 para uno
#define DHTTYPE DHT11 // DHT 11

// posição dos pinos na placa de cada periferico
// sensor de luz (LDR)                A0           fio branco
// sensor temperatura                 A1           dados
// sensor de chuva                    A2           dados A0
// botão da SD                        D25          terminal de contato
// led verde                          D4           positivo
// led vermelho                       D5           positivo
// sd card pinos                      D50          MISO
// sd card pinos                      D51          MOSI
// sd card pinos                      D52          SCK
// sd card pinos                      D53          CS

DHT dht(DHTPIN, DHTTYPE);

File myFile;
//define variaaves
//numero com casas decimais
float temperatura = 0;
float umidade     = 0;
float Lux         = 0;
float Tmax        = 0;
float Tmed        = 0;
float Tmin        = 50;
//texto
String ErroA;
String Data;
String Hora;
String direcao;
String chuvaE;
String nome;
String Cinicio = "00:00:00" ;
String Cfim    = "00:00:00" ;
String so;
String HI      = "++++++++";
//numeor inteiros
int tempo     = 0;
int segundo   = 0;
int minuto    = 0;
int hora      = 0;
int dia       = 0;
int mes       = 0;
int ano       = 0;
int ldrValor  = 0;  //Valor lido do LDr
int tela      = 1;  //define tela inicial
int Ti        = 0;  //tempo de irrigação em minutos
int sdc       = 0;
int us        = 0;
int td        = 0;
int I         = 0; // define numeros de vezes irrigados no dia como parametro
//bytes
byte btT       = 0; //difine posição do botão A
byte btM       = 0; //difine posição do botão B
byte btI       = 0; //difine posição do botão C
int sd        = 0; //define se msg do sd card ja foi mostrada
//condições
boolean  Display = true;
boolean  irrigar = false;
boolean  erro    = false;
boolean  chuva   = false;
boolean  bCW;    //VARIÁVEL DE CONTROLE DO SENTIDO (HORÁRIO / ANTI-HORÁRIO)
boolean  sdcard  = false;
unsigned long UtlTime;
boolean G        = false;
boolean in       = false;

void setup() {
  Serial.begin(9600);
  Serial.println("iniciando programa");
  dht.begin();

  //define pinos Digitais
  pinMode(botaoM , INPUT);       //Definindo pino digital 3 como de entrada do sinal.
  pinMode(ledaP  , OUTPUT);      //define o pno digital 5 para o led verde como saida
  pinMode(ledAP  , OUTPUT);      //define o pno digital 6 para o led vermelho como saida
  pinMode(pinoSS , OUTPUT);       // Declara pinoSS como saída
  //define pinos analogicos
  pinMode(ldrP , INPUT);         //Define pino analogico 0 como entrada de dados
  pinMode(sdcP , INPUT);         //Define pino analogico 2 como entrada de dados
  //zera as variaveis do relogio
  UtlTime = 0;
  delay(200);
  //verificação do cartão sd
  if (SD.begin()) { // Inicializa o SD Card
    Serial.println("SD Card pronto para uso."); // Imprime na tela
    delay(500);
  }

  else {
    Serial.println("SD Card não encotrado");
    return;
  }



  //pagina de configuração do relogio é timer pelo monitor Serial

  Serial.print("\nEntre Minuto >>> ");
  while (minuto == 0)
  {
    if (Serial.available() > 0)
    {

      minuto = Serial.parseInt();

    }
  }

  Serial.print("\nEntre Hora >>> ");
  while (hora == 0)
  {
    if (Serial.available() > 0)
    {

      hora = Serial.parseInt();

    }
  }

  Serial.print("\nEntre Dia >>> ");
  while (dia == 0)
  {
    if (Serial.available() > 0)
    {

      dia = Serial.parseInt();

    }
  }
  Serial.print("\nEntre Mes >>> ");
  while (mes == 0)
  {
    if (Serial.available() > 0)
    {

      mes = Serial.parseInt();

    }
  }
  Serial.print("\nEntre ano >>> ");
  while (ano == 0)
  {
    if (Serial.available() > 0)
    {

      ano = Serial.parseInt();

    }
  }


  Serial.println("carregando programa");
  delay(2500);
  Serial.println("=========================================================================================================================================================");
  Serial.println("| temperatura atual | Temp max | Temp min | Temp med | umidade relativa | Luminosidade | chuva |  inicio  |  Fim    |   Hora   | tempo de funcionamento |");
  Serial.println("=========================================================================================================================================================");

}

void loop() {


  //inicia o ledverde de funcionamento a cada 10 segundos
  if (segundo == 10 || segundo == 20 || segundo == 30 || segundo == 40 || segundo == 50 || segundo == 15 || segundo == 25 || segundo == 35 || segundo == 45 || segundo == 55 && erro == false) {
    //acende o led pelo terminal digital 4
    digitalWrite(ledaP, HIGH);

  } else {
    digitalWrite(ledaP, LOW);


  }

  //leitura dos botões
  btM = digitalRead(botaoM);
  //leitura dos sensores
  ldrValor = analogRead(ldrP);         // leitura da luz
  umidade = dht.readHumidity();        // leitura da umidade
  temperatura = dht.readTemperature(); // leitura da temperatura
  sdc = analogRead(sdcP);              // leitura de chuva
  // testa se a erro no DHT.
  if (isnan(temperatura) || isnan(umidade))
  {
    //Serial.println("erro na leitura de temperatura");
    ErroA = ("sensor temp/umd");
    //Serial.println(tela);
    erro = true;
  }
  //fução de temperatura
  if (temperatura > Tmax) {
    Tmax = temperatura;
  } else {
    if (temperatura < Tmin) {
      Tmin = temperatura;
    }
  }

  //função de zera temperatura max e min no final do dia
  if (hora == 23 && minuto == 59 && segundo == 59) {
    Tmax = temperatura;
    Tmed = temperatura;
    Tmin = temperatura;
  }
  //funões de conversão
  //temperatura media
  Tmed = (Tmax + Tmin) / 2;
  //conversor para lux do ldr
  Lux = 10 - 0.009775171065 * ldrValor;
  //algoritimo de hora de chuva
  Serial.print(sdc);
  if (sdc < 450) {
    chuva = true;
    chuvaE = "SIM";
  } else {
    if (sdc > 450) {
      chuva = false;
      chuvaE = "NAO";
    }
  }
  if (chuva == true && in == false) {
    Cinicio = Hora;
    in = true;
  } else {
    if (chuva == false && in == true ) {
      Cfim = Hora;
      in = false;

    }

  }




  //inicia a contagem das horas
  if (millis() - UtlTime < 0)
  {
    UtlTime = millis();
  } else
  {
    segundo = int((millis() - UtlTime) / 1000);
  }
  if (segundo > 59)
  {
    segundo = 0;
    minuto++;
    UtlTime = millis();

    if (minuto > 59)
    {
      hora++;
      minuto = 0;
      if (hora > 23)
      {
        dia++;
        hora = 0;
        //inicia a contagem dos dias
        if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12)
        {
          if (dia > 31)
          {
            dia = 1;
            mes++;
            if (mes > 12)
            {
              ano++;
              mes = 1;
            }
          }
        }
        else if (mes == 2)
        {
          if (ano % 400 == 0)
          {
            if (dia > 29)
            {
              dia = 1;
              mes++;
            }

          }
          else if ((ano % 4 == 0) && (ano % 100 != 0))
          {
            if (dia > 29)
            {
              dia = 1;
              mes++;
            }

          }
          else
          {
            if (dia > 28)
            {
              dia = 1;
              mes++;
            }
          }
        }
        else
        {
          if (dia > 30)
          {
            dia = 1;
            mes++;
          }
        }
      }
    }
  }


  //guarda os dados em variaves em formato de texto
  Data =  (dia);
  Data += ("/");
  Data += (mes);
  Data += ("/");
  Data += (ano);

  Hora =  (hora);
  Hora += (":");
  Hora += (minuto);
  Hora += (":");
  Hora += (segundo);
  Hora += ("\n");
  //Verifica sé o sdcard está plugado constantemente
  if (SD.begin()) { // Inicializa o SD Card
    Serial.println("SD Card pronto para uso."); // Imprime na tela
  }

  else {
    Serial.println("Falha na inicialização do SD Card.");
    return;
  }

  //marca uma nova tabela com data e hora
  if (hora == 23 && minuto == 59 && segundo == 59) {
    myFile = SD.open("d1.txt", FILE_WRITE); // Cria / Abre arquivo .txt
    myFile.println(Data);
    myFile.println("==========================================================================================================================================================");
    myFile.println("|| temperatura atual | Temp max | Temp min | Temp med | umidade relativa | Luminosidade | chuva |  inicio  |  Fim    |   Hora   | tempo de funcionamento |");
    myFile.println("==========================================================================================================================================================");
    myFile.close(); // Fecha o Arquivo após escrever
  }

  if (minuto == 59 && segundo == 59 || btM == true) {
    digitalWrite(ledAP, HIGH);
    td ++;
    myFile = SD.open("d1.txt", FILE_WRITE); // Cria / Abre arquivo .txt
    if (myFile) { // Se o Arquivo abrir imprime:
      digitalWrite(ledAP, HIGH);
      Serial.println("Gravando dados"); // Imprime na tela
      myFile.print("|     ");
      myFile.print(temperatura);
      myFile.print(" °C      | ");
      myFile.print(Tmax);
      myFile.print(" °C | ");
      myFile.print(Tmin);
      myFile.print(" °C | ");
      myFile.print(Tmed);
      myFile.print(" °C |      ");
      myFile.print(umidade);
      myFile.print(" %     |    ");
      myFile.print(Lux);
      myFile.print(" lux   | ");
      myFile.print(chuvaE);
      myFile.print("  |  ");
      myFile.print(Cinicio);
      myFile.print(" | ");
      myFile.print(Cfim);
      myFile.print(" | ");
      myFile.print(Hora);
      myFile.print(" |          ");
      myFile.print(td);
      myFile.println(" horas   |");
      myFile.close(); // Fecha o Arquivo após escrever
      Serial.println("Terminado."); // Imprime na tela
      Serial.println(" ");

    }

    else {
      Serial.println("Erro ao Abrir Arquivo .txt"); // Imprime na tela
    }
    myFile = SD.open("d1.txt"); // Abre o Arquivo

    if (myFile) {
      Serial.println("Conteúdo do Arquivo:"); // Imprime na tela

      while (myFile.available()) { // Exibe o conteúdo do Arquivo
        Serial.write(myFile.read());
      }

      myFile.close(); // Fecha o Arquivo após ler
      

    }
  } else {     // Se o Arquivo não abrir
    Serial.println("Erro ao Abrir DADO .txt"); // Imprime na tela
  }
  digitalWrite(ledAP, LOW);




  //retorna dados pro monitor serial
  Serial.print("|     ");
  Serial.print(temperatura);
  Serial.print(" °C      | ");
  Serial.print(Tmax);
  Serial.print(" °C | ");
  Serial.print(Tmin);
  Serial.print(" °C | ");
  Serial.print(Tmed);
  Serial.print(" °C |      ");
  Serial.print(umidade);
  Serial.print(" %     |    ");
  Serial.print(Lux);
  Serial.print(" lux   | ");
  Serial.print(chuvaE);
  Serial.print("  |  ");
  Serial.print(Cinicio);
  Serial.print(" | ");
  Serial.print(Cfim);
  Serial.print(" | ");
  Serial.print(Hora);
  Serial.print(" |          ");
  Serial.print(td);
  Serial.println(" horas   |");


}
