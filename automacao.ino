//BIBLIOTECA PARA GRAVAÇAO NA EEPROM
#include <EEPROM.h>

//BIBLIOTECAS PARA O FUNCIONAMENTO DO IR.
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


//BIBLIOTECA PARA A COMUNICACAO I2C.
#include <Wire.h>


//BIBLIOTECAS PARA COMUNICACAO WIFI.
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


//DEFINES. 
#define qtdeCi  1
#define BLYNK_PRINT Serial

#define IN_0    9
#define IN_1    10
#define IN_2    0
#define A_0     A0

#define OUT_0   7
#define OUT_1   6
#define OUT_2   0
#define OUT_3   1
#define OUT_4   2
#define OUT_5   3
#define OUT_6   D5
#define OUT_7   D4
#define OUT_8   D6
#define OUT_9   D7
#define OUT_10  D8
#define OUT_11  5
#define OUT_12  4


//VARIAVEL DO RECEPTOR IR.
const uint16_t Pino_Receptor = IN_2;


//VARIAVEIS PARA AUTENTICACAO DO WIFI.
char auth[] = "chave de autenticaçao do blynk";
char ssid[] = "nome do wifi";
char pass[] = "senha do wifi";


//VARIAVEIS DO EXPANSOR DE PORTAS.
byte enderecosPCF8574[qtdeCi] = {32};                
bool ciPCF8574PinMode(byte pino, int modo = -1);
void ciPCF8574Write(byte pino, bool estado);
bool ciPCF8574Read(byte pino);


//VARIAVEIS DO PROGRAMA.
bool Luz_botao = 0;
bool Luz_wifi = 0;
bool Luz_IR = 0;
bool Borda_Luz = 0;
bool Estado_Lampada = 0;

unsigned long Delay_tranca = 0;
bool Destrancar_wifi = 0;
bool Trancar_wifi = 0;
bool Tranca_IR = 0;
bool Estado_tranca = 0;
bool set_tranca = 0;
bool Borda_tranca = 0;
bool Trancar = 0;
bool Destrancar = 0;

unsigned long Delay_C1 = 0;
bool Subir_C1_wifi = 0;
bool Descer_C1_wifi = 0;
bool Subir_C1_IR = 0;
bool Descer_C1_IR = 0;
bool Estado_C1 = 0;
bool set_C1 = 0;
bool Borda_C1 = 0;
bool Subir_C1 = 0;
bool Descer_C1 = 0;
bool Regula_C1 = 0;
bool Borda_regula_C1 = 0;
int Tempo_subida_C1 = 3120;
int Tempo_descida_C1 = 3490;

unsigned long Delay_C2 = 0;
bool Subir_C2_wifi = 0;
bool Descer_C2_wifi = 0;
bool Subir_C2_IR = 0;
bool Descer_C2_IR = 0;
bool Estado_C2 = 0;
bool set_C2 = 0;
bool Borda_C2 = 0;
bool Subir_C2 = 0;
bool Descer_C2 = 0;
bool Regula_C2 = 0;
bool Borda_regula_C2 = 0;
int Tempo_subida_C2 = 3900;
int Tempo_descida_C2 = 2800;

bool Emergencia_cortinas = 0;
bool Cortinas_ok = 0;
bool Set_emergencia_cortinas = 0;

unsigned long Delay_Ventilador = 0;
bool Timer_Ventilador = 0;
bool Ventilador_wifi = 0;
bool Ventilador_IR = 0;
bool Borda_Ventilador = 0;
bool Set_Ventilador = 0;
bool Estado_Timer_Ventilador = 0;
bool Ventilador_OFF = 0;
int Tempo_Set_Ventilador = 200;


//OBJETOS PARA COMUNICACAO IR.
IRrecv Receptor_IR(Pino_Receptor);
decode_results Resultado;


//RECEBE OS COMANDOS VIA WIFI.
BLYNK_WRITE(V0) 
{
  Luz_wifi = param.asInt();
}

BLYNK_WRITE(V1)
{
  Destrancar_wifi = param.asInt();
}

BLYNK_WRITE(V2) 
{
  Trancar_wifi = param.asInt();
}

BLYNK_WRITE(V3)
{
  Subir_C1_wifi = param.asInt();
}

BLYNK_WRITE(V4) 
{
  Descer_C1_wifi = param.asInt();
}

BLYNK_WRITE(V5) 
{
  Subir_C2_wifi = param.asInt();
}

BLYNK_WRITE(V6) 
{
  Descer_C2_wifi = param.asInt();
}

BLYNK_WRITE(V7)
{
  Ventilador_wifi = param.asInt();
}

