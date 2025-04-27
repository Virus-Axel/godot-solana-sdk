extends Node

func send_get_request(request_link:String,parse_body_to_json:bool=true,custom_headers:PackedStringArray=PackedStringArray()) -> Dictionary:
	var http_request = HTTPRequest.new()
	add_child(http_request)
	## Perform a GET request. The URL below returns JSON as of writing.
	var request = http_request.request(request_link,custom_headers)
	if request != OK:
		push_error("An error occurred in the HTTP request.")
		return {"error":"HTTP REQUEST FAILED"}
		
	var raw_response = await http_request.request_completed
	#print(raw_response)
	http_request.queue_free()
	var response_dict = parse_http_response(raw_response,parse_body_to_json)
	if !is_valid(response_dict):
		print("FAILED TO VALIDATE GET REQUEST: ", response_dict)
		var error_data:Dictionary = {"error":"INCORRECT REQUEST"}
		if response_dict["body"] != null and parse_body_to_json and response_dict["body"].has("error"):
			error_data = response_dict["body"]
		return error_data
	return response_dict
	
func send_post_request(body, headers:Array,endpoint:String,parse_body_to_json:bool=true) -> Dictionary:
	var http_request = HTTPRequest.new()
	add_child(http_request)
	var request = http_request.request(endpoint,headers,HTTPClient.METHOD_POST,body)
	if request != OK:
		push_error("An error occurred in the HTTP request.")
		return {"error":"HTTP REQUEST FAILED"}
		
	var raw_response = await http_request.request_completed
	#print(raw_response, raw_response[3].get_string_from_utf8())
	http_request.queue_free()
	var response_dict = parse_http_response(raw_response,parse_body_to_json)
	if !is_valid(response_dict):
		print("FAILED TO VALIDATE POST REQUEST: ", response_dict)
		var error_data:Dictionary = {"error":"INCORRECT REQUEST"}
		if response_dict["body"] != null and parse_body_to_json and response_dict["body"].has("error"):
			error_data = response_dict["body"]
		print(error_data)
		return error_data
	return response_dict
	
func parse_http_response(response:Array, body_to_json:bool=false) -> Dictionary:
	var body = response[3]
	if body_to_json:
		var json = JSON.new()
		json.parse(response[3].get_string_from_utf8())
		body = json.get_data()
	
	var dict = {
		"result":response[0],
		"response_code":response[1],
		"headers":response[2],
		"body":body
	}
	
	return dict
	
func is_valid(response:Dictionary) -> bool:
	return response["response_code"] == 200 and response["body"]!=null
