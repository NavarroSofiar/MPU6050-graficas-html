#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include<Wire.h>

//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "35.247.218.237";
const int mqtt_port = 1883;
const char *mqtt_user = "admin";
const char *mqtt_pass = "public";
const char *root_topic_subscribe = "ESP32";
const char *Acelx = "IoT/ACX";
const char *Acely = "IoT/ACY";
const char *Acelz = "IoT/ACZ";
const char *Girox = "IoT/GYX";
const char *Giroy = "IoT/GYY";
const char *Giroz = "IoT/GYZ";


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "CasaNavarro";
const char* password =  "casanavarro1035";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
long count=0;
unsigned long previousMillis = 0;

long tiempoActual=0 ;
long tiempoAnterior =0 ;
int intervalo = 2000;
char AcX_char[50];
char AcY_char[50];
char AcZ_char[50];
char GyX_char[50];
char GyY_char[50];
char GyZ_char[50];
char tiempo_char[50];

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,GyX,GyY,GyZ;

char valueStr[15];
String strtemp = "";


//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
void init_MPU6050();

void setup() {

   //prepara GPI13 y 12 como salidas 
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  init_MPU6050();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
}

void loop() {

   Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6,true);  // request a total of 6 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x43);  // starting with register  0x43 (GYRO_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6,true);  // request a total of 6 registers 
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)

  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
 

tiempoActual = millis();
  if ((tiempoActual - tiempoAnterior) >= intervalo){
   
    itoa( tiempoActual,tiempo_char,10);
    itoa( AcX,AcX_char,10); 

            

tiempoAnterior = tiempoActual; 
  }

  
  if (!client.connected()) {
		reconnect();
	}

  if (client.connected()){
    //String str = "La cuenta es -> " + String(count);
    //str.toCharArray(msg,25);
   // client.publish(root_topic_publish,msg);
    //count++;
   // delay(300);

	unsigned long currentMillis = millis();
    
  if (currentMillis - previousMillis >= 200) { //envia la temperatura cada 10 segundos
    previousMillis = currentMillis;
    
    strtemp.toCharArray(valueStr, 15);
    itoa( AcX,AcX_char,10);
    Serial.println("Enviando X: [" +  String(AcX) + "] " );
    client.publish(Acelx, AcX_char);

    strtemp.toCharArray(valueStr, 15);
    itoa( AcY,AcY_char,10);
    Serial.println("Enviando  Y: [" +  String(AcY) + "] " );
    client.publish(Acely, AcY_char);

    strtemp.toCharArray(valueStr, 15);
    itoa( AcZ,AcZ_char,10);
    Serial.println("Enviando  Z: [" +  String(AcY) + "] " );
    client.publish(Acelz, AcZ_char);   
   
    strtemp.toCharArray(valueStr, 15);
    itoa( GyX,GyX_char,10);
    Serial.println("Enviando  Z: [" +  String(GyX) + "] " );
    client.publish(Girox, GyX_char); 

    strtemp.toCharArray(valueStr, 15);
    itoa( GyY,GyY_char,10);
    Serial.println("Enviando  Z: [" +  String(GyY) + "] " );
    client.publish(Giroy, GyY_char);  

    strtemp.toCharArray(valueStr, 15);
    itoa( GyZ,GyZ_char,10);
    Serial.println("Enviando  Z: [" +  String(GyZ) + "] " );
    client.publish(Giroz, GyZ_char);   
   
  }
  
  }
  client.loop();
}

//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
	delay(10);
	// Nos conectamos a nuestra red Wifi
	Serial.println();
	Serial.print("Conectando a ssid: ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("Conectado a red WiFi!");
	Serial.println("Dirección IP: ");
	Serial.println(WiFi.localIP());
}
//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

	while (!client.connected()) {
		Serial.print("Intentando conexión Mqtt...");
		// Creamos un cliente ID
		String clientId = "SofiESP32";
		clientId += String(random(0xffff), HEX);
		// Intentamos conectar
		if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
			Serial.println("Conectado!");
			// Nos suscribimos
			if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
		} else {
			Serial.print("falló :( con error -> ");
			Serial.print(client.state());
			Serial.println(" Intentamos de nuevo en 5 segundos");
			delay(5000);
		}
	}
}


//*****************************
//***       CALLBACK        ***
//*****************************

 void callback(char* topic, byte* payload, unsigned int length) {

  char PAYLOAD[5] = "    ";
  
  Serial.print("Mensaje Recibido: [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    PAYLOAD[i] = (char)payload[i];
  }
  Serial.println(PAYLOAD);

  
}
void init_MPU6050(){
  //MPU6050 Initializing & Reset
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);


//MPU6050 Accelerometer Configuration Setting
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1C);  // Accelerometer Configuration register
  //Wire.write(0x00);     // AFS_SEL=0, Full Scale Range = +/- 2 [g]
  //Wire.write(0x08);     // AFS_SEL=1, Full Scale Range = +/- 4 [g]
  Wire.write(0x10);     // AFS_SEL=2, Full Scale Range = +/- 8 [g]
  //Wire.write(0x18);     // AFS_SEL=3, Full Scale Range = +/- 10 [g]
  Wire.endTransmission(true);
  
//MPU6050 Gyroscope Configuration Setting
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B);  // Gyroscope Configuration register
  //Wire.write(0x00);     // FS_SEL=0, Full Scale Range = +/- 250 [degree/sec]
  Wire.write(0x08);     // FS_SEL=1, Full Scale Range = +/- 500 [degree/sec]
  //Wire.write(0x10);     // FS_SEL=2, Full Scale Range = +/- 1000 [degree/sec]
  //Wire.write(0x18);     // FS_SEL=3, Full Scale Range = +/- 2000 [degree/sec]
  Wire.endTransmission(true);

  //MPU6050 Clock Type
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0x03);     // Selection Clock 'PLL with Z axis gyroscope reference'
  Wire.endTransmission(true);

 

 //MPU6050 DLPF(Digital Low Pass Filter)
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1A);  // DLPF_CFG register
  //Wire.write(0x00);     // Accel BW 260Hz, Delay 0ms / Gyro BW 256Hz, Delay 0.98ms, Fs 8KHz 
  //Wire.write(0x01);     // Accel BW 184Hz, Delay 2ms / Gyro BW 188Hz, Delay 1.9ms, Fs 1KHz 
  //Wire.write(0x02);     // Accel BW 94Hz, Delay 3ms / Gyro BW 98Hz, Delay 2.8ms, Fs 1KHz 
  //Wire.write(0x03);     // Accel BW 44Hz, Delay 4.9ms / Gyro BW 42Hz, Delay 4.8ms, Fs 1KHz 
 //Wire.write(0x04);     // Accel BW 21Hz, Delay 8.5ms / Gyro BW 20Hz, Delay 8.3ms, Fs 1KHz 
 //Wire.write(0x05);     // Accel BW 10Hz, Delay 13.8ms / Gyro BW 10Hz, Delay 13.4ms, Fs 1KHz 
  Wire.write(0x06);     // Accel BW 5Hz, Delay 19ms / Gyro BW 5Hz, Delay 18.6ms, Fs 1KHz 
  Wire.endTransmission(true);
}