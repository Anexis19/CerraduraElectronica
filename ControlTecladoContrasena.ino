//IMPORTACION DE LIBRERIAS
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

//DEFINICION DE VARIABLES PARA ALMACENAMIENTO SD
#define SSpin 53
File archivo;
int contadorRegistro=0;

//DECLARACION DE VARIABLES


SoftwareSerial objBluetooth(10,11);       // 10 = Transmision Arduino, 11 =  Recepcion Arduino
LiquidCrystal_I2C lcd (0x27, 16,2);       // Creacion de objeto para la LCD
Servo servoMotor;

boolean bandera = true;                   //  Bandera que detiene la lectura del Keypad               
boolean bandera2 = true;
boolean bandera3 = true;
boolean bandera4 = true;


char lecturaTecla;                        //  Variable CHAR que almacena UN caracter ingresado
char claveIngresada[7];                   //  Array CHAR que almacena un maximo de 7 elementos
char claveMaestra[7]="123456";            //  Array CHAR que almacena una contrasena definida
char claveSonido[7]="ABCD11";
int indice = 0;                           //  Variable indice

const int fila = 4;                       //  Asignacion de variable FILA = 4
const int columna = 4;                    //  Asignacion de variable COLUMNA = 4
char letras[fila][columna] = {            //  Array de 2 dimensiones que almacena KEYPAD
  
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

//SE DEFINEN LOS PINES DEL KEYPAD COMO BYTE

byte pinesColumna[columna] = {5,4,3,2};   //  Asignacion de pines del ARDUINO correspondientes a las columnas
byte pinesFila[fila] = {9,8,7,6};         //  Asignacion de pines del ARDUINO correspondientes a las FILAS

//CREACION DE OBJETO KEYPAD USANDO VALORES DEFINIDOS

Keypad mapaTeclado = Keypad(makeKeymap(letras), pinesFila, pinesColumna, fila, columna);

//DEFINICION DE PINES Y VARIABLES PARA SENSOR DE SONIDO

int microfono = 23;
int led = 22;
int estadoMicrofono;
int estadoLed;
int contador = 0;
int contadorSesion = 0;

//DEFINICION DE PINES Y VARIABLES PARA SENSOR TIPO HALL

int led2 = 13;
int pinDigital = 24;
int pinAnalogo = A0;
int valorDigital;
int valorAnalogo;

//DEFINICION DE VARIABLE DE CONTROL DE TIEMPO
int contadorControlTiempo = 0;

void setup() {

    
    
    //INICIALIZACION DE MONITOR SERIAL

  
    Serial.begin(9600);
    objBluetooth.begin(9600);
    pinMode(microfono,INPUT);
    pinMode(led, OUTPUT);
    Wire.begin();
    lcd.begin(16,2);
    servoMotor.attach(12);
    pinMode(led2, OUTPUT);
    pinMode(pinDigital,INPUT);

    
    

    
    
}


void loop() {
  // put your main code here, to run repeatedly:
  

  while(bandera == true){                                 // Bucle que se encarga de la lectura de datos


      
      //      Verificacion de conexion BT
      if (objBluetooth.available()){

              Serial.println("Conexion Bluetooth Establecida");
              Serial.write(objBluetooth.read());
              accesoConcedido();
              aperturaPuerta();

              while (bandera4 = true){

                  delay(1000);
                  inicioSesion();
                
              }
              
      }

      
      lecturaTecla = mapaTeclado.getKey();                // Almacenamiento de la lectura ingresada en el objeto KEYPAD definido
      
      if (lecturaTecla){                                  // Si la lectura es es correcta, hacer:
        
          claveIngresada[indice] = lecturaTecla;          // El dato leido se agrega al Array claveIngresado
          indice++;                                       // Aumento de indice para seguir almacenando LECTURAS
          Serial.print(lecturaTecla);                     // Impresion de datos en MONITOR SERIAL
          ingreseContrasena();                            // Llamado a funcion ingreseContrasena - LCD
          
      }  
      if (indice == 6){                                   // Si el Array claveIngresada ya tiene sus 6 elmentos, hacer:
          
          validacionTeclado(claveIngresada);              // Llamado de funcion validacionTeclado(ARGUMENTO) - LCD
      }
     
  }
                                              // Retardo para el conteo de tiempo de sesion
  inicioSesion();                                         // Llamado a funcion inicioSesion - LCD
  estadoPuerta();

  Serial.println("CICLO WHILE PRINCIPAL");
}


void validacionTeclado(char claveIng[7]){                 // Funcion que recibe la clave ingresada
  
    if(!strcmp(claveIng, claveMaestra)){                  // Funcion que compara dos elementos CHAR o STRING
        Serial.print(" Contrasena Correcta");             // !strcmp (Indica que si es DIFERENTE a 0, los elementos son IGUALES)

        registroTecladoSD();
         
        aperturaPuerta();
        accesoConcedido();                                // Llamado a funcion accesoConcedido - LCD
        delay(3000);                                      // Retardo para salida de bucle
        bandera = false;                                  // Salida del Bucle que lee el ingreso por KEYPAD
    }
    else if(!strcmp (claveIng,claveSonido)){
        Serial.print(" Codigo Ingresado para lectura de sonido");
        
        if(bandera3 == true){
                delay(1000);
                accesoPorSonido();  
        }  
        else{
                delay(1000);
                inicioSesion();  
        }
        
        ingresoSonido();
        
    }
    else{
      
        Serial.println(" Contrasena Incorrecta, vuelva a digitar");
        accesoDenegado();                                 // Llamado funcion accedoDenegado - LCD
        delay(3000);                                      // Retardo para una nueva lectura
        lcd.clear();                                      // Apagado de LCD
        indice = 0;                                       // Validacion INCORRECTA, el bucle se sigue REPITIENDO
    }
    
}

//CODIGOS DE LCD

void inicioSesion(){
    delay(1000);
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Bienvenido USER");
    lcd.setCursor(0,1);
    lcd.print("T_SESION:");
    lcd.print( contador+=1);
    lcd.print(" sg");

    if(contador == 9999){

          contador = 0;
    }

}

void accesoConcedido(){

    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("ACCESO CONCEDIDO");
    lcd.setCursor(0,1);
    lcd.print("   BIENVENIDO");
    
}

void accesoDenegado(){
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print(" ACCESO DENEGADO");
    lcd.setCursor(0,1);
    lcd.print("INTENTE DE NUEVO");
}

void ingreseContrasena(){
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("   BIENVENIDO");
    lcd.setCursor(0,1);
    lcd.print("DIGITE  PASSWORD");
}

void accesoPorSonido(){
    lcd.clear();
    lcd.backlight();  
    lcd.setCursor(0,0);
    lcd.print("DIGITE LA CLAVE");
    lcd.setCursor(0,1);
    lcd.print("   DE SONIDO");
    
}

void apagadoPantalla(){
    lcd.clear();  
}

void cerrarSesion(){
  
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);  
    lcd.print(" CERRANDO PUERTA");
    lcd.setCursor(0,1);
    lcd.print("  HASTA PRONTO");
}

