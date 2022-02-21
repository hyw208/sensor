#!/bin/bash

##################################################
# create needed directories
mkdir -p exporter grafana prometheus

##################################################
# replace ip address, for mac only
echo "What is your local ip address? eg. 192.168.1.156"
read address

sed -i '' "s/IP_ADDRESS/$address/g" ./prometheus.yml
echo "IP addresses in prometheus.yml are replaced"

sed -i '' "s/IP_ADDRESS/$address/g" ./pub.py
echo "IP addresses in pub.py are replaced"

sed -i '' "s/IP_ADDRESS/$address/g" ./sub.py
echo "IP addresses in sub.py are replaced"

sed -i '' "s/IP_ADDRESS/$address/g" ./arduino/sketch_feb20a/sketch_feb20a.ino
echo "IP addresses in sketch_feb20a.ino are replaced"

##################################################
# replace WiFi SSID
echo "What is your WiFi SSID? eg. Verizon-FiOS"
read ssid

sed -i '' "s/ssid/$ssid/g" ./arduino/sketch_feb20a/arduino_secrets.h
echo "SSID in arduino_secrets.h is replaced"

##################################################
# replace WiFi password
echo "What is your WiFi PASS? eg. My-Secure-Password"
read pass

sed -i '' "s/pass/$pass/g" ./arduino/sketch_feb20a/arduino_secrets.h
echo "PASS in arduino_secrets.h is replaced"
