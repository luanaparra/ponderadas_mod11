#include <Arduino.h>
#include <esp_camera.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FreeRTOS.h>

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const char* ssid = "Chile_5G";
const char* password = "senha";

SemaphoreHandle_t imageSemaphore;

WebServer server(80);

// capturar imagem 
void captureImageTask(void *pvParameters) {
    while (true) {
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Falha ao capturar imagem");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        xSemaphoreGive(imageSemaphore);
        Serial.println("Imagem capturada");
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);  
    }
}

// enviar a imagem via HTTP
void sendImageTask(void *pvParameters) {
    while (true) {
        if (xSemaphoreTake(imageSemaphore, portMAX_DELAY)) {
            camera_fb_t * fb = esp_camera_fb_get();
            if (fb) {
                server.send(200, "image/jpeg", (const char *)fb->buf, fb->len);
                esp_camera_fb_return(fb);
                Serial.println("Imagem enviada via HTTP!");
            } else {
                Serial.println("Falha ao capturar a imagem para envio");
            }
        }
    }
}

// receber dados de detecção via Serial
void receiveDetectionTask(void *pvParameters) {
    while (true) {
        if (Serial.available() > 0) {
            String detectionData = Serial.readString();
            Serial.println("Dados de detecção recebidos: " + detectionData);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void initCamera() {
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
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    
    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Erro ao inicializar a câmera: 0x%x", err);
        return;
    }
}

void handleCapture() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Falha ao capturar a imagem");
        server.send(500, "text/plain", "Erro ao capturar a imagem");
        return;
    }
    
    server.send(200, "image/jpeg", (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
}

void setup() {
    Serial.begin(115200);

    initCamera();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao Wi-Fi...");
    }
    Serial.println("Conectado ao Wi-Fi");

    server.on("/capture", handleCapture);
    server.begin();

    imageSemaphore = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(captureImageTask, "Capture Image Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendImageTask, "Send Image Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(receiveDetectionTask, "Receive Detection Task", 2048, NULL, 1, NULL, 1);
}

void loop() {
    server.handleClient(); 
}
