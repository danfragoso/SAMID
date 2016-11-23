//SAMID 07/10/2016
#include <Arduino.h>
#include <LiquidCrystal.h> //Inclui biblioteca



LiquidCrystal lcd(8, 9, 4, 5, 6, 7);// PINOS LCD

const int ledAlerta = 12;//PINO DO LED
const int valvula_pino = 11;//PINO DA VALVULA

int ledAlerta_Estado = LOW;
int valvula_pino_Estado = LOW;


int tempo_regar = 2000;

int reservatorio_pino = 5;
int umidade_pino = 3;
int luminosidade_pino = 2;

unsigned long previousMillis = 0;
unsigned long interval = 1000;

unsigned long tempoAnterior = 0;
unsigned long intervalo = 10000;
//unsigned long intervalo = 3000000; //5 MINUTOS DE INTERVALO PARA VERIFICACAO DE SENSORES E ALERTAS



unsigned long vai; // LIXO, APAGAR


String planta_especie[5]; // ESPECIES DE PLANTA
String modo_irrigacao[3]; //MUDAR PARA TAMANHO_PLANTA

int luminosidade_limite;
int umidade_limite;

int contador_de_planta = 0;// CONTADOR DE OPCAO PARA LOOP DE MENU
int contador_de_modo = 0;// MUDAR NOME PARA CONTADOr DE TAMANHO
int umidade_percent;
int reservatorio_percent; // MUDAR NOME PARA RESERVTARIO PERCENT
int luminosidade_percent;
int contador_de_menu = 2;
int botao = 0;
int botao_raw  = 0;
int umi_cust = 5;
int lock = 1; // VERIFICAR E APAGAR SE NAO ESTIVER EM USO

//Declaracao dos botões
#define botao_direita  0
#define botao_cima     1
#define botao_baixo   2
#define botao_esquerda   3
#define botao_selecao 4
#define botao_nulo   5

byte gotachar[8] = { B00100,B00100,B01010,B01010,B10001,B10001,B10001,B01110,};// ICONE DA GOTA
byte reservatoriochar[8] = { B10001,B10001,B10001,B11111,B11111,B11111,B11111,B01110,};// ICONE RESERVATORIO
byte luzchar[8] = { B00100,B01010,B10001,B10001,B01010,B01110,B01110,B00100,};//ICONE LAMPADA


int get_botao(){ // LE BOTAO

  botao_raw = analogRead(0);
  if (botao_raw > 1000){
    return(botao_nulo);
  } else if (botao_raw < 50){
    return (botao_direita);
  } else if (botao_raw < 250){
     return (botao_cima);
  } else if (botao_raw < 450){
     return (botao_baixo);
  } else if (botao_raw < 650){
     return (botao_esquerda);
  } else if (botao_raw < 850){
     return (botao_selecao);
  } else {
    return(botao_nulo);
  }

}

void init_plantas(){// DECLARA PLANTAS

  planta_especie[0] = "Suculenta   ";
  planta_especie[1] = "Cacto       ";
//  planta_especie[2] = "Pimenteira ";
//  planta_especie[3] = "CopoDeLeite";
//  planta_especie[4] = "Girassol   ";
  planta_especie[2] = "Rosa       ";
//  planta_especie[6] = "LirioDaPaz ";
  planta_especie[3] = "Samambaia  ";
  planta_especie[4] = "Custom     ";

}

void init_modo(){ // DECLARA TAMANHO DE PLANTA

  modo_irrigacao[0] = "Pequena     ";
  modo_irrigacao[1] = "Media       ";
  modo_irrigacao[2] = "Grande      ";

}


int get_reservatorio(){ //CALIBRADO: LE RESERVATORIO

  int reservatorio_raw;
  reservatorio_raw = analogRead(reservatorio_pino);

  if ( reservatorio_raw > 600){
    return(99);
  }else if ( reservatorio_raw < 600 && reservatorio_raw > 550){
    return(75);
  }else if ( reservatorio_raw < 550 && reservatorio_raw > 500){
    return(50);
  }else if ( reservatorio_raw < 500 && reservatorio_raw > 450){
    return(25);
  }else if ( reservatorio_raw < 450 && reservatorio_raw > 400){
    return(10);
  }else if ( reservatorio_raw < 400){
    return(00);
  }
}

