extends Node

@export var listen_port = 8181
const TARGET_PORT := 8899
const TARGET_HOST := "localhost"

@onready var server = TCPServer.new()

var clients: Array[StreamPeerTCP] = []

func _ready() -> void:
	var err = server.listen(listen_port)
	if err != OK:
		print("Failed to listen")
		get_tree().quit(1)

func _process(delta: float) -> void:
	
	if server.is_connection_available():
		print("CONNNN")
		var client = server.take_connection()
		client.set_no_delay(true)
		clients.append(client)

	# Handle active clients
	for client in clients.duplicate():
		if client.get_status() != StreamPeerTCP.STATUS_CONNECTED:
			clients.erase(client)
			continue

		if client.get_available_bytes() > 0:
			handle_client_request(client)
			clients.erase(client)
			

func handle_client_request(client: StreamPeerTCP):
	# Read full HTTP request
	var request_bytes := PackedByteArray()
	while client.get_available_bytes() > 0:
		request_bytes.append_array(client.get_data(client.get_available_bytes())[1])

	# Forward request to target server
	var upstream := StreamPeerTCP.new()
	if upstream.connect_to_host(TARGET_HOST, TARGET_PORT) != OK:
		client.disconnect_from_host()
		return

	while upstream.get_status() == StreamPeerTCP.STATUS_CONNECTING:
		await get_tree().process_frame

	upstream.put_data(request_bytes)

	# Read response
	var response_bytes := PackedByteArray()
	while true:
		await get_tree().process_frame
		var available = upstream.get_available_bytes()
		if available > 0:
			response_bytes.append_array(upstream.get_data(available)[1])
		elif upstream.get_status() != StreamPeerTCP.STATUS_CONNECTED:
			break

	# Send response back to client
	client.put_data(response_bytes)

	client.disconnect_from_host()
	upstream.disconnect_from_host()