BLYNK_WRITE(V8)
{
  Timer_Ventilador = param.asInt();
}
 
BLYNK_WRITE(V9) 
{
  Estado_Timer_Ventilador = param.asInt();
}


void setup() 
{

  //DEFINE VELOCIDADE DE LEITURA SERIAL.  
  Serial.begin(9600);

  //INICIA O RECEPTOR.   
  Receptor_IR.enableIRIn();

  //ESPERA ATE A CONEXAO SERIAL ESTAR ESTAVEL.
  while (!Serial)
  delay(50);

  //INFORMA O STATUS DO RECEPTOR.
  Serial.println();
  Serial.println("O Receptor está em execução e aguardando um código IR");

  //INICIA COMUNICACAO I2C.
  Wire.begin();

  //LÊ O ESTADO DAS CORTINAS.
  EEPROM.begin(4);
  Estado_C1 = EEPROM.read(0);
  Estado_C2 = EEPROM.read(1);
  set_C1 = Estado_C1;
  set_C2 = Estado_C2;
  EEPROM.end();

  //DEFINE ENTRADAS E SAIDAS.   
  pinMode(IN_0,  INPUT);
  pinMode(IN_1,  INPUT);
  pinMode(IN_2,  INPUT);
  pinMode(A_0,   INPUT);
    
  pinMode(OUT_6,  OUTPUT);
  pinMode(OUT_7,  OUTPUT);
  pinMode(OUT_8,  OUTPUT);
  pinMode(OUT_9,  OUTPUT);
  pinMode(OUT_10, OUTPUT);

  ciPCF8574PinMode(OUT_0,   OUTPUT);
  ciPCF8574PinMode(OUT_1,   OUTPUT);
  ciPCF8574PinMode(OUT_2,   OUTPUT);
  ciPCF8574PinMode(OUT_3,   OUTPUT);
  ciPCF8574PinMode(OUT_4,   OUTPUT);
  ciPCF8574PinMode(OUT_5,   OUTPUT);
  ciPCF8574PinMode(OUT_11,  OUTPUT);
  ciPCF8574PinMode(OUT_12,  OUTPUT);

  //CORRIGE BUG DO EXPANSOR DE PORTAS
  ciPCF8574Write(OUT_0, 1);
  ciPCF8574Write(OUT_1, 1);
  ciPCF8574Write(OUT_2, 1);
  ciPCF8574Write(OUT_3, 1);
  ciPCF8574Write(OUT_4, 1);
  ciPCF8574Write(OUT_5, 1);
  ciPCF8574Write(OUT_11, 1);
  ciPCF8574Write(OUT_12, 1);

  //AUTENTICACAO WIFI
  Blynk.begin(auth, ssid, pass);
}
 
