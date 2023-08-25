#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

// Defina as credenciais da sua rede Wi-Fi
const char* ssid = "****";
const char* password = "****";

// Configuração do display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SDA_PIN 4 // Pino SDA conectado ao GPIO 4
#define SCL_PIN 5 // Pino SCL conectado ao GPIO 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cria uma instância do servidor web na porta 80
ESP8266WebServer server(80);

String message = ""; // Variável para armazenar a mensagem recebida do site

void setup() {
  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao iniciar o display SSD1306"));
    while (true);
  }

  display.clearDisplay(); // Limpa o buffer do display
  display.setTextColor(SSD1306_WHITE); // Define a cor do texto como branco
  display.setTextSize(1); // Tamanho da fonte 1x (6x8)
  display.setCursor(0, 0); // Define a posição inicial do cursor

  // Exibe a mensagem de inicialização no display
  display.println("Iniciando...");
  display.display();

  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Conecta-se à rede Wi-Fi
  Serial.print("Conectando à rede Wi-Fi...");
  display.setCursor(0, 17);
  display.println("Conectando ao Wi-Fi.");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado com o IP: ");
  Serial.println(WiFi.localIP());

  // Exibe a mensagem de conexão bem-sucedida no display
  display.setCursor(0, 27);
  display.print("Conectado ao IP:");
  display.setCursor(0, 37);
  display.print(WiFi.localIP().toString());
  display.display();
  delay(2000);
  
  // Inicializa o sistema de arquivos LittleFS
  if (!LittleFS.begin()) {
    Serial.println(F("Falha ao montar o sistema de arquivos LittleFS"));
    while (true);
  }
  Serial.println(F("Sistema de arquivos LittleFS montado"));

  // Define rotas do servidor web
  server.on("/", handleRoot);
  server.on("/send_message", handleSendMessage);

  // Inicia o servidor web
  server.begin();
  Serial.println("Servidor web iniciado");
  display.setCursor(0, 48);
  display.print("Servidor web iniciado");
  display.display();
  delay(2000);
}

void loop() {
  server.handleClient();

  // Exibe a mensagem do site no display OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Mensagem:");
  display.setCursor(0, 17);
  display.setTextSize(1);
  display.print(message);
  display.display();
}

// Trata a página principal do servidor web
void handleRoot() {
  // Abre o arquivo da página HTML
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(500, "text/plain", "Erro ao carregar a página");
  }
}

// Trata a ação de envio de mensagem do site
void handleSendMessage() {
  if (server.hasArg("message")) {
    message = server.arg("message");
    Serial.println("Mensagem recebida: " + message);
  }
  server.send(200, "text/plain", "Mensagem recebida: " + message);
}