int get_luminosidade(){ // CALIBRADO: LE LUMINOSIDADE

  int luminosidade_raw;
  luminosidade_raw = analogRead(luminosidade_pino);

  if ( luminosidade_raw > 1021 && luminosidade_raw < 1028){
    return(99);
  }else if ( luminosidade_raw < 1022 && luminosidade_raw > 800){
    return(75);
  }else if ( luminosidade_raw < 800 && luminosidade_raw > 600){
    return(50);
  }else if ( luminosidade_raw < 600 && luminosidade_raw > 400){
    return(25);
  }else if ( luminosidade_raw < 400 && luminosidade_raw > 200){
    return(10);
  }else if ( luminosidade_raw < 200){
    return(05);
  }
}


int get_umidade(){ // CALIBRAR : LE UMIDADE

  int umidade_raw;
  umidade_raw = analogRead(umidade_pino);

  if (umidade_raw > 800 && umidade_raw < 950){
    return(99);
  }else if ( umidade_raw < 800 && umidade_raw > 750){
    return(75);
  }else if ( umidade_raw < 750 && umidade_raw > 700){
    return(50);
  }else if ( umidade_raw < 700 && umidade_raw > 650){
    return(25);
  }else if ( umidade_raw < 650 && umidade_raw > 600){
    return(10);
  }else if ( umidade_raw < 2){
    return(00);
  }
}

void splash(){// SPLASH

 lcd.begin(16, 2);
 lcd.setCursor(0,0);
 lcd.print(" ***SAMID V1***");
 lcd.setCursor(0,1);
 lcd.print(" Auto Irrigador");
 delay(2000);
 lcd.clear();
}

void opt_tipo(){// ESCREVE O TIPO NA TELA

  lcd.setCursor(0,0);
  lcd.print("Tipo:");
  lcd.print(planta_especie[contador_de_planta]);

}

void set_custom(){// SET O NIVEL DE UMIDADE CUSTOM

  lcd.setCursor(0,0);
  lcd.print("Umidade em ");
  lcd.print(umi_cust);
  lcd.print("% ");
    delay(800);

  umi_cust = umi_cust + 5;
  if (umi_cust > 100){
   umi_cust = 5;
 }else if (umi_cust < 0){
   umi_cust = 2;
  }
}

void opt_modo(){// ESCREVE MODO

  lcd.setCursor(0,0);
  lcd.print("Tamanho:");
  lcd.print(modo_irrigacao[contador_de_modo]);
}


void loop_modo(){// LOOP DE MENU DE TAMANHO

  if (contador_de_modo > 2){
   contador_de_modo = 0;
 }else if (contador_de_modo < 0){
   contador_de_modo = 2;
  }
}

void loop_tipo(){// LOOP DE MENU DE PLANTA

  if (contador_de_planta > 4){
   contador_de_planta = 0;
 }else if (contador_de_planta < 0){
   contador_de_planta = 4;
  }
}

void loop_menu(){// LOOP DOS MENUS
  if (contador_de_menu > 2){
    contador_de_menu = 1;
  }else if (contador_de_menu < 1){
    contador_de_menu = 2;
  }
}

void call_regar(){// ROTINA RESPONSAVEL POR REGAR AS PLANTAS E DISPARAR ALERTAS

    unsigned long tempoAtual = millis();

    if (tempoAtual - tempoAnterior > intervalo){
      tempoAnterior = tempoAtual;


    if (reservatorio_percent <= 0){
     set_alerta(1);
   }
    if (luminosidade_percent >= luminosidade_limite){
      set_alerta(2);
    }
    if ((umidade_percent <= umidade_limite) && (reservatorio_percent != 0) /**/){
      get_regar();

    }
  }
}

void call_led(){

  unsigned long currentMillis = millis();
   if (currentMillis - previousMillis > interval) {
     previousMillis = currentMillis;
     if (ledAlerta_Estado == LOW)
       ledAlerta_Estado = HIGH;
     else
       ledAlerta_Estado = LOW;
     digitalWrite(ledAlerta, ledAlerta_Estado);
   }

}

void set_tempo(){
  if (contador_de_modo == 0){
    tempo_regar == 2000;
  }else if (contador_de_modo == 1){
    tempo_regar == 3000;
  }else if (contador_de_modo == 2){
    tempo_regar == 6000;
  }
}

void set_luminosidade(){

 if (contador_de_planta == 0){
   luminosidade_limite = 51;
 }else if (contador_de_planta == 1){
   luminosidade_limite = 76;
 }else if (contador_de_planta == 2){
   luminosidade_limite = 10;
 }else if (contador_de_planta == 3){
   luminosidade_limite = 26;
 }else if (contador_de_planta == 4){
   luminosidade_limite = 99;
 }
}

