
/*
 * CODIGO RIEGO 
 * @AUTOR DANIEL VALENCIA, SEBASTIAN MENDEZ
 * @VERSION 1.0
 */

#include <LiquidCrystal.h>
#include <DHT.h>
#include <EEPROM.h>
#include <Wire.h>
#include <DS3231.h>

#define DHTPIN 8
#define DHTTYPE DHT22

DS3231 Clock; // se direcciona automáticamente el hardware
RTClib RTC;

LiquidCrystal lcd(32, 30, 28, 26, 24, 22);    //(A0, A1, A2, A3, 8, 9);   //Pines donde va conectada la pantalla (RS, E, D4, D5, D6, D7)

const int dry = 584; //valor más seco o en el aire
const int wet = 239; //valor del agua
const int pinBuzzer = 9;
int address, dato, i;
int datos[6], clave[4];
int year, month, date, hour, minute, second;
DHT dht(DHTPIN, DHTTYPE);
float humedad, temperatura, humidity, temperature; 
long segundos,auxsegundos;
short flag,espacio_vacio;
char datom;

//Variables para fecha y hora descomentar lineas en caso de reasignar fecha y hora
/*int year = 21;
int month = 11;
int date = 22;
int hour = 13;
int minute = 6;
int second = 0;*/

void setup() {
  pinMode(13, OUTPUT); //esta es el pin para la salida del rele del motor
  pinMode(pinBuzzer, OUTPUT);
  Serial.begin(9600); 
  lcd.begin(16,2); 
  dht.begin(); 
  Wire.begin();

  //Asignar datos solo descomentar en caso de reasignar fecha y hora 
  /*Clock.setClockMode(false);
  Clock.setYear((byte)year);
  Clock.setMonth((byte)month);
  Clock.setDate((byte)date);
  Clock.setHour((byte)hour);
  Clock.setMinute((byte)minute);
  Clock.setSecond((byte)second);*/
}

// DTMF
void dtmf()
{
  byte signal ;  
  signal = digitalRead(7);
  dato = '\0';
  if(signal == HIGH)  
   { 
    delay(100);
    dato = ( 0x00 | (digitalRead(3)<<0) | (digitalRead(4)<<1) | (digitalRead(5)<<2) | (digitalRead(6)<<3) );
      switch (dato)
      {
        case 0x01:
        dato ='1';
        break;
        case 0x02:
        dato ='2';
        break;
        case 0x03:
        dato ='3';
        break;
        case 0x04:
        dato ='4';
        break;
        case 0x05:
        dato ='5';
        break;
        case 0x06:
        dato ='6';
        break;
        case 7:
        dato ='7';
        break;
        case 0x08:
        dato ='8';
        break;
        case 0x09:
        dato ='9';
        break;
        case 0x0A:
        dato ='0';
        break;
        case 0x0B:
        dato ='*';
        break;
        case 0x0C:
        dato ='#';
        break; 
        case 0x0D:
        dato ='A';
        break; 
        case 0x0E:
        dato ='B';
        break; 
        case 0x0F:
        dato ='C';
        break; 
        case 0x00:
        dato ='D';
        break; 
      }     
   }  
}

void un_digitoDTMF()
{
     lcd.clear(); 
     segundos = 0;
     auxsegundos = 0;
     flag = false;
     do{
        dtmf();
        datom = dato;
        delay(60);
        
        if(datom !=('\0'))
        {
         lcd.setCursor(8,1);
         lcd.print(datom);
         datom = datom-0x30;
         flag = true;
        }
        else
        {
         auxsegundos++;
         if(auxsegundos>20)
         {    
             segundos++;
             auxsegundos=0;
         }
        }  
      }while(segundos<6 && flag == false);
}


//Sensor de humedad y temperatura variable
void dht22()
{
  humedad = dht.readHumidity();                           
  temperatura = dht.readTemperature(); 

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temperatura:");
  lcd.println(temperatura);
  
  lcd.setCursor(0,1);
  lcd.print("Humedad:");
  lcd.print(humedad);

  delay(1000);
}

