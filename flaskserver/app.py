from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
from datetime import datetime
import socket, atexit, logging
from zeroconf import ServiceInfo, Zeroconf
import serial
import serial.tools.list_ports
import time

# === Port série : ouverture avec attente automatique ===
def try_open_serial(port='COM3', baudrate=9600, timeout=1, retries=5):
    for i in range(retries):
        try:
            return serial.Serial(port, baudrate, timeout=timeout)
        except Exception as e:
            print(f"⏳ Tentative {i+1}/{retries} : COM3 indisponible ({e})")
            time.sleep(1)
    return None

# === Initialisation port série ===
serial_port = try_open_serial()
if serial_port:
    print("✅ Port COM3 ouvert après attente.")
else:
    print("❌ Échec ouverture COM3 après plusieurs tentatives.")

# === Flask app ===
app = Flask(__name__, static_folder="static", static_url_path="")
CORS(app)

sent_messages = []
received_messages = []

# === mDNS Zeroconf ===
zeroconf_instance = None
service_info = None

def get_timestamp():
    return datetime.now().strftime("%H:%M:%S")

def get_local_ip():
    return "192.168.137.1"

def register_mdns_service(port=5000):
    global zeroconf_instance, service_info
    service_type = "_http._tcp.local."
    service_name_instance = "morse-server"
    full_service_name = f"{service_name_instance}.{service_type}"
    server_hostname = "morse-server.local"
    local_ip_packed = socket.inet_aton(get_local_ip())

    service_info = ServiceInfo(
        type_=service_type,
        name=full_service_name,
        addresses=[local_ip_packed],
        port=port,
        properties={},
        server=server_hostname,
    )
    zeroconf_instance = Zeroconf()
    zeroconf_instance.register_service(service_info)

    def cleanup_mdns():
        if zeroconf_instance and service_info:
            try:
                zeroconf_instance.unregister_service(service_info)
                zeroconf_instance.close()
                print("✅ Service mDNS désenregistré proprement.")
            except Exception as e:
                print(f"⚠️ Erreur mDNS cleanup: {e}")

    atexit.register(cleanup_mdns)

# === ROUTES API ===
@app.route("/send", methods=["POST"])
def send():
    from_who = request.form.get("from", "user")
    msg = request.form.get("msg", "").strip()
    if not msg:
        return "Empty message", 400

    timestamp = get_timestamp()
    message_obj = {"text": msg, "from": from_who, "timestamp": timestamp}

    if from_who == "user":
        sent_messages.append(message_obj)
    else:
        received_messages.append(message_obj)
        print(f"📩 Nouveau message reçu ({msg}) — envoi au microcontrôleur...")
        if serial_port:
            try:
                serial_port.write((msg + "\n").encode())
                print("✅ Message envoyé via Serial.")
            except Exception as e:
                print(f"⚠️ Erreur d'envoi série: {e}")

    return "OK"

@app.route("/get", methods=["GET"])
def get():
    return jsonify({
        "sent": sent_messages,
        "received": received_messages
    })

@app.route("/")
def home():
    return send_from_directory("static", "index.html")

# === Lancer serveur Flask + mDNS ===
if __name__ == "__main__":
    try:
        register_mdns_service(port=5000)
    except Exception as e:
        print(f"⚠️ mDNS non activé: {e}")

    print("🚀 Démarrage serveur Flask...")
    app.run(host="0.0.0.0", port=5000, debug=True)
