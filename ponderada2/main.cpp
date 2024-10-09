#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h> 

const char* ssid = "Chile_5G";  // Nome da rede Wi-Fi
const char* password = "senha";  // Senha da rede Wi-Fi

// Pinos da câmera para ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WebServer server(80);  // Cria um servidor web na porta 80

void startCameraServer();  // Declaração da função

void setup() {
  Serial.begin(115200);

  // Configurar a câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Inicializar a câmera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Erro na inicialização da câmera");
    return;
  }

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi");

  startCameraServer();  // Chama a função para iniciar o servidor
}

void loop() {
  delay(10000);
}

// Implementação da função para iniciar o servidor da câmera
void startCameraServer() {
  // Rota para capturar a imagem
  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request){
    camera_fb_t *fb = esp_camera_fb_get(); // Captura uma imagem da câmera
    if (!fb) {
      Serial.println("Falha ao capturar imagem");
      request->send(500);
      return;
    }

    // Envia a imagem como resposta
    request->send_P(200, "image/jpeg", fb->buf, fb->len); 
    esp_camera_fb_return(fb); // Libera a imagem
  });

  server.begin(); // Inicia o servidor
  Serial.println("Servidor de câmera iniciado");
}
