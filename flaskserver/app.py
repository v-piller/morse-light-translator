from flask import Flask, request, jsonify

app = Flask(__name__)

stored_message = ""  # A variable to keep the last sent message

@app.route("/send", methods=["POST"])
def send():
    global stored_message
    stored_message = request.form.get("msg", "")
    print(f"Message received: {stored_message}")
    return "OK"

@app.route("/get", methods=["GET"])
def get():
    return jsonify({"message": stored_message})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
