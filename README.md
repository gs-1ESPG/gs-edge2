# 📡 Projeto: Monitoramento de Lixeira com ESP32 + FIWARE + Dashboard (Plotly/Dash)

Este projeto utiliza um **ESP32** com sensor **ultrassônico** para medir a distância dentro de uma lixeira e enviar os dados via **MQTT** para um broker FIWARE. Em seguida, um dashboard (feito em Python / Dash / Plotly) exibe os gráficos em tempo real, incluindo **média móvel**, **estado da lixeira**, e um alerta quando a distância medida indica que ela está cheia.

---

## 🚀 Funcionalidades Principais

* 📏 Leitura de distância com sensor ultrassônico (HC-SR04)
* 📡 Envio dos dados via MQTT para FIWARE (tópicos attrs e attrs/d)
* 🔄 Reconexão automática WiFi e MQTT
* 🚨 Alerta de lixeira cheia (distância abaixo de 4 cm)
* 📊 Dashboard em Python com Plotly/Dash exibindo:

  * Distância bruta
  * Média móvel
  * Linha limite (gatilho)
  * Status atual da lixeira (Cheia / Normal)

---

## 🔧 Hardware Utilizado

* ESP32
* Sensor Ultrassônico HC-SR04
* Conexão WiFi

---

## 🧠 Lógica do ESP32 (Resumo)

O ESP32 mede a distância a cada 5 segundos e envia ao broker MQTT:

* **/TEF/bin001/attrs/d** → distância lida em cm



---

## 🖥️ Dashboard – Exemplos de Saída

### 🔥 **Exemplo 1 — Lixeira Cheia**

![Lixeira cheia](sandbox:/mnt/data/c4f519a2-f4b5-4288-8bff-b4ede89c402c.png)

---

### ✅ **Exemplo 2 — Lixeira Normal**

![Lixeira normal](sandbox:/mnt/data/e958d3ff-cfe7-457c-93b1-853ab82ce40b.png)

---

## 📁 Estrutura do Código do ESP32 (Comentado)

O código contém as seguintes partes principais:

### 🔌 1. **Configuração de WiFi e MQTT**

Conecta ao WiFi, tenta reconectar automaticamente e assina tópicos FIWARE.

### 📡 2. **Publicação dos Dados**

A cada 5 segundos publica a distância lida (`attrs/d`).

### 🎯 3. **Callback MQTT**

Interpreta comandos FIWARE como `bin001@on|`.

### 📏 4. **Leitura do Sensor HC-SR04**

Função `readDistance()` calcula a distância usando o tempo de retorno do pulso.



## Representantes  
- **João Pedro Palmeira** – 563542  
- **Lucas de Almeida Sales da Silva** – 564458  
- **André Ricardo Spinola Castor** – 563964  

## Vídeo demonstração  
[Assista ao vídeo de demonstração](https://youtu.be/MCNJdSynpwg)



Se quiser evoluir o projeto com novos sensores, FIWARE NGSIv2, banco de dados ou analytics, posso te ajudar!
