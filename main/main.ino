#include <Keypad.h>
#include<SPI.h>
#include<LiquidCrystal.h>
#include<Servo.h>


//////////////////////////////////////////////////////////////////////////////////////////////////
//Este codigo fue creado como sistema de control del banco de preubas de intercambiadores de calor
//El proyecto general esta hecho por estudiantes de la Universidad de Guadalajara como parte del proyecto de titulacion
//Ingeniería Mecánica Eléctrica
//Rodrigo Ruíz Sandoval
//Jonathan Barba Tostado
//Abraham Donatt Sánchez Sánchez
//Salvador Maximiliano González Cervantes 

//Este programa permite interactuar con el usuario tanto en el monitor serie como en la pantalla LCD
//////////////////////////////////////////////////////////////////////////////////////////////////

//Initialize the library with the number of the pin
  LiquidCrystal lcd(26, 25, 24, 23, 22, 21);
//Conexiones de la pantalla LCD
  //VSS   GND
  //VDD   +5V
  //V0    a resistencia de 1520 ohms y de esta a GND
  //RS    Pin 26
  //RW    GND
  //E     25
  //D4    24
  //D5    23
  //D6    22
  //D7    21
  //A     a resistencia de 220 Ohm y de esta a +5
  //K     a GND  

//Designacion de pines 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Constantes para el control del relevador de las resistencia 
int resistenceRel=21;
//Sensores de temperatura en el deposito de agua caliente y en las entradas y salidad de agua caliente y fria
/* Se cambio las variables indiviaduales a un arreglo de tipo float de 13 elementos */
float temp_sensor [13];
//Variables de comprobacion del sensor de temperatura
/* Se cambio las variables indiviaduales a un arreglo de tipo float de 13 elementos */
float temp_state [13];
//Variable para el valor introducido de la temperatura
int temp;
//Flag de variable de temperatura
int temp_flag;
//Variables para comprobar si se ha elegido una opcion valida de configuracion de menu
/* Se cambio las variables indiviaduales a un arreglo de tipo int de 6 elementos */
int state [6];
//Variable para comprobar si el usuario deja de introducir informacion del keypad
int data_in;
//Termopar del baño de agua caliente
int hot_tank=A0;
//Heat exchanger ids'
int hex_id;
//Termopares de cada uno de los intercambiadores de calor (nomenclaturas)
/*
  int hex1_in_hot=A1; int hex1_out_hot=A2; int hex1_in_cold=A3; int hex1_out_cold=A4; int hex2_in_hot=A5; int hex2_out_hot=A6;
  int hex2_in_cold=A7; int hex2_out_cold=A8; int hex3_in_hot=A9; int hex3_out_hot=A10; int hex3_in_cold=A11; int hex3_out_cold=A12;
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Nomenclatura:
//hex     Heat Exchanger
//valv    Valvule
//bomb    Bomba
//in      Entrada
//out     Salida
//state   varable de comprobacion de menu
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Constantes para el control del banco de intercambiadores de calor
//Constantes para el control de valvulas de intercambiador de tubo doble
  //Valvulas de entrada y salida => [0,1,2,3]
  /*
   * [0,1] => [inh,outh] {int hex1_valv_in=9,int hex1_valv_out=10 }
   * [2,3] => [inc,outc]
   */
  //Valvulas de control de flujo en el intercambiador de tubo doble => [4,5,6,7]
  //Numeros impares para valvulas de flujo paralelo => [11,13]
  //Numeros pares para valvulas de contraflujo => [12,14]
  Servo hex1_valv [8];
//Constantes para el control de valvulas de intercambiador de banco de tubos
  //Valvulas de entrada y salida => [0,1,2,3]
  /*
   * [0,1] => [inh,outh] {int hex2_valv_in=15 ,int hex2_valv_out=16 }
   * [2,3] => [inc,outc]
   */
  Servo hex2_valv [4];
//Constantes para el control de valvulas de intercambiador de aletas
  //Valvulas de entrada y salida => [0,1]
  /*
   * [0,1] => [inh,outh] {int hex3_valv_in=17 ,int hex3_valv_out=18 }
   */
    Servo hex3_valv [2];
//Constantes para el control de motor de bomba liquido caliente
    int bomb_h=19;
//Constantes para el control de motor de bomba liquido frio
    int bomb_c=20;
//Variables para el control de cierre y apertura de valvulas
    boolean closed;
    int x=0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Conexiones de Keypad
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const byte rows = 4; //four rows
    const byte cols = 3; //three columns
    char keys[rows][cols] = {
      {'1','2','3'},
      {'4','5','6'},
      {'7','8','9'},
      {'*','0','#'}
    };
    byte rowPins[rows] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
    byte colPins[cols] = {8, 7, 6}; //connect to the column pinouts of the keypad
    Keypad teclado = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );
    