void loop() 
{ 
 //INICIA RECEBIMENTOS DE COMANDO POR WIFI.
 Blynk.run();

 //INICIA RECEBIMENTOS DE COMANDO POR IR E MOSTRA OS CODIGOS RECEBIDOS EM HEXADECIMAL. 
 //OBS: print() e println() não funaciona com a impressão de variaveis grandes. (uint64_t)
 if (Receptor_IR.decode(&Resultado)) 
 { 
    serialPrintUint64(Resultado.value, HEX);
    Serial.println("");

     //VERIFICA OS CODIGOS RECEBIDOS E ALTERA O VALOR DAS VARIAVEIS.
     switch (Resultado.value)
     {
        case (0x4FB8877): 
          Luz_IR = 1;
          break;
            
        case (0x4FB609F): 
          Tranca_IR = 1;
          break;

        case (0x4FB48B7): 
          Subir_C1_IR = 1;
          break;

        case (0x4FB50AF): 
          Descer_C1_IR = 1;
          break;

          
        case (0x4FBA857): 
          Regula_C1 = 1;
          break;

        case (0x4FB6897): 
          Subir_C2_IR = 1;
          break;

        case (0x4FB58A7): 
          Descer_C2_IR = 1;
          break;

          
        case (0x4FBD02F): 
          Regula_C2 = 1;
          break;
          
        case (0x4FB906F): 
          Emergencia_cortinas = 1;
          break;

        case (0x4FB10EF): 
          Cortinas_ok = 1;
          break;

        case (0x4FB20DF): 
          Ventilador_IR = 1;
          break;
      
      default:
      {
        Luz_IR = 0;
        Tranca_IR = 0;
        Descer_C1_IR = 0;
        Subir_C1_IR = 0;
        Regula_C1 = 0;
        Descer_C2_IR = 0;
        Subir_C2_IR = 0;
        Regula_C2 = 0;
        Emergencia_cortinas = 0;
        Cortinas_ok = 0;
        Ventilador_IR = 0;
      }
      break;
    }

    //RECEBE O PROXIMO CODIGO. 
    Receptor_IR.resume();
  }

  //Ventilador
  if(Borda_Ventilador == 0)
  {
    if(Ventilador_wifi || Ventilador_IR || Ventilador_OFF )
    {
      if(Set_Ventilador == 0)
      {
        Delay_Ventilador = millis();
      } 
        Set_Ventilador = 1;
        Borda_Ventilador = 1;
    }
  }

  else if(Ventilador_wifi == 0 && Ventilador_IR == 0 && Ventilador_OFF == 0)
  {
    Borda_Ventilador = 0;
  }
       
  if(millis() - Delay_Ventilador >= Tempo_Set_Ventilador) 
  { 
    Set_Ventilador = 0;
  }

  if( Estado_Timer_Ventilador && Timer_Ventilador)
  {
    Ventilador_OFF = 1;
  }

  else
  {
    Ventilador_OFF = 0;
  }
  //Controle das saidas.  
  digitalWrite(OUT_10, Set_Ventilador);

  //LUZ
  if(Borda_Luz == 0)
  {
    if(Luz_wifi || Luz_IR || Luz_botao) 
    {
      Estado_Lampada = !Estado_Lampada;
      Borda_Luz = 1;
    }
  }
  else if(Luz_wifi == 0 && Luz_IR == 0 && Luz_botao == 0)
    Borda_Luz = 0;
  //Controle das saidas.  
  Luz_botao = digitalRead(IN_0);
  digitalWrite(OUT_6, Estado_Lampada);

  //TRANCA
  if(Trancar == 0 && Destrancar == 0)
  {
    if(Borda_tranca == 0)
    {
      if((Destrancar_wifi || Tranca_IR)&& Estado_tranca) 
      {
        Estado_tranca = 0;
        Borda_tranca = 1;
      }
    }
 
    if(Borda_tranca == 0)
    {
      if((Trancar_wifi || Tranca_IR) && Estado_tranca == 0) 
      {
        Estado_tranca = 1;
        Borda_tranca = 1;
      }
    } 
 
    if(Destrancar_wifi == 0 && Trancar_wifi == 0 && Tranca_IR == 0)
      Borda_tranca = 0;
  }

  if(Estado_tranca && set_tranca == 0)
  {
    if(Trancar == 0)
    {
      Delay_tranca = millis();
    }

    Trancar = 1;
       
    if(millis() - Delay_tranca >= 300)  
      { 
        Trancar = 0;
        set_tranca = 1;
      }
  }
  
  if(Estado_tranca == 0 && set_tranca)
  {
    if(Destrancar == 0)
    {
      Delay_tranca = millis();
    }  
    
    Destrancar = 1;
    
    if(millis() - Delay_tranca >= 300)  
    { 
      Destrancar = 0;
      set_tranca = 0;
    }
  }
  //Controle das saidas. 
  ciPCF8574Write(OUT_11, !Trancar);
  ciPCF8574Write(OUT_12, !Destrancar);

  //Cortina 1
  if(Descer_C1 == 0 && Subir_C1 == 0)
  {
    if(Borda_C1 == 0)
    {
      if((Descer_C1_wifi || Descer_C1_IR)&& Estado_C1) 
      {
        Estado_C1 = 0;
        EEPROM.begin(4);
        EEPROM.write(0, Estado_C1);
        EEPROM.end();
        Borda_C1 = 1;
      }
    }
 
    if(Borda_C1 == 0)
    {
      if((Subir_C1_wifi || Subir_C1_IR) && Estado_C1 == 0) 
      {
        Estado_C1 = 1;
        EEPROM.begin(4);
        EEPROM.write(0, Estado_C1);
        EEPROM.end();
        Borda_C1 = 1;
      }
    } 
  
    if(Descer_C1_wifi == 0 && Descer_C1_IR == 0 && Subir_C1_wifi == 0 && Subir_C1_IR == 0)
      Borda_C1 = 0;
  }

  if(Estado_C1 == 0 && set_C1)
  {
    if(Descer_C1 == 0)
    {
      Delay_C1 = millis();
    }  
    
    Descer_C1 = 1;
    
    if(millis() - Delay_C1 >= Tempo_descida_C1)  
    { 
      Descer_C1 = 0;
      set_C1 = 0;
    }
  }

  if(Estado_C1 && set_C1 == 0)
  {
    if(Subir_C1 == 0)
    {
      Delay_C1 = millis();
    }  
    
    Subir_C1 = 1;
    
    if(millis() - Delay_C1 >= Tempo_subida_C1)    
    { 
      Subir_C1 = 0;
      set_C1 = 1;
    }
  }

  if(Borda_regula_C1 == 0)
  {
    if(Regula_C1)
    {
      Subir_C1 = !Subir_C1;
      Borda_regula_C1 = 1;
    }
  }

  if(Regula_C1 == 0)
  {
    Borda_regula_C1 = 0;
  }

  //Cortina 2
  if(Descer_C2 == 0 && Subir_C2 == 0)
  {
    if(Borda_C2 == 0)
    {
      if((Descer_C2_wifi || Descer_C2_IR)&& Estado_C2) 
      {
        Estado_C2 = 0;
        EEPROM.begin(4);
        EEPROM.write(1, Estado_C2);
        EEPROM.end();
        Borda_C2 = 1;
      }
    }
 
    if(Borda_C2 == 0)
    {
      if((Subir_C2_wifi || Subir_C2_IR) && Estado_C2 == 0) 
      {
        Estado_C2 = 1;
        EEPROM.begin(4);
        EEPROM.write(1, Estado_C2);
        EEPROM.end();
        Borda_C2 = 1;
      }
    } 
    
    if(Descer_C2_wifi == 0 && Descer_C2_IR == 0 && Subir_C2_wifi == 0 && Subir_C2_IR == 0)
      Borda_C2 = 0;
  }

  if(Estado_C2 == 0 && set_C2)
  {
    if(Descer_C2 == 0)
    {
      Delay_C2 = millis();
    }

    Descer_C2 = 1;
      
    if(millis() - Delay_C2 >= Tempo_descida_C2) 
    { 
      Descer_C2 = 0;
      set_C2 = 0;
    }
  }

  if(Estado_C2 && set_C2 == 0)
  {
    if(Subir_C2 == 0)
    {
      Delay_C2 = millis();
    } 

    Subir_C2 = 1;
        
    if(millis() - Delay_C2 >= Tempo_subida_C2) 
    { 
      Subir_C2 = 0;
      set_C2 = 1;
    }
  }

  if(Borda_regula_C2 == 0)
  {
    if(Regula_C2)
    {
      Subir_C2 = !Subir_C2;
      Borda_regula_C2 = 1;
    }
  }

  if(Regula_C2 == 0)
    Borda_regula_C2 = 0;

  //Para o movimento das cortinas em situaçao de emergencia.
  if(Emergencia_cortinas)
    Set_emergencia_cortinas = 1;

  if(Cortinas_ok)
    Set_emergencia_cortinas = 0;

  //Controle das saidas.  
  if(Set_emergencia_cortinas)
  {
    ciPCF8574Write(OUT_0, 1); 
    ciPCF8574Write(OUT_1, 1);
    ciPCF8574Write(OUT_2, 1); 
    ciPCF8574Write(OUT_3, 1);
  }
    
  if(Set_emergencia_cortinas == 0)
  {
    ciPCF8574Write(OUT_0, !Subir_C1); 
    ciPCF8574Write(OUT_1, !Descer_C1);
    ciPCF8574Write(OUT_2, !Subir_C2); 
    ciPCF8574Write(OUT_3, !Descer_C2);
  }

}

