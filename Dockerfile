FROM python:3.7-alpine
RUN mkdir -p /app
RUN pip install paho-mqtt prometheus_client  
COPY __init__.py /app/__init__.py
COPY sub.py /app/sub.py
COPY pub.py /app/pub.py
WORKDIR /app