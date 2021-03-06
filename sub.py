import paho.mqtt.client as mqtt
import json
import os
import logging
from logging.handlers import TimedRotatingFileHandler

path = os.environ['logger_path']

formatter = logging.Formatter('%(asctime)s - %(message)s')
handler = TimedRotatingFileHandler(path, when="D", interval=1, backupCount=30)
handler.setFormatter(formatter)

log = logging.getLogger(__name__)
log.setLevel(logging.INFO)
log.addHandler(handler)

# The callback function of connection
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("sensor/json")

# The callback function for received message
def on_message(client, userdata, msg):
    # print(type(msg.payload)) -> bytes
    txt = msg.payload.decode("utf-8")
    d = json.loads(txt)
    # troubleshoot
    log.info(d)
    # for node exporter to pick up
    dump2(d)

def dump2(d):
    import os
    print(str(d))
    client = d["client"]
    path = '/exporter'
    cmd = "echo" + " 'arduino_luminosity{{client=\"{client}\"}} {lumen:.2f}' ".format(client=client, lumen=d["lumen"]) + " > " + " {}/{}_lumen.prom ".format(path, client)
    print(cmd)
    os.system(cmd)
    
    cmd = "echo" + " 'arduino_air_temperature{{client=\"{client}\"}} {temp:.2f}' ".format(client=client, temp=d["temp"]) + " > " + " {}/{}_temp.prom ".format(path, client)
    print(cmd)
    os.system(cmd)

    cmd = "echo" + " 'arduino_air_humidity{{client=\"{client}\"}} {humidity:.2f}' ".format(client=client, humidity=d.get("humidity", 0)) + " > " + " {}/{}_humidity.prom ".format(path, client)
    print(cmd)
    os.system(cmd)

clientId = "subscriber"
client = mqtt.Client(clientId)
client.on_connect = on_connect
client.on_message = on_message
client.connect("IP_ADDRESS", 1883, 60)
client.loop_forever()

