#include <WiFi.h>
#include <EMailSender.h>
#include <ZMPT101B.h>

const char* ssid = "YourWiFi";
const char* password = "Password";

// Gmail SMTP configuration
EMailSender emailSend("cmtelco.ct@gmail.com", 
                      "bcvx ikng kgko robg",
                      "cmtelco.ct@gmail.com", 
                      "ALERTA DE PRUEBA",
                      "smtp.gmail.com", 
                      465);  // Port 587 for STARTTLS

// SENSOR ZMPT101B:
ZMPT101B voltagesensor(34);
unsigned int dcOffsetSamples = 0;

 // Sensitivity = (Esp32 AC inputs peak volts)/(VAC input peak volts)
 // 1.4/232 = 0.0060344827586207

 float Sensitivity = 0.0060344827586207;
 float ADCScale = 1023.0;
 float Vref = 6.0;

// Functions
void sendEmailAlert(float voltage);
float readACVoltage(int pin);
void ConectionWiFi();

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  
}

void loop() {
  ConectionWiFi();

  float voltage = readACVoltage(sensorPin);
  Serial.printf("Voltaje AC: %.2f V\n", voltage);

  if (voltage > voltageThreshold) {
    sendEmailAlert(voltage);
    delay(60000);
  }

  delay(5000);
}

void ConectionWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Wi-Fi activo");
  } else {
    Serial.println("❌ Wi-Fi desconectado");
  }
}

float readACVoltage(int pin){
  int samples = 1000;
  float sum = 0;
  for (int i = 0; i < samples; i++) {
    int raw = analogRead(pin);
    float voltage = (raw / 4095.0) * 3.3; // ADC de 12 bits
    sum += voltage;
    delayMicroseconds(100);
  }
  float avg = sum / samples;
  return avg * 100.0; 
}

void sendEmailAlert(float voltage){
    // Prepare email message
  EMailSender::EMailMessage message;
  message.subject = "⚠️ Alerta de voltaje AC";
  message.message =  "Se ha detectado un voltaje de " + String(voltage, 2) + " V, que supera el umbral de seguridad.";;
  
  // Send email
  EMailSender::Response resp = emailSend.send("diegoalexbsc@gmail.com", message);
  
  Serial.println("Sending status: ");
  Serial.println(resp.code);
  Serial.println(resp.desc);
  Serial.println(resp.status);
}