//Constantes para el control de la pantalla LCD

char tecla,input;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////  Setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  int h1 = 0;h2 = 0;h3 = 0;
  for(int i = 9; i <=22;i++){
    if(i<=12 || (i>16 && i<=20)){
      hex1_valv[h1].attach(i);
      hex1_valv[h1].write(x);
      h1++;
    }
    else if (i>12 && i<=16){
      hex2_valv[h1].attach(i);
      hex2_valv[h2].write(x);
      h2++;
    }
    else if(i>20){
      hex3_valv[h3].attach(i);
      hex3_valv[h3].write(x);
      h3++;
    }
  }

  hex3_valv_in.attach(21);
  hex3_valv_out.attach(22);
  
  
  pinMode(bomb_h,OUTPUT);                     //Señal de salida para activar la bomba de agua caliente
  pinMode(bomb_c,OUTPUT);                     //Señal de salida para activar la bomba de agua fria
  pinMode(resistenceRel,OUTPUT);              //Señal de salida para activar el relevador de la resistecia
  pinMode(A0,INPUT);                          //Señal de entrada para toma de datos de la temperatura del baño de agua caliente
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);                          //Señales para entrada de toma de datos de los termopares de cada intercambiador
  pinMode(A6,INPUT);
  pinMode(A7,INPUT);
  pinMode(A8,INPUT);
  pinMode(A9,INPUT);
  pinMode(A10,INPUT);
  pinMode(A11,INPUT);
  pinMode(A12,INPUT);
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Time,temp_sensor_A,temp_sensor_B,temp_sensor_C,temp_sensor_D");
  //Set up the LCD's number of cols and rows
  lcd.begin(16, 2);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////  Void loop: se corre el programa
void loop() {
  // put your main code here, to run repeatedly:
  //Press '*' to start the hole program
   Serial.println("Bienvenido!");
   Serial.println("Presione * para iniciar");
   lcd_print("Bienvenido","presione *");
   tecla=teclado.waitForKey();                                      //Revisa si la tecla '*' esta siendo presionada para iniciar el sistema
   if(tecla=='*'){
    Serial.println("Bienvenido al sistema de control de intercambiadores de calor");
    Serial.println("Presione una tecla para escoger el intercambiador de calor a utilizar");
    Serial.println("1. Tubo doble \n2. Banco de tubos \n3. Tubo y aletas");
    lcd_print("Seleccione","el intercambiador");
    delay(1000);
    lcd_print("1 Tubo y aletas","2 Banco 3 Aletas");
    tecla=teclado.waitForKey();
    if(tecla=='1'){                                                       //Eleccion del intercambiador de calor de tubo doble
       hex_id=1;                                                          //hex_id indica el id del intercambiador de calor que se esta trabajando
       delay(1000);
       valve_control(hex1_valv_inh,1);                                   //Valvula de entrada al intercambiador de calor de tubo doble encendida
       valve_control(hex1_valv_outh,1);                                  //Valvula de salida al intercambiador de calor de tubo doble encendida
       valve_control(hex1_valv_inc,1);
       valve_control(hex1_valv_outc,1);
       valve_control(hex2_valv_inh,0);                                    //Valvula de entrada al intercambiador de calor de banco de tubos apagada
       valve_control(hex2_valv_outh,0);
       valve_control(hex2_valv_inc,0);                                    //Valvula de entrada al intercambiador de calor de tubo y aletas apagada
       valve_control(hex2_valv_outc,0);
       valve_control(hex3_valv_out,0);
       valve_control(hex3_valv_in,0);               
       Serial.println("Presione * para salir al menu anterior");
       lcd_print("Presione","* para salir");
       delay(1000);
       Serial.println("Seleccione el tipo de flujo que desea que se desarrolle");
       Serial.println("1. Contraflujo \n2. Flujo paralelo");
       lcd_print("Seleccione","el flujo");
       delay(1000);
       lcd_print("1 Contraflujo","2 Paralelo");
       state=1;
       do{                                                    //Submenu de eleccion de configuracion de flujo en el intercambiador de calor de tubo doble
        lcd.clear(); lcd.print("Tubo doble");
        Serial.println("Intercambiador de tubo doble");
        tecla=teclado.getKey();
        if(tecla=='*'){
          state=0;
        }
        flux_menu('1',"Contraflujo",0,1,0,1);                              //Eleccion de flujo paralelo 
        flux_menu('2',"Paralelo",1,0,1,0);                                 //Eleccion de flujo contraflujo
      }while(state==1);
    }
    else if(tecla=='2'){                                                                //Se elige el intercambiador de banco de tubos
      hex_id=2;                                                            //hex_id indica el id del intercambiador de calor que se esta trabajando
      main_menu(state,"Banco de tubos",1,1,0,0,0,0);
    }
    else if(tecla=='3'){                                                                //Se elige intercambiador de tubo y aletas
      hex_id=3;                                                            //hex_id indica el id del intercambiador de calor que se esta trabajando
      main_menu(state,"Tubo y aletas",0,0,0,0,1,1);
    }
  }     
}//void loop


