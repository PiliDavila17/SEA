#include <WiFi.h>
#include <PubSubClient.h>

// Set the network SSID/Password
const char* ssid = "lapdelguapoese";
const char* password = "esbienguapo";

// Add mosquitto MQTT Broker IP address
const char* mqtt_server = "192.168.137.1";

// Define
WiFiClient espClient;
PubSubClient client(espClient);

// Define los pines de los semáforos
const int semaforoPins[6][2] = {
  {13, 26}, // Semaforo 1 (Verde, Rojo)
  {12, 25}, // Semaforo 2 (Verde, Rojo)
  {14, 33}, // Semaforo 3 (Verde, Rojo)
  {27, 32}, // Semaforo 4 (Verde, Rojo)
  {22, 23}, // Semaforo 5 (Verde, Rojo)
  {4, 18}   // Semaforo 6 (Verde, Rojo)
};

unsigned int semaforoActual = 0; // Índice del semáforo actual (0-5)

unsigned long lastMsg = 0;

// Set-up configurations
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callbackSemaforos);

  // Configura los pines de los semáforos como salida
  for (int i = 0; i < 6; i++) {
    pinMode(semaforoPins[i][0], OUTPUT); // Verde
    pinMode(semaforoPins[i][1], OUTPUT); // Rojo
  }
}

// Connecting to WiFi network
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback for MQTT received messages
void callbackSemaforos(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  // Si un mensaje se recibe en un tópico de semáforo, verifica y cambia el estado del semáforo actual
  if (String(topic) == "esp32/Semaforo" && messageTemp != "0") {
    cambiarEstadoSemaforo(messageTemp, String(topic));
    client.publish("esp32/Semaforo", "0");
  }
}


// Cambia el estado del semáforo actual según el mensaje recibido
void cambiarEstadoSemaforo(String estado, String topic) {
  Serial.print("Ambulance comming by light: " + estado);

  // Apaga todos los semáforos antes de encender el correspondiente
  for (int i = 0; i < 6; i++) {
    digitalWrite(semaforoPins[i][0], LOW);
    digitalWrite(semaforoPins[i][1], HIGH);
  }

  if (estado == "1") {
    // Código para Semaforo 1
    client.publish("esp32/message", "1");
    digitalWrite(semaforoPins[0][0], HIGH); // Verde
    digitalWrite(semaforoPins[0][1], LOW);  // Rojo
    delay(2000);
  } else if (estado == "2") {
    // Código para Semaforo 2
    client.publish("esp32/message", "2");
    digitalWrite(semaforoPins[1][0], HIGH);
    digitalWrite(semaforoPins[1][1], LOW);
    delay(2000);
  } else if (estado == "3") {
    // Código para Semaforo 3
    client.publish("esp32/message", "3");
    digitalWrite(semaforoPins[2][0], HIGH);
    digitalWrite(semaforoPins[2][1], LOW);
    delay(2000);
  } else if (estado == "4") {
    // Código para Semaforo 4
    client.publish("esp32/message", "4");
    digitalWrite(semaforoPins[3][0], HIGH);
    digitalWrite(semaforoPins[3][1], LOW);
    delay(2000);
  } else if (estado == "5") {
    // Código para Semaforo 5
    client.publish("esp32/message", "5");
    digitalWrite(semaforoPins[4][0], HIGH);
    digitalWrite(semaforoPins[4][1], LOW);
    delay(2000);
  } else if (estado == "6") {
    // Código para Semaforo 6
    client.publish("esp32/message", "6");
    digitalWrite(semaforoPins[5][0], HIGH);
    digitalWrite(semaforoPins[5][1], LOW);
    delay(2000);
  }
  // Apaga todos los semáforos antes de encender el correspondiente
  for (int i = 0; i < 6; i++) {
    digitalWrite(semaforoPins[i][0], LOW);
    digitalWrite(semaforoPins[i][1], HIGH);
  }
}



// Connecting to MQTT broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe to topics
      client.subscribe("esp32/Semaforo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();

  Serial.println("Sending message on topic: esp32/message");
  client.publish("esp32/message", "(^_^) Apagado (^_^)");

  // Cambia el semáforo actual cada 3 segundos
  if (now - lastMsg > 3000) {
    lastMsg = now;

    // Apaga el semáforo actual
    digitalWrite(semaforoPins[semaforoActual][0], LOW); // Verde
    digitalWrite(semaforoPins[semaforoActual][1], HIGH); // Rojo

    // Incrementa el índice del semáforo actual y asegúrate de que esté en el rango 0-5
    semaforoActual = (semaforoActual + 1) % 6;

    // Enciende el nuevo semáforo en verde
    digitalWrite(semaforoPins[semaforoActual][0], HIGH); // Verde
    digitalWrite(semaforoPins[semaforoActual][1], LOW);  // Rojo
    client.publish("esp32/funcionamiento", String(semaforoActual + 1).c_str());
  }
}
