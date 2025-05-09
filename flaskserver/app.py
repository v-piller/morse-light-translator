from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import socket
import atexit
from zeroconf import ServiceInfo, Zeroconf
import logging # Optionnel: pour voir les logs détaillés de zeroconf

app = Flask(__name__, static_folder="static", static_url_path="")
CORS(app)                       # enable CORS *once* on the real app

stored_message = ""             # keeps the last message only

# --- Variables globales pour mDNS ---
zeroconf_instance = None
service_info = None

# --- Fonction pour obtenir l'IP locale (améliorée) ---
# --- Fonction pour obtenir l'IP locale (Version SIMPLIFIÉE - IP codée en dur) ---
def get_local_ip():
    # Adresse IP typique du Mobile Hotspot sous Windows
    # Vérifiez avec `ipconfig` dans cmd si nécessaire, mais c'est souvent celle-ci.
    hotspot_ip = "192.168.137.1"
    print(f"Utilisation de l'adresse IP codée en dur pour le hotspot: {hotspot_ip}")
    return hotspot_ip

# --- Fonction pour enregistrer le service mDNS ---
def register_mdns_service(port=5000):
    global zeroconf_instance, service_info

    # Optionnel: Activer les logs de zeroconf pour le débogage
    # logging.basicConfig(level=logging.DEBUG)
    # logging.getLogger('zeroconf').setLevel(logging.DEBUG)

    service_type = "_http._tcp.local."
    # Nom de l'instance de service (doit correspondre à ce que l'ESP32 recherche)
    service_name_instance = "morse-server"
    full_service_name = f"{service_name_instance}.{service_type}"
    # Nom d'hôte optionnel pour le serveur
    server_hostname = "morse-server.local"

    local_ip = get_local_ip()
    local_ip_packed = socket.inet_aton(local_ip) # Convertit l'IP en format binaire

    print(f"--- Enregistrement du Service mDNS ---")
    print(f"  Nom complet: {full_service_name}")
    print(f"  IP: {local_ip}:{port}")
    print(f"  Nom d'hôte: {server_hostname}")
    print(f"------------------------------------")

    # Création de l'objet ServiceInfo
    service_info = ServiceInfo(
        type_=service_type,
        name=full_service_name,
        addresses=[local_ip_packed], # Liste des adresses IP binaires
        port=port,
        properties={}, # Propriétés optionnelles (dictionnaire)
        server=server_hostname, # Nom d'hôte du serveur
    )

    # Création de l'instance Zeroconf et enregistrement
    zeroconf_instance = Zeroconf()
    zeroconf_instance.register_service(service_info)
    print("Service mDNS enregistré avec succès.")

    # Fonction de nettoyage qui sera appelée à la sortie du script
    def cleanup_mdns():
        print("\n--- Désenregistrement du service mDNS ---")
        if zeroconf_instance and service_info:
            try:
                zeroconf_instance.unregister_service(service_info)
                zeroconf_instance.close()
                print("Service mDNS désenregistré et fermé.")
            except Exception as e:
                print(f"Erreur lors du nettoyage mDNS: {e}")

    # Enregistrement de la fonction de nettoyage
    atexit.register(cleanup_mdns)

# ---------- API ROUTES ----------
# ---------- API ROUTES ----------
@app.route("/send", methods=["POST"])
def send():
    global stored_message
    msg     = request.form.get("msg", "")
    sender  = request.form.get("sender", "unknown")  # <—

    stored_message = {"msg": msg, "sender": sender}  # stocke un dict
    print(f"Reçu de {sender}: {msg}")
    return "OK"

@app.route("/get", methods=["GET"])
def get():
    return jsonify(stored_message)

# ---------- (Optional) Serve the front-end from the same origin ----------
@app.route("/")
def home():
    # Assumes  index.html  lives in   ./static/index.html
    return send_from_directory("static", "index.html")


if __name__ == "__main__":
    flask_port = 5000 # Port utilisé par Flask

    # Enregistrer le service mDNS avant de lancer Flask
    try:
        register_mdns_service(port=flask_port)
    except Exception as e:
        print(f"ERREUR: Impossible d'enregistrer le service mDNS: {e}")
        print("Le serveur Flask démarrera sans annonce mDNS.")

    # Lancer le serveur Flask
    print(f"\nDémarrage du serveur Flask sur 0.0.0.0:{flask_port}...")
    # Note: debug=True peut parfois interférer avec atexit sur certaines plateformes
    # Si le nettoyage ne fonctionne pas, essayez avec debug=False
    app.run(host="0.0.0.0", port=flask_port, debug=True)