void main_menu(int state,String hex_name,int valv1,int valv2,int valv3,int valv4,int valv5,int valv6){
  Serial.println(hex_name);
  lcd.clear(); lcd.print(hex_name);
  delay(1000);
  valve_control(hex2_valv_in,valv1);                                       //Valvula de entrada al intercambiador de banco de tubos
  valve_control(hex2_valv_out,valv2);                                      //Valvula de salida al intercambiador de banco de tubos
  valve_control(hex1_valv_in,valv3);                                       //Valvula de entrada al intercambiador de calor de tubo doble
  valve_control(hex1_valv_out,valv4);                                      //Valvula de salida al intercambiador de calor de tubo doble
  valve_control(hex3_valv_in,valv5);                                       //Valvula de entrada al intercambiador de calor de tubo y aletas
  valve_control(hex3_valv_out,valv6);                                      //Valvula de salida al intercambiador de calor de tubo y aletas
  Serial.println("Elija la temperatura que desea que se desarrolle");

  state1=1;
  temp_menu(state1);
}

void temp_menu(int state){ 
  Serial.println("introduzca el valor de la temperatura que desea alcanzar");
  Serial.println("deben ser valores menores a 60 grados celsius");
  Serial.println("ingrese el valor con los digitos y presione C para confirmar o # para borrar");
  lcd_print("Introduzca","la temperatura");
  delay(1000);
  lcd_print("Debe ser menor","a 60 grados");
  do{                                                                  //Submenu de eleccion de la temperatura
     tecla=teclado.getKey();
     switch(tecla){
      case '0'...'9':   //case: '0'...'9':
        temp=temp*10+(tecla-'0');
      break;
      case 'A':         //case: A
        if(temp>60){                                                                //Si la temperatura introducida es mayor al valor permitido el programa no procede
          Serial.println("Valor de temperatura no permitido");
          lcd_print("Valor","no permitido");
          temp=0;
          break;
        }
        else{
        state2=1;
        start_menu(state2);
        }
      break;
      case 'C':         //case C                                                    //Si se desea limpiar el valor de temperatura para corregirlo 
        temp=0; Serial.println("Reingrese el valor de temperatura");
        lcd_print("Reingrese","valor de temp");
      break;
      case '*':         //case *                                                    //Si se desea volver al menu anterior
        temp=0;
        state=0;
      break;
      }
  }while(state==1);
}
void start_menu(int state){
  do{
    Serial.println("Valor de temperatura introducido:");
    lcd.clear(); lcd.print("Valor de temp");
    lcd.setCursor(0,1); lcd.print(temp);
    Serial.println(temp);
    delay(1000);
    Serial.println("Inicie la prueba pulsando # \nMenu anterior *");
    lcd_print("iniciar #","Menu anterior *");
    tecla=teclado.getKey();
    if(tecla=='#'){                                                         //Si se presiona # la prueba se inicia
      Serial.println("Prueba iniciada");
      lcd.clear(); lcd.print("Prueba iniciada");
      digitalWrite(resistenceRel,HIGH);                                     //La resistencia se enciende
      state4=1;
      do{
        input=teclado.getKey();
        temp_output(hot_tank,temp_sensor_0,temp_state_0);
        if(temp_state_0>=temp){                                             //Si el valor de temperatura en el baño caliente alcanza la temperatura deseada, se comienza la prueba
          temp_flag=1;
        }
        else if(temp_flag==1){                                              //temp_flag comienza la prueba y se encienden las bombas
          digitalWrite(bomb_c,HIGH);
          digitalWrite(bomb_h,HIGH);
          state5=1;
          do{
            input=teclado.getKey();
            temp_output(hot_tank,temp_sensor_0,temp_state_0);
            if(hex_id==1){                                                  //Si el id del intercambiador fue 1 se toma lectura solo de los termopares de ese intercambiador
              lcd_temp_print(A1,A2,A3,A4,temp_sensor_1,temp_sensor_2,temp_sensor_3,temp_sensor_4,temp_state_1,temp_state_2,temp_state_3,temp_state_4);             
            } 
            else if(hex_id==2){                                             //Si el id del intercambiador fue 2 se toma lectura solo de los termopares de ese intercambiador
              lcd_temp_print(A5,A6,A7,A8,temp_sensor_5,temp_sensor_6,temp_sensor_7,temp_sensor_8,temp_state_5,temp_state_6,temp_state_7,temp_state_8);
            }
            else if(hex_id==3){                                             //Si el id del intercambiador fue 3 se toma lectura solo de los termopares de ese intercambiador
              lcd_temp_print(A9,A10,A11,A12,temp_sensor_9,temp_sensor_10,temp_sensor_11,temp_sensor_12,temp_state_9,temp_state_10,temp_state_11,temp_state_12);
            }
            else if(temp_state_0<temp){                                     //mientras el valor de temperatura leido sea menor al valor deseado se enciende la resistencia
              digitalWrite(resistenceRel,HIGH);
            }
            else if(input=='C'){                                            //si se presiona la C en el keypad se cancela la prueba
              state5=0;
              state4=0;
            }
          }while(state5==1);
        }
        else if(input=='*'){
          state4=0;
        }
      }while(state4==1);
    }
    else if(tecla=='*'){
      state=0;
      temp=0;
    }
  }while(state==1);
}
void flux_menu(char state,String flux_name,int valv1,int valv2,int valv3,int valv4){
       if(tecla==state){                                                    //Configuracion de flujo
          Serial.println(flux_name);
          lcd.clear(); lcd.print(flux_name);
          digitalWrite(hex1_valv1,valv1);                                    //Valvulas  1 y 3 para configuracion flujo paralelo 
          digitalWrite(hex1_valv3,valv3);                                    
          digitalWrite(hex1_valv2,valv2);                                    //Valvulas  2 y 4 para configuracion flujo contraflujo
          digitalWrite(hex1_valv4,valv4);                                         
          delay(1000);
          state3=1;

          temp_menu(state3);
        }
}