//COMUINICACAO COM O EXPANSOR DE PORTAS VIA I2C.
bool ciPCF8574PinMode(byte pino, int modo) 
{
  static byte modoPinos[qtdeCi];

  if (modo == -1)
  {
    return bitRead(modoPinos[pino / 8], pino % 8); 
  } 

  else
  {
    bitWrite(modoPinos[pino / 8], (pino % 8), modo);
    return modo;
  }
}

void ciPCF8574Write(byte pino, bool estado) 
{
  static bool inicio = true;
  static byte estadoPin[qtdeCi];
  if (inicio) 
  {
    byte estadoCI;
    for (int nL = 0; nL < qtdeCi; nL++) 
    {
      for (int nM = 0; nM < 8; nM++) 
      {
        bitWrite(estadoCI, nM, !ciPCF8574PinMode(nM + (nL * 8)) );  
      }  
      estadoPin[nL] = estadoCI;
    }
    inicio = false;
    }

    bitWrite(estadoPin[pino / 8], pino % 8, estado);
    Wire.beginTransmission(enderecosPCF8574[pino / 8]);    
    Wire.write(estadoPin[pino / 8]);                            
    Wire.endTransmission();        
}

bool ciPCF8574Read(byte pino) 
{
  byte lido;
  bool estado;
  Wire.requestFrom(enderecosPCF8574[pino / 8], 1);
  if(Wire.available())
  {   
    lido = Wire.read();        
  }     
  estado = bitRead(lido, pino % 8);
  return estado;  
}
