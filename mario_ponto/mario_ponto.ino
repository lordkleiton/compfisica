//bibliotecas a serem usadas
#include <LiquidCrystal.h>
#include <Keypad.h>
#include "SPI.h"
#include "MFRC522.h"

//configuração do RFID
const int pinRST = 8;                                     //pinagem de dados do rfid
const int pinSDA = 53;                                    //pinagem de dados do rfid
MFRC522 mfrc522(pinSDA, pinRST);                          //configura o rfid

//configuração do teclado
const byte ROWS = 4;                                      //quantidade de linhas
const byte COLS = 3;                                      //quantidade de colunas

char hexaKeys[ROWS][COLS] = {                             //identificação de cada tecla
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {20, 19, 18, 17};                    //define as linhas do teclado
byte colPins[COLS] = {16, 15, 14};                        //define as colunas do teclado

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //configura o teclado

//configuração senha teclado
#define Password_Length 5                                 //tamanho da senha

char Data[Password_Length]; 
char Master[Password_Length] = "1234";                 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

//configuração LCD
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //pinos do lcd
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                //configuração do lcd

//inicializa tudo
void setup() {
  //rfid
  SPI.begin();
  mfrc522.PCD_Init();

  //porta serial
  Serial.begin(9600);

  //lcd
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);                            //posiciona o cursor
  lcd.print("Aproxime o");                        //escreve na tela
  lcd.setCursor(0, 1);                            //posiciona a tela
  lcd.print("seu cartao");                        //escreve na tela

  //leds
  pinMode(13, OUTPUT);                    //vermelho
  pinMode(12, OUTPUT);                    //verde
  pinMode(11, OUTPUT);                    //azul

  //comunicação com o processing
  contato();
}

//variáveis auxiliares
char val;                                 //auxiliar que contem os dados recebidos do processing

int vermelho = 13;                        //pino led vermelho
boolean evermelho = HIGH;                 //estado do led vermelho

int verde = 12;                           //pino led verde
boolean everde = HIGH;                    //estado do led verde

int azul = 11;                            //pino do led azul
boolean eazul = HIGH;                     //estado do led azul

//roda tudo em loop
void loop() {

  teclado();
  executaRFID();

}

void executaRFID(){
  digitalWrite(azul, eazul);              //acende o led azul
  
  if (Serial.available() > 0) {           //verifica se tem alguma coisa a ser lida na porta serial
    val = Serial.read();                  //lê a porta serial caso sim

    if (val == '0'){                      //se recebe 0, executa o codigo de acerto
      digitalWrite(azul, !eazul);
      digitalWrite(verde, everde);
      lcd.clear();
      lcd.setCursor(0, 0);                            
      lcd.print("Entrada aceita");                        
      lcd.setCursor(0, 1);                           
      lcd.print("Seja bem vindo");
      delay(1500);
      digitalWrite(verde, !everde);
    }

    if (val == '1'){                      //se recebe 1, executa o codigo de erro
      digitalWrite(azul, !eazul);
      digitalWrite(vermelho, evermelho);
      lcd.clear();
      lcd.setCursor(0, 0);                            
      lcd.print("Entrada recusada");                        
      lcd.setCursor(0, 1);                           
      lcd.print("Nao cadastrado");
      delay(1500);
      digitalWrite(vermelho, !evermelho);
    }
  }
  else {                                  //caso não tenha nada a ser lido na serial
    leituraRFID();                        //lê o rfid
  }
}

//recebe o "ok" inicial do processing, dando início a tudo
void contato() {
  while (Serial.available() <= 0) {
    Serial.println("A");                  //processing espera esse "a" pra começar tudo
    delay(300);
  }
}

char auxTeclado;

//faz as magias do teclado
void teclado() {
  char customKey = customKeypad.getKey();
  
  if (customKey) {
      lcd.setCursor(0, 0);                            //posiciona o cursor
      lcd.print("Digite sua senha");                        //escreve na tela
      Data[data_count] = customKey; 
      lcd.setCursor(data_count,1); 
      lcd.print(Data[data_count]); 
      lcd.setCursor(data_count+1,1); 
      lcd.print("          "); 
      data_count++; 
  }

  if(data_count == Password_Length-1){
    lcd.clear();
    if(!strcmp(Data, Master)){
      String str((char*)Data);
      Serial.println(str);
    }
    else{
      String str((char*)Data);
      Serial.println(str);
    }
    clearData();  
  }
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}


//faz as magias do rfid
void leituraRFID() {
  if (mfrc522.PICC_IsNewCardPresent()) {            //se tem um cartão na área do rfid
    delay(500);                                     //demora meio segundo pra processar o cartão
                                                    //todos os delays aqui são exclusivamente
                                                    //de forma a evitar que a porta serial
                                                    //sofra spam 
    clearData();
                                          
    if (mfrc522.PICC_ReadCardSerial()) {            //lê o cartão
      digitalWrite(azul, !eazul);                   //apaga o led azul
      lcd.clear();                                  //limpa a tela lcd
      lcd.setCursor(0, 0);                          //posiciona o cursor
      lcd.print("Fazendo leitura");                 //escreve na tela
      Serial.print("RFID TAG ID:");                 //escreve na serial
      lcd.setCursor(0, 1);                          //posiciona o cursor
      lcd.print("Nao tire cartao");
      for (byte i = 0; i < mfrc522.uid.size; ++i){  //lê o id do cartão em partes
        Serial.print(mfrc522.uid.uidByte[i], HEX);  //imprime o resultado em hexadecimal
        Serial.print(" ");                          //melhora a legibilidade
        //lcd.print(mfrc522.uid.uidByte[i], HEX);     //escreve na tela
        //lcd.print(" ");                             //escreve na tela
      }
      Serial.println();                             //escreve na serial
      delay(1000);                                  //demora mais um segundo
    }
  }
  else {                                            //caso não
    if (data_count == 0){
      lcd.clear();                                    //limpa a tela
      lcd.setCursor(0, 0);                            //posiciona o cursor
      lcd.print("Aproxime o");                        //escreve na tela
      lcd.setCursor(0, 1);                            //posiciona a tela
      lcd.print("seu cartao");                        //escreve na tela
    }
  }
}