void LCD_ControlTiempo(){

    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print(" FAVOR ESCRIBIR");
    lcd.setCursor(0,1);
    lcd.print(" PSW MAS RAPIDO");
  
}
void ingresoSonido(){
      Serial.println(" Ingreso a funcion de sonido");
      Serial.println(" Fuera de while");
      
      
      while(bandera2 == true){
        
            
            
            controlTiempo();
            Serial.println("Dentro del while");
            estadoMicrofono = digitalRead(microfono);
            if(estadoMicrofono == HIGH){
                contador+=1;
                Serial.println("Hola");
                Serial.println(contador);

                
                
                digitalWrite(led, HIGH);
                delay(1000);
                digitalWrite(led, LOW);

                if (contador > 5){
                  
                    Serial.println("Ingreso correcto por sensor de sonido");
                    accesoConcedido();
                    aperturaPuerta();
                    bandera3 = false;
                    bandera2 = false;
                    bandera = false;
                    
                }
                
            }  
        
      }
      
        
      
}
      
    
  


void aperturaPuerta(){
    
    
    for (int pos = 90; pos <= 180; pos++){
        
        servoMotor.write(pos);
        delay(15);
    }

    
}

void cierrePuerta(){
  
    for (int pos = 180; pos >= 90; pos--){
            
            servoMotor.write(pos);
            delay(15);
    }
}

void reinicioSensorSonido(){
  
      bandera2 = true;
      bandera3 = true;
      contador = 0;  
}

void controlTiempo(){


     Serial.println("INGRESO A CONTROL TIEMPO");
     Serial.println(contadorControlTiempo);
     contadorControlTiempo+=1;

     if (contadorControlTiempo > 300){
      
          contador=0; 
          LCD_ControlTiempo();
          delay(3000);
          contadorControlTiempo = 0;
          accesoPorSonido();
          
     }
  
}

void estadoPuerta(){

      valorDigital = digitalRead(pinDigital);

      if (valorDigital == HIGH){

            digitalWrite(led2, HIGH);
            Serial.println("Campo Magnetico detectado");
            Serial.println("Puerta CERRADA");
            bandera = true;
            indice = 0;
            cierrePuerta();
            cerrarSesion();
            delay(4000);
            apagadoPantalla();
            reinicioSensorSonido();
      }
      else{
        
            digitalWrite(led2, LOW);
            Serial.println("Campo Magnetico NO detectado");
            Serial.println("Puerta ABIERTA");
      }

      valorAnalogo = analogRead(pinAnalogo);
      Serial.println(valorAnalogo);
  
}


void registroTecladoSD(){

      archivo = SD.open("DATOS.txt", FILE_WRITE);
      contadorRegistro+=1;
      if (archivo){

            Serial.println("Creacion de archivo exitoso");
            archivo.print("INGRESO DE ADMINISTRADOR POR TECLADO");
            archivo.close();
      }
      else{
            Serial.println("Error al crear archivo");
      }
}