void lcd_print(String fl,String sl){     //String fl y sl de first line y second line
  lcd.clear(); lcd.print(fl);
  lcd.setCursor(0,1); lcd.print(sl);
}
void temp_output(int A,float temp_sensor,float temp_state){
    temp_sensor=analogRead(A);
    temp_state=temp_sensor*0.009;
}
//******
//Void lcd_temp_print() toma varios parametros: los parametros de tipo int son para identificar cuales son los pines analogicos de los que se va obtener el dato de temperatura
//los parametros float de tipo temp_sensor son para obtener el valor de voltaje de los pines analogicos identificados por los parametros tipo int 
//los parametros float de tipo temp_state son para convertir el valor de voltaje entregado por temp_sensor a valor de temperatura 
//******
void lcd_temp_print(int A,int B,int C,int D,float temp_sensor_A,float temp_sensor_B,float temp_sensor_C,float temp_sensor_D,
                    float temp_state_A,float temp_state_B,float temp_state_C,float temp_state_D){
    temp_sensor_A=analogRead(A);temp_state_A=temp_sensor_A*0.009;temp_sensor_B=analogRead(B);temp_state_B=temp_sensor_B*0.009;
    temp_sensor_C=analogRead(C);temp_state_C=temp_sensor_C*0.009;temp_sensor_D=analogRead(D);temp_state_D=temp_sensor_D*0.009;

    lcd.clear();
    lcd.print(temp_sensor_A);lcd.setCursor(7,0);lcd.print(temp_sensor_B);
    lcd.setCursor(0,1);lcd.print(temp_sensor_C);lcd.setCursor(7,1);lcd.print(temp_sensor_D);

    Serial.print("DATA,TIME,");Serial.print(temp_sensor_A);Serial.print(",");Serial.print(temp_sensor_B);Serial.print(",");Serial.print(temp_sensor_C);
      Serial.print(",");Serial.println(temp_sensor_D);
}
void valve_control(Servo valve,int value){ 
  if(value==1){  
    if(x==0 or closed==true){
      for(int x=0;x<=180;x++){
        valve.write(x);
        delay(20);
      }
      closed==false;
    }
  }
  else if(value==0){
    if(x==180 or closed==false){
      for(int x=180;x>=0;x--){
        valve.write(x);
        delay(20);
      }
      closed==true;
    }
  }
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//Código hecho por: Salvador Maximiliano González Cervantes
//////////////////////////////////////////////////////////////////////////////////////////////////


