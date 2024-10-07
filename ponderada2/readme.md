# Ponderada 2 - Visão Computacional
Utilizando um ESP32-CAM para capturar imagens e enviá-las via Wi-Fi para um computador, onde as faces serão detectadas usando OpenCV. O código é desenvolvido com a extensão PlatformIO.

**Arquivos principais:**
- main.cpp: Código para capturar imagens no ESP32-CAM.
- image.py: Script Python para processar e detectar faces nas imagens.
- platformio.ini: Arquivo de configuração do PlatformIO.

## Passos de Instalação e Configuração
1. Configurar o VSCode com PlatformIO
- Abra o VSCode.
- Vá para a aba de Extensões (Ctrl + Shift + X).
- Procure por PlatformIO IDE e instale.
2. Criar o Projeto no PlatformIO
- Clique no ícone do PlatformIO no lado esquerdo.
- Clique em New Project.
- Nomeie o projeto (ESP32_CAM_FaceDetection).
- Selecione ESP32 Dev Module como a placa.
- Escolha o framework Arduino.
3. Instalar dependências do python `pip install opencv-python`
4. Carregando o Código no ESP32-CAM
- Conecte o ESP32-CAM ao ESP32-CAM-MB e ao computador via cabo USB.
- Clique no ícone do PlatformIO e selecione Upload para compilar e carregar o código no ESP32-CAM.
- Abra o Serial Monitor no VSCode para ver o endereço IP que o ESP32-CAM obteve ao se conectar ao Wi-Fi.

Por fim, execute o comando: `python image.py`

A demonstração está na pasta `/demonstracao`