//Riego
void riego()
{
        int sensorVal = analogRead(A0);  //puerto que lee el sensor al entrar en tierra     
        int porcentajeHumedad = map(sensorVal, wet, dry, 100, 0);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" HUMEDAD SUELO ");
        lcd.setCursor(8,1);
        lcd.print(porcentajeHumedad);
        lcd.println("%");
        delay(5000);
        
        if (porcentajeHumedad < 40) // % en donde se inicia el riego
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("SECO! INICIANDO");
          digitalWrite(13, LOW); //porcentaje bajo, señal alta para encender el relé
          delay(5000);
        }else if (porcentajeHumedad > 50) // maximo nivel de humedad
        {   
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("MOJADO! APAGANDO");
          digitalWrite(13, HIGH); //porcentaje alto, señal baja se apaga el relé
          delay(5000);
        }

         delay(1000);
          
       /* if(porcentajeHumedad >= 100)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("POCENTAJE HUMED:");
          lcd.setCursor(0,1);
          lcd.print("      100 %     ");
          delay(5000);
        }else if(porcentajeHumedad <=0)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("POCENTAJE HUMED:");
          lcd.setCursor(0,1);
          lcd.print("       0 %      ");
          delay(5000);
        }else if(porcentajeHumedad >0 &&  porcentajeHumedad < 100)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("POCENTAJE HUMED:");
          lcd.setCursor(0,1);
          lcd.print(porcentajeHumedad);
          lcd.println("%");
          delay(5000);
        }*/
}

void loop() 
{
  digitalWrite(13, HIGH);
  delay(5);
menu: 
    //  digitalWrite(pinBuzzer, HIGH);
      delay(1000);
      digitalWrite(pinBuzzer, LOW);
      /*
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SEBASTIAN MENDEZ");
      lcd.setCursor(0,1);
      lcd.print("DANIEL VALENCIA ");
      delay(2000);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("     GURUPO     ");
      lcd.setCursor(0,1);
      lcd.print("       13       ");
      delay(2000);*/

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" BIENVENIDO AL  ");
      lcd.setCursor(0,1);
      lcd.print(" MENU DE RIEGOS ");
      delay(2000);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" MARQUE 1 PARA  ");
      lcd.setCursor(0,1);
      lcd.print("  MONITOREO DE  ");
      delay(500);
      lcd.setCursor(0,0);
      lcd.print("  MONITOREO DE  ");
      lcd.setCursor(0,1);
      lcd.print("     HUMEDAD    ");
      delay(2000);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" MARQUE 2 PARA  ");
      lcd.setCursor(0,1);
      lcd.print("VER TEMPERATURA ");
      delay(2000);

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" MARQUE 3 PARA  ");
      lcd.setCursor(0,1);
      lcd.print("   VER LA HORA  ");
      delay(2000);
      
      un_digitoDTMF();     
      
      if(datom == 0x01)
      {
rtmn:
        riego();
        delay(250);
        goto rtmn;
      }  

      if(datom == 0x02)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" BIENVENIDO AL  ");
        lcd.setCursor(0,1);
        lcd.print("     MENU DE    ");
        delay(500);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  TEMPERATURA   ");
        lcd.setCursor(0,1);        
        lcd.print(" RELATIVA DHT22 ");
dht22:
        dht22();
        delay(1000);
        goto dht22;
      }
       
      if(datom == 0x03)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" BIENVENIDO AL  ");
        lcd.setCursor(0,1);
        lcd.print("     MENU DE    ");
        delay(500);
        lcd.clear();
        lcd.print("   MENU 3 PARA  ");
        lcd.setCursor(0,1);
        lcd.print("   FECHA Y HORA ");
        delay(500);
        lcd.clear();
fec_hor: 
        DateTime now = RTC.now();
        year = now.year();
        month = now.month();
        date = now.day();
        hour = now.hour();
        minute = now.minute();
        second = now.second();
        
        lcd.setCursor(0,0);
        lcd.print("D:");
        lcd.setCursor(3,0);
        lcd.print(date);
        lcd.print("/");
        lcd.print(month);
        lcd.print("/");
        lcd.print(year,DEC);
      
        lcd.setCursor(0,1);
        lcd.print("H:");
        lcd.setCursor(3,1);
        lcd.print(hour);
        lcd.print(":");
        lcd.print(minute);
        lcd.print(":");
        lcd.print(second);
        delay(1000);
        
        goto fec_hor;
      }
       
      goto menu;
}
