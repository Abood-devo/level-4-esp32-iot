#include <SD.h>
#include <SPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>

// Network credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Pin definitions
#define DHTPIN 4
#define DHTTYPE DHT11  // Change to DHT22 if using that sensor
#define SD_CS 5
#define LED_PIN 2

// Create objects
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Variables
unsigned long lastLogTime = 0;
const unsigned long logInterval = 10000; // Log every 10 seconds
int logCounter = 0;

// HTML webpage (same as before)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>Data Logger Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            text-align: center;
            background-color: #f0f0f0;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        h2 {
            color: #333;
        }
        .button {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
            border-radius: 4px;
            transition: background-color 0.3s;
        }
        .button:hover {
            background-color: #45a049;
        }
        .status {
            margin: 20px 0;
            padding: 10px;
            border-radius: 4px;
        }
        .chart-container {
            margin: 20px auto;
            width: 100%;
            height: 400px;
        }
        #lastUpdate {
            color: #666;
            font-size: 0.9em;
            margin-top: 10px;
        }
        .current-values {
            display: flex;
            justify-content: center;
            gap: 20px;
            margin: 20px 0;
        }
        .value-box {
            background-color: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            min-width: 150px;
        }
    </style>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <div class="container">
        <h2>ESP32 Data Logger Dashboard</h2>
        
        <div class="current-values">
            <div class="value-box">
                <h3>Temperature</h3>
                <div id="currentTemp">--</div>
            </div>
            <div class="value-box">
                <h3>Humidity</h3>
                <div id="currentHum">--</div>
            </div>
        </div>

        <a href="/download" class="button">Download CSV</a>
        <a href="#" class="button" onclick="updateChart()">Refresh Data</a>

        <div class="chart-container">
            <canvas id="sensorChart"></canvas>
        </div>

        <div id="lastUpdate"></div>
    </div>

    <script>
        let chart;

        async function updateChart() {
            try {
                const response = await fetch('/data');
                const data = await response.text();
                const rows = data.split('\n');
                const labels = [];
                const tempData = [];
                const humData = [];
                
                for(let i = 1; i < rows.length; i++) {
                    const cols = rows[i].split(',');
                    if(cols.length === 3) {
                        labels.push(cols[0]);
                        tempData.push(parseFloat(cols[1]));
                        humData.push(parseFloat(cols[2]));
                    }
                }

                // Update current values
                if(tempData.length > 0 && humData.length > 0) {
                    document.getElementById('currentTemp').textContent = 
                        tempData[tempData.length-1].toFixed(1) + '°C';
                    document.getElementById('currentHum').textContent = 
                        humData[humData.length-1].toFixed(1) + '%';
                }

                if(chart) {
                    chart.destroy();
                }

                const ctx = document.getElementById('sensorChart').getContext('2d');
                chart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [{
                            label: 'Temperature (°C)',
                            data: tempData,
                            borderColor: 'rgb(255, 99, 132)',
                            tension: 0.1,
                            fill: false
                        },
                        {
                            label: 'Humidity (%)',
                            data: humData,
                            borderColor: 'rgb(54, 162, 235)',
                            tension: 0.1,
                            fill: false
                        }]
                    },
                    options: {
                        responsive: true,
                        maintainAspectRatio: false,
                        scales: {
                            y: {
                                beginAtZero: false
                            }
                        },
                        plugins: {
                            legend: {
                                position: 'top',
                            }
                        }
                    }
                });

                document.getElementById('lastUpdate').textContent = 
                    'Last updated: ' + new Date().toLocaleTimeString();

            } catch (error) {
                console.error('Error updating chart:', error);
            }
        }

        // Initial load
        updateChart();

        // Update every 10 seconds
        setInterval(updateChart, 10000);
    </script>
</body>
</html>
)rawliteral";

// LED indicator functions
void blinkSuccess() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

void blinkError(int times) {
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

// Web server handler functions
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleDownload() {
  File dataFile = SD.open("/sensor_data.csv", FILE_READ);
  if (dataFile) {
    server.sendHeader("Content-Type", "text/csv");
    server.sendHeader("Content-Disposition", "attachment; filename=sensor_data.csv");
    server.streamFile(dataFile, "text/csv");
    dataFile.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

void handleData() {
  File dataFile = SD.open("/sensor_data.csv", FILE_READ);
  if (dataFile) {
    server.streamFile(dataFile, "text/csv");
    dataFile.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

// Data logging function
void logData() {
  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    blinkError(1);
    return;
  }

  // Create data string
  logCounter++;
  String dataString = String(logCounter) + "," + 
                     String(temperature) + "," + 
                     String(humidity);

  // Open file and write data
  File dataFile = SD.open("/sensor_data.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    blinkSuccess();
    
    // Print to Serial Monitor
    Serial.println("Saved: " + dataString);
  } else {
    Serial.println("Error opening sensor_data.csv");
    blinkError(1);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Initialize DHT sensor
  dht.begin();
  
  // Initialize SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    blinkError(2);
    return;
  }
  Serial.println("SD card initialized successfully!");

  // Create or open data file and write header
  File dataFile = SD.open("/sensor_data.csv", FILE_WRITE);
  if (dataFile) {
    if(dataFile.size() == 0) {
      dataFile.println("Reading,Temperature,Humidity");
    }
    dataFile.close();
    blinkSuccess();
  } else {
    Serial.println("Error opening sensor_data.csv");
    blinkError(3);
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/download", HTTP_GET, handleDownload);
  server.on("/data", HTTP_GET, handleData);
  
  server.begin();
}

void loop() {
  server.handleClient();
  
  if (millis() - lastLogTime >= logInterval) {
    logData();
    lastLogTime = millis();
  }
}