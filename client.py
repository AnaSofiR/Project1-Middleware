import requests
import json

BASE_URL = "http://localhost:8080"
token = ""

def register():
    username = input("👤 Nombre de usuario: ")
    password = input("🔐 Contraseña: ")
    data = {"username": username, "password": password}
    response = requests.post(f"{BASE_URL}/register", json=data)
    print(f"✅ Registro: {response.status_code} - {response.text}")

def login():
    global token
    username = input("👤 Nombre de usuario: ")
    password = input("🔐 Contraseña: ")
    data = {"username": username, "password": password}
    response = requests.post(f"{BASE_URL}/login", json=data)
    if response.status_code == 200:
        token = response.json()["token"]
        print("✅ Login exitoso. Token guardado.")
    else:
        print("❌ Error al iniciar sesión:", response.text)

def create_topic():
    topic_name = input("📝 Nombre del tópico: ")
    user = input("👤 Usuario creador: ")
    response = requests.post(f"{BASE_URL}/createTopic", json={"topic_name": topic_name, "user": user})
    print(f"📌 Crear tópico: {response.status_code} - {response.text}")

def delete_topic():
    topic_name = input("🗑️ Tópico a eliminar: ")
    user = input("👤 Usuario dueño: ")
    response = requests.post(f"{BASE_URL}/deleteTopic", json={"topic_name": topic_name, "user": user})
    print(f"🗑️ Eliminar tópico: {response.status_code} - {response.text}")

def list_topics():
    response = requests.get(f"{BASE_URL}/topics")
    print(f"📚 Listado de tópicos: {response.status_code}")
    if response.ok:
        print("🧾 Tópicos:", response.json()["topics"])

def send_to_topic():
    topic = input("📌 Nombre del tópico: ")
    mensaje = input("✉️ Mensaje: ")
    response = requests.post(f"{BASE_URL}/sendTopic/{topic}", json={"msm": mensaje})
    print(f"📤 Envío a tópico: {response.status_code} - {response.text}")

def receive_from_topic():
    topic = input("📌 Nombre del tópico: ")
    response = requests.get(f"{BASE_URL}/receiveTopic/{topic}")
    print(f"📥 Recibir de tópico: {response.status_code}")
    if response.ok:
        print("📨 Mensajes recibidos:", response.json()["mensajes"])

def create_queue():
    queue_name = input("📦 Nombre de la cola: ")
    user = input("👤 Usuario creador: ")
    response = requests.post(f"{BASE_URL}/createQueue", json={"queue_name": queue_name, "user": user})
    print(f"📦 Crear cola: {response.status_code} - {response.text}")

def delete_queue():
    queue_name = input("🗑️ Cola a eliminar: ")
    user = input("👤 Usuario dueño: ")
    response = requests.post(f"{BASE_URL}/deleteQueue", json={"queue_name": queue_name, "user": user})
    print(f"🗑️ Eliminar cola: {response.status_code} - {response.text}")

def list_queues():
    response = requests.get(f"{BASE_URL}/queues")
    print(f"📋 Listado de colas: {response.status_code}")
    if response.ok:
        print("📦 Colas:", response.json()["queues"])

def send_to_queue():
    queue = input("📦 Nombre de la cola: ")
    mensaje = input("✉️ Mensaje: ")
    response = requests.post(f"{BASE_URL}/send/{queue}", json={"msm": mensaje})
    print(f"📤 Envío a cola: {response.status_code} - {response.text}")

def receive_from_queue():
    queue = input("📦 Nombre de la cola: ")
    response = requests.get(f"{BASE_URL}/receive/{queue}")
    print(f"📥 Recibir de cola: {response.status_code}")
    if response.ok:
        print("📨 Mensaje recibido:", response.json()["message"])

def menu():
    while True:
        print("\n=== CLIENTE MOM ===")
        print("1. Registrar usuario")
        print("2. Iniciar sesión")
        print("3. Crear tópico")
        print("4. Enviar a tópico")
        print("5. Recibir de tópico")
        print("6. Eliminar tópico")
        print("7. Listar tópicos")
        print("8. Crear cola")
        print("9. Enviar a cola")
        print("10. Recibir de cola")
        print("11. Eliminar cola")
        print("12. Listar colas")
        print("0. Salir")

        op = input("Selecciona una opción: ")

        if op == "1": register()
        elif op == "2": login()
        elif op == "3": create_topic()
        elif op == "4": send_to_topic()
        elif op == "5": receive_from_topic()
        elif op == "6": delete_topic()
        elif op == "7": list_topics()
        elif op == "8": create_queue()
        elif op == "9": send_to_queue()
        elif op == "10": receive_from_queue()
        elif op == "11": delete_queue()
        elif op == "12": list_queues()
        elif op == "0": break
        else: print("❌ Opción inválida")

if __name__ == "__main__":
    menu()
