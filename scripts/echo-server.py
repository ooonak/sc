import socket
import os

# Path for the Unix domain socket
socket_path = '/tmp/sc_test.sock'

# Remove the socket file if it exists from a previous run
if os.path.exists(socket_path):
    os.remove(socket_path)

# Create a Unix domain socket
server_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# Bind the socket to the path
server_socket.bind(socket_path)

# Listen for incoming connections (maximum of 1 connection in the backlog)
server_socket.listen(1)
print(f"Listening for connections on {socket_path}...")

while True:
    # Accept a connection
    client_socket, _ = server_socket.accept()
    print("Client connected")

    # Receive data from the client and echo it back
    while True:
        data = client_socket.recv(1024)
        if not data:
            break  # If no data, client disconnected
        print(f"Received: {data.decode()}")
        client_socket.sendall(data)  # Echo back the data

    # Close the client connection
    client_socket.close()
    print("Client disconnected")

# Close the server socket (this part won't be reached unless you stop the server)
server_socket.close()

