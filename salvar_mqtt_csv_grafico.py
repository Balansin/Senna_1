import csv
import json
import os
import time
import paho.mqtt.client as mqtt

# Caminho para salvar o CSV
base_dir = "/home/safena/Senna_1/Log"
csv_path = os.path.join(base_dir, "dados_mqtt.csv")

# Garante que a pasta existe
os.makedirs(base_dir, exist_ok=True)

# Inicia tempo base
start_time = time.time()

# Cria arquivo CSV com cabeçalho
csv_file = open(csv_path, mode='w', newline='')
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["tempo_s", "x", "y", "z"])
csv_file.flush()

# Callback para mensagens recebidas
def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        x = float(data.get("x"))
        y = float(data.get("y"))
        z = float(data.get("z"))
        t = time.time() - start_time
        print(f"[{t:.1f}s] x={x}, y={y}, z={z}")
        csv_writer.writerow([t, x, y, z])
        csv_file.flush()
    except Exception as e:
        print("Erro ao processar mensagem:", e)

# Configura e conecta o cliente MQTT
client = mqtt.Client(client_id="logger_csv", protocol=mqtt.MQTTv311)
client.on_message = on_message
client.connect("192.168.193.115", 1883)
client.subscribe("esp32/adxl355")

print("Conectado ao broker. Aguardando mensagens...")
client.loop_forever()