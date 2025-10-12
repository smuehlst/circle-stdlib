import socket
import select
import sys
import time

# --- Configuration ---
HOST_IP = '127.0.0.1'  # Listen on localhost on the host machine
HOST_PORT = 12345      # The port you will forward in QEMU, e.g., using -netdev user,hostfwd=tcp::12345-:12345
BUFFER_SIZE = 1024

def run_select_server():
    """
    A Python TCP server that uses select() to handle one client connection.
    This simulates a server for testing the client's select() function.
    """
    
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Allows the port to be reused immediately after the server is closed
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server_socket.bind((HOST_IP, HOST_PORT))
    except socket.error as msg:
        print(f"Bind failed. Error: {msg}")
        sys.exit()

    server_socket.listen(1)
    server_socket.setblocking(0) # Non-blocking for use with select
    
    print(f"Server listening on {HOST_IP}:{HOST_PORT}")

    # File descriptors for select:
    # 1. Server socket (for accepting new connections)
    # 2. Client socket (once connected, for reading/writing data)
    
    # The list of sockets to check for readable events
    inputs = [server_socket]
    # The list of sockets to check for writable events
    outputs = []
    # The list of connected clients
    client_connection = None
    
    # Data to send (initial message to test client's readability)
    send_data = b"Hello QEMU client! Ready for your test data.\n"
    
    # State for testing purposes
    response_sent = False
    
    print("Waiting for a client connection from QEMU...")

    try:
        while inputs:
            # The select call checks for activity. Timeout is 1 second.
            readable, writable, exceptional = select.select(inputs, outputs, inputs, 1)

            # If no sockets are ready, loop again (optional timeout handling)
            if not (readable or writable or exceptional):
                # print("Waiting...") # Uncomment for verbose output
                continue

            for s in readable:
                if s is server_socket:
                    # New connection attempt from client
                    connection, client_address = s.accept()
                    connection.setblocking(0) # Ensure client socket is also non-blocking
                    
                    print(f"Accepted connection from {client_address}")
                    client_connection = connection
                    inputs.append(connection)
                    # Once a connection is established, we want to be able to write to it
                    # if there is data to send.
                    if send_data:
                        outputs.append(connection)

                elif s is client_connection:
                    # Data from an established client connection
                    try:
                        data = s.recv(BUFFER_SIZE)
                        if data:
                            print(f"Received: {data.decode().strip()}")
                            # Optionally respond to the client's data
                            if not response_sent:
                                response_data = b"Server acknowledges your data.\n"
                                print("Preparing response to client...")
                                # Add socket to outputs to send response
                                if s not in outputs:
                                    outputs.append(s)
                                # Prepend the response to the send buffer if it was consumed
                                if not send_data:
                                    send_data = response_data
                                else:
                                    send_data = response_data + send_data
                                response_sent = True
                                
                        else:
                            # Interpreted as a closed connection
                            print("Client closed connection.")
                            # Clean up
                            if s in outputs:
                                outputs.remove(s)
                            inputs.remove(s)
                            s.close()
                            client_connection = None
                            return # Exit after client disconnects

                    except socket.error as e:
                        # Handle errors like connection reset
                        print(f"Socket error on read: {e}. Closing connection.")
                        if s in outputs:
                            outputs.remove(s)
                        inputs.remove(s)
                        s.close()
                        client_connection = None
                        return # Exit on error

            for s in writable:
                # Socket is ready for writing
                if s is client_connection and send_data:
                    try:
                        # Send the data
                        sent = s.send(send_data)
                        print(f"Sent {sent} bytes of data to client.")
                        # Remove the sent data from the buffer
                        send_data = send_data[sent:]
                        
                        # Stop checking for writability if the buffer is empty
                        if not send_data:
                            print("Finished sending data.")
                            outputs.remove(s)
                            
                    except socket.error as e:
                        print(f"Socket error on write: {e}. Closing connection.")
                        # Clean up
                        if s in outputs:
                            outputs.remove(s)
                        inputs.remove(s)
                        s.close()
                        client_connection = None
                        return # Exit on error

            for s in exceptional:
                # Handle exceptional conditions (e.g., connection error)
                print(f"Handling exceptional condition for {s.getpeername()}")
                if s in inputs:
                    inputs.remove(s)
                if s in outputs:
                    outputs.remove(s)
                s.close()
                client_connection = None
                return # Exit on exceptional condition
                
    except KeyboardInterrupt:
        print("\nServer shutting down.")
    finally:
        if client_connection:
            client_connection.close()
        server_socket.close()

if __name__ == '__main__':
    run_select_server()