void set_umidade(){

  if (contador_de_planta == 0){
    umidade_limite = 50;
  }else if (contador_de_planta == 2){
    umidade_limite = 75;
  }else if (contador_de_planta == 1){
    umidade_limite = 10;
  }else if (contador_de_planta == 3){
    umidade_limite = 25;
  }else if (contador_de_planta == 4){
    umidade_limite = umi_cust;
  }
 }


void get_regar(){
  lcd.setCursor(0,0);
  lcd.write(1);
  lcd.print("   Regando !  ");
  lcd.write(1);
  lcd.setCursor(0,1);
  lcd.print("               ");
  digitalWrite(valvula_pino, HIGH);
  delay(tempo_regar);
  digitalWrite(valvula_pino, LOW);
  lcd.clear();
}

void set_alerta(int tipo_alerta){

  if (tipo_alerta == 1){
    //Reservatorio vazio
    lcd.setCursor(0,0);
    lcd.print(" O reservatorio ");
    lcd.setCursor(0,1);
    lcd.print(" esta vazio :/  ");
    delay(2000);
    lcd.clear();

  }else if (tipo_alerta == 2){
    //Muita luz
    lcd.setCursor(0,0);
    lcd.print("Muita luz, mova");
    lcd.setCursor(0,1);
    lcd.print("   a planta.   ");
    delay(2000);
    lcd.clear();
  }else if (tipo_alerta == 3){
    //Muita luz
    lcd.setCursor(0,0);
    lcd.print("Pouca luz, mova");
    lcd.setCursor(0,1);
    lcd.print("   a planta.   ");
    delay(2000);
    lcd.clear();
  }

}

void set_confirmacao(){

   lcd.setCursor(0,0);
   lcd.print("       OK      ");
   delay(2000);
   lcd.clear();
}

void sensor_update(){

   luminosidade_percent = get_luminosidade();
   reservatorio_percent = get_reservatorio();
   umidade_percent = get_umidade();

   lcd.createChar(3,luzchar);
   lcd.setCursor(5,1);
   lcd.write(3);
   lcd.print(luminosidade_percent);
   lcd.print("% ");

   lcd.createChar(1,gotachar);
   lcd.setCursor(0,1);
   lcd.write(1);
   lcd.print(umidade_percent);
   lcd.print("% ");

   lcd.createChar(2,reservatoriochar);
   lcd.setCursor(10,1);
   lcd.write(2);
   lcd.print(reservatorio_percent);
   lcd.print("%   ");
}

void setup()
{
  pinMode(ledAlerta, OUTPUT);
  pinMode(valvula_pino, OUTPUT);
  init_modo();
  init_plantas();
  splash();
  Serial.begin(9600);
}

void loop()
{
  //vai=millis();


 loop_tipo();
 loop_modo();
 loop_menu();



  switch (contador_de_menu)
  {
    case 1:
    {
      opt_modo();
      break;
    }
    case 2:
    {
      opt_tipo();
      break;
    }
  }

 set_tempo();
 set_umidade();
 set_luminosidade();
 sensor_update();
 call_regar();

 if ((umidade_percent <= umidade_limite) || (reservatorio_percent <= 0) || (luminosidade_percent >= luminosidade_limite)){
   call_led();
 }else{
   ledAlerta_Estado = LOW;
 }

 if((contador_de_planta == 4) && (contador_de_menu == 2)){
   int umi_now = umi_cust - 5;
    lcd.setCursor(12,0);
    lcd.print(umi_now);
    lcd.print("%");
    delay(250);

  }


 botao = get_botao();

switch (botao)
 {
   case botao_direita:
     {
       switch (contador_de_menu){
         case 1:
         {
           contador_de_modo++;
           break;
         }
         case 2:
         {
           contador_de_planta++;
           break;
         }
       }
     delay(300);
     break;
     }

   case botao_esquerda:
     {
       switch (contador_de_menu){
         case 1:
         {
           contador_de_modo--;
           break;
         }
         case 2:
         {
           contador_de_planta--;
           break;
         }
       }
     delay(300);
     break;
     }

   case botao_cima:
     {
       contador_de_menu++;
     delay(300);
     break;
     }
   case botao_baixo:
     {
       contador_de_menu--;
     delay(300);
     break;
     }
   case botao_selecao:
     {
      if((contador_de_planta == 4) && (contador_de_menu == 2)){
         set_custom();
      } else {
      set_confirmacao();
     }
     break;
     }
     case botao_nulo:
     {
     lcd.print(" ");
     break;
     }
 }


}
