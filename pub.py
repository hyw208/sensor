import paho.mqtt.client as mqtt
import time
import json

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

# this is a fake publisher if you don't have arduino to pump sensor data
clientId = "publisher"
client = mqtt.Client(clientId)
client.on_connect = on_connect
client.connect("IP_ADDRESS", 1883, 60)
for i in range(300):
    d = {'client': 'mrk-1010-g1', 'temp': 29.78515625+i, 'humidity': 33.93259048+i, 'lumen': 179+i}
    txt = json.dumps(d)
    client.publish('sensor/json', payload=txt, qos=0, retain=False)
    print(f"sent data", str(d))
    time.sleep(3)

#client.loop_forever()
