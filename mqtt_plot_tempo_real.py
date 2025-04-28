import paho.mqtt.client as mqtt
import csv
import os
import json
from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt

# === Caminho do CSV ===
csv_dir = "/home/safena/Senna_1/Log"
csv_path = os.path.join(csv_dir, "dados_mqtt.csv")
os.makedirs(csv_dir, exist_ok=True)

# Cria o arquivo com cabeçalho se ainda não existir
if not os.path.exists(csv_path):
    with open(csv_path, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["timestamp", "x", "y", "z"])

# === Inicializa o gráfico ===
plt.ion()
fig, ax = plt.subplots()
line_x, = ax.plot([], [], label='X')
line_y, = ax.plot([], [], label='Y')
line_z, = ax.plot([], [], label='Z')
ax.set_xlabel("Amostras")
ax.set_ylabel("Aceleração (g)")
ax.legend()
plt.tight_layout()

# Buffer para plotagem ao vivo
dados = []

# === Callback MQTT ===
def on_message(client, userdata, msg):
    global dados
    try:
        payload = msg.payload.decode("utf-8").strip()
        data = json.loads(payload)

        x = float(data["x"])
        y = float(data["y"])
        z = float(data["z"])
        timestamp = datetime.now().isoformat()

        # Salva no CSV
        with open(csv_path, mode="a", newline="") as file:
            writer = csv.writer(file)
            writer.writerow([timestamp, x, y, z])

        # Atualiza o buffer
        dados.append((x, y, z))
        if len(dados) > 100:
            dados = dados[-100:]  # Mantém só os últimos 100 pontos

        # Atualiza gráfico
        xs = [d[0] for d in dados]
        ys = [d[1] for d in dados]
        zs = [d[2] for d in dados]
        line_x.set_data(range(len(xs)), xs)
        line_y.set_data(range(len(ys)), ys)
        line_z.set_data(range(len(zs)), zs)
        ax.relim()
        ax.autoscale_view()
        plt.pause(0.001)

        print(f"Salvo: {timestamp}, {x:.4f}, {y:.4f}, {z:.4f}")

    except Exception as e:
        print("Erro ao processar mensagem:", e)

# === Conecta ao broker MQTT ===
client = mqtt.Client(client_id="mqtt_plot_csv", protocol=mqtt.MQTTv311)
client.on_message = on_message
client.connect("192.168.100.45", 1883, 60)
client.subscribe("esp32/adxl355")
print("Conectado. Aguardando e plotando mensagens...")

# === Inicia loop ===
client.loop_forever()
