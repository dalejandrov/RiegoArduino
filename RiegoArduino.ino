
/*
 * CODIGO RIEGO 
 * @AUTOR DANIEL VALENCIA, SEBASTIAN MENDEZ
 * @VERSION 1.0
 */

#include <LiquidCrystal.h>
#include <DHT.h>
#include <EEPROM.h>
#include <Wire.h>

#define DHTPIN 8
#define DHTTYPE DHT22
#define DS1307_I2C_ADDRESS 0x68

LiquidCrystal lcd(32, 30, 28, 26, 24, 22);    //(A0, A1, A2, A3, 8, 9);   //Pines donde va conectada la pantalla (RS, E, D4, D5, D6, D7)

const int valaire = 550;   //valor más seco o en el aire
const int valagua = 250;  //valor del agua
int valhumedad = 0;
int porcenhumedad=0;
int address, dato, i;
int datos[6], clave[4];
DHT dht(DHTPIN, DHTTYPE);
float humedad, temperatura, humidity, temperature; 
byte second, minute,  hour, dayOfWeek, dayOfMonth, month, year;
long segundos,auxsegundos;
short flag,espacio_vacio;
char datom;

void setup() {
  pinMode(9, OUTPUT); //esta es el pin para la salida del rele del motor
  Serial.begin(9600); 
  lcd.begin(16,2); 
  dht.begin(); 
  Wire.begin();
}

/*
//lector de temperatura lm35
void lectura_lm35()
{
  tempC = analogRead(pinLM35);
  tempC = (5.0 * tempC * 100.0) / 1024.0;

  Serial.print("Temperatura: ");
  Serial.print(tempC);
  Serial.print("\n");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEMPERATURA");
  lcd.setCursor(4,1);
  lcd.print(tempC);
  lcd.setCursor(9,1);
}
*/

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
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  TEMPERATURA   ");
  lcd.setCursor(0,0);
  lcd.print("RELATIVA: ");
  lcd.print(temperatura);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HUMEDAD RELATIVA");
  lcd.setCursor(0,0);
  lcd.print(humedad);

  delay(2000);
}

//Actualizar hora y mostrar hora
void setDateDs1307(byte second,
                   byte minute, 
                   byte hour, 
                   byte dayOfWeek,
                   byte dayOfMonth, 
                   byte month, 
                   byte year)
{
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0);
      Wire.write(decToBcd(second));
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour));
    
    Wire.write(decToBcd(dayOfWeek));
    Wire.write(decToBcd(dayOfMonth));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));
    Wire.endTransmission();
}

void getDateDs1307(byte *second, 
                   byte *minute, 
                   byte *hour, 
                   byte *dayOfWeek, 
                   byte *dayOfMonth,
                   byte *month,
                   byte *year)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
  *second     = bcdToDec(Wire.read() & 0x7F);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3F);
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

void actualizar()
{
  second = 0x00;
  minute = 0x00;
  hour =  0x0E;
  dayOfWeek = 0x07;
  dayOfMonth = 0x12;
  month = 0x0A;
  year = 0x15;
  setDateDs1307(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
}

void reloj()
{
 getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
 datos[0] = dayOfMonth;
 datos[1] = month;     
 datos[2] = year;
 datos[3] = hour;
 datos[4] = minute;
 datos[5] = second;

 for(i=0;i<20;i++)
  {
   EEPROM.write(i,clave[i]);
    delay(5);
    address++;
  }

    //delay(2000);
    lcd.setCursor(0,0);
    lcd.print("DATE: "); 
    lcd.setCursor(7, 0);
    lcd.print(dayOfMonth, DEC); 
    lcd.print("/");
    
    lcd.setCursor(10, 0);
    lcd.print(month, DEC); 
    lcd.print("/");
    
    lcd.setCursor(13, 0);
    lcd.print(year, DEC); 
    lcd.setCursor(15, 0);
  
    lcd.setCursor(0,1);
    lcd.print("HOUR: "); 
    lcd.setCursor(7, 1);
    lcd.print(hour, DEC); 
    lcd.print(":");

    lcd.setCursor(10, 1);
    lcd.print(minute, DEC); 
    lcd.print(":");

    lcd.setCursor(13, 1);
    lcd.print(second, DEC); 
    //lcd.print(":");

    lcd.setCursor(16,1);
    lcd.print(dayOfWeek, DEC);
    lcd.print(":");
    delay(1000);
    lcd.clear();
}

byte decToBcd(byte val)
{
  return((val/10*16) + (val%10));
}

byte bcdToDec(byte val)
{
  return((val/16*10) + (val%16));
}

void loop() 
{
menu: lcd.clear();
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
      delay(2000);

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
      lcd.print("VER TEMPERATURA ");
      delay(500);
      lcd.setCursor(0,0);
      lcd.print("VER TEMPERATURA ");
      lcd.setCursor(0,1);
      lcd.print(" RELATIVA DHT22 ");
      delay(2000);

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" MARQUE 4 PARA  ");
      lcd.setCursor(0,1);
      lcd.print("   VER LA HORA  ");
      delay(2000);
      
      un_digitoDTMF();     
      
      if(datom == 0x01)
      {
        valhumedad = analogRead(A0);  //puerto que lee el sensor al entrar en tierra
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" HUMEDAD SUELO ");
        lcd.setCursor(0,1);
        lcd.print(valhumedad);
        porcenhumedad = map(porcenhumedad, valaire, valagua, 0, 100);
        
        if (porcenhumedad < 40) // % en donde se inicia el riego
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" SECO, INICIANDO");
          digitalWrite(9, HIGH); //porcentaje bajo, señal alta para encender el relé
          delay(5);
        }else if (porcenhumedad > 50) // maximo nivel de humedad
        {   
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("MOJADO, APAGANDO");
          digitalWrite(9, LOW); //porcentaje alto, señal baja se apaga el relé
          delay(5);
        }
      
        if(porcenhumedad >= 100)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("POCENTAJE HUMED:");
          lcd.setCursor(0,1);
          lcd.print("      100 %     ");
          delay(5);
        }else if(porcenhumedad <=0)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("POCENTAJE HUMED:");
          lcd.setCursor(0,1);
          lcd.print("       0 %      ");
          delay(5);
        }else if(porcenhumedad >0 && porcenhumedad < 100)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("POCENTAJE HUMED:");
          lcd.setCursor(0,1);
          lcd.print(porcenhumedad);
          delay(5);
        }
          delay(250);
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
   /*
        lcd.setCursor(0,0);
        lcd.print("  TEMPERATURA   ");
        lcd.setCursor(0,1);        
        lcd.print("  ");
        lectura_lm35();
        delay(2000);
        */
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
        lcd.setCursor(0,0);
        lcd.print("  TEMPERATURA   ");
        lcd.setCursor(0,1);        
        lcd.print(" RELATIVA DHT22 ");
        dht22();
        delay(5);
      }

fec_hor:        
      if(datom == 0x04)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" BIENVENIDO AL  ");
        lcd.setCursor(0,1);
        lcd.print("     MENU DE    ");
        delay(500);
        lcd.clear();
        lcd.print("   MENU 2 PARA  ");
        lcd.setCursor(0,1);
        lcd.print("   FECHA Y HORA ");
        delay(500);
        lcd.clear();
        
        //actualizar();
        reloj();
        delay(100);
        goto fec_hor;
      }
       
      goto menu;
}
