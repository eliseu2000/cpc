//Código que detecta o movimento com sensor PIR no arduino e o nível de luminosidade! Sendo que quando quando têm a presença de alguem no local, ele faz um alarme sonoro tocar!

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Wokwi-GUEST";
const char* pass = "";

const long interval = 2000; // Intervalo de 2 segundos para envio de dados
unsigned long previousMillis = 0;

const long ledInterval = 15000; // Intervalo de 15 segundos para alternar o LED
unsigned long previousLedMillis = 0;
bool ledState = false; // Estado inicial do LED

// Definição das portas
const int pirPin = 13; // Porta digital para o sensor PIR
const int ldrPin = 34; // Porta analógica para o LDR
const int ledPin = 12; // Porta digital para o LED
const int buzzerPin = 14; // Porta digital para o buzzer

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());

  // Configuração dos pinos
  pinMode(pirPin, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // Alterna o estado do LED a cada 15 segundos
  if (currentMillis - previousLedMillis >= ledInterval) {
    previousLedMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
  }

  // Envio de dados a cada 2 segundos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int pirState = digitalRead(pirPin);
    int ldrValue = analogRead(ldrPin);

    // Ativa o buzzer se o PIR detectar movimento
    if (pirState == HIGH) {
      digitalWrite(buzzerPin, HIGH);
      delay(1000); // Buzzer soa por 1 segundo
      digitalWrite(buzzerPin, LOW);
    }

    Serial.print("LDR Value: ");
    Serial.println(ldrValue);
    Serial.print("Normalized LDR Value (entrada_analogica): ");
    float entradaAnalogica = (ldrValue / 4095.0) * 100.0;
    Serial.println(entradaAnalogica, 1);
    Serial.print("PIR State: ");
    Serial.println(pirState);

    // Criação da URL com os dados do sensor PIR e LDR
    String numero_serial = "5";
    String entrada_analogica_str = String(entradaAnalogica, 1); // Converte para string com 1 casa decimal
    String url = "https://eliseu2000.pythonanywhere.com/atualizacao_dados/" + numero_serial + "/" + entrada_analogica_str + "/" + String(pirState) + "/";

    HTTPClient http;
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(payload);

      // Processa a resposta JSON
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      // Extração dos valores de saida_analogica e saida_digital
      int saidaAnalogica = doc["saida_analogica"];
      int saidaDigital = doc["saida_digital"];

      Serial.println(saidaAnalogica);
      Serial.println(saidaDigital);

    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);

      // Tente novamente se ocorrer um erro
      delay(1000); // Atraso de 1 segundo antes de tentar novamente
      httpResponseCode = http.GET();
      if (httpResponseCode == 200) {
        String payload = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(payload);

        // Processa a resposta JSON
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }

        // Extração dos valores de saida_analogica e saida_digital
        int saidaAnalogica = doc["saida_analogica"];
        int saidaDigital = doc["saida_digital"];

        Serial.println(saidaAnalogica);
        Serial.println(saidaDigital);
      } else {
        Serial.print("Retry failed with HTTP request error: ");
        Serial.println(httpResponseCode);
      }
    }
    http.end();
  }
}
