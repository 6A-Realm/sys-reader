import logging
from websocket_server import WebsocketServer
# https://github.com/Pithikos/python-websocket-server

def new_client(client, server):
    print("New client connected and was given id %d" % client['id'])
    server.send_message_to_all("Hey all, a new client has joined us")

def new_message(client, server, message):
    print(f"Client {client['id']} said: {message}")
    server.send_message_to_all(f"Client {client['id']} said: {message}")

def client_left(client, server):
    print(f"Client {client['id']} left")

server = WebsocketServer(host='0.0.0.0', port=9876, loglevel=logging.DEBUG)
server.set_fn_new_client(new_client)
server.set_fn_message_received(new_message)
server.set_fn_client_left(client_left)
server.run_forever()