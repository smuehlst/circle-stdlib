import socket
import threading
import sys

START_PORT = 5000
NUM_PORTS = 3   # listens on 5000, 5001, 5002

def handle_client(conn, addr, port):
    print(f"[+] Connection on port {port} from {addr}")
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                break
            print(f"[{port}] Received: {data!r}")
            reply = f"ACK from port {port}: got {len(data)} bytes\n".encode()
            conn.sendall(reply)
    except Exception as e:
        print(f"[{port}] Error: {e}")
    finally:
        conn.close()

def listen_on_port(port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("0.0.0.0", port))
    s.listen(1)
    print(f"Listening on port {port}")
    while True:
        conn, addr = s.accept()
        threading.Thread(target=handle_client, args=(conn, addr, port), daemon=True).start()

if __name__ == "__main__":
    threads = []
    for i in range(NUM_PORTS):
        port = START_PORT + i
        t = threading.Thread(target=listen_on_port, args=(port,), daemon=True)
        t.start()
        threads.append(t)
    print("Servers running. Press Ctrl+C to stop.")
    try:
        while True:
            pass
    except KeyboardInterrupt:
        sys.exit(0)
