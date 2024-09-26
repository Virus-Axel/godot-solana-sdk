extends Node
class_name FileLoader
	
func load_token_metadata(uri:String) -> Dictionary:
	var http_request = HTTPRequest.new()
	add_child(http_request)
	# Perform a GET request. The URL below returns JSON as of writing.
	var request = http_request.request(uri)
	if request != OK:
		push_error("An error occurred in the HTTP request.")
		return {}
		
	var raw_response = await http_request.request_completed
	http_request.queue_free()
	var response_dict = parse_http_response(raw_response,true)
	
	if response_dict["response_code"] != 200:
		push_error("Failed to fetch Token Metadata")
		return {}
	
	if response_dict["body"] == null:
		return {}
		
	return response_dict["body"]
	

func load_token_image(image_link:String,size:int=512) -> Texture2D:
	var http_request = HTTPRequest.new()
	add_child(http_request)
	
	var request = http_request.request_raw(image_link)
	if request != OK:
		push_error("An error occurred in the HTTP request.")
		return null
	
	var raw_response = await http_request.request_completed
	http_request.queue_free()
	
	var response_dict = parse_http_response(raw_response)
	
	if response_dict["response_code"] != 200:
		push_error("Failed to fetch Token Image")
		return null
	
	var content_type:String = parse_image_type(response_dict["headers"])
	if content_type=="":
		push_error("Failed to figure out Image Type")
		return null
	
	var texture:Texture2D
	if content_type == "gif":
		return null
#		texture = parse_gif_data_to_texture(response_dict["body"],size)
	else:
		texture = parse_image_data_to_texture(response_dict["body"],size)
		
	if texture==null:
		return null
	
	return texture
	
	
	
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
	
func parse_image_type(headers:Array[String]) -> String:
	for header in headers:
		if header.to_lower().contains("content-type"):
			return header.split("/")[1]
	return ""
	
func parse_image_data_to_texture(image_raw_data:PackedByteArray, image_size:int) -> Texture2D:
	var image = Image.new()
	var img_load_request
	
	if has_jpg_signature(image_raw_data):
		img_load_request = image.load_jpg_from_buffer(image_raw_data)
		
	elif has_png_signature(image_raw_data):
		img_load_request = image.load_png_from_buffer(image_raw_data)
		
	elif has_webp_signature(image_raw_data):
		img_load_request = image.load_webp_from_buffer(image_raw_data)
		
	else:
		img_load_request = image.load_png_from_buffer(image_raw_data)
		if img_load_request != OK:
			img_load_request = image.load_jpg_from_buffer(image_raw_data)
			if img_load_request!= OK:
				img_load_request = image.load_webp_from_buffer(image_raw_data)
			
	if img_load_request != OK:
		return null
		
	image.resize(image_size,image_size)	
	return ImageTexture.create_from_image(image)
	
#func parse_gif_data_to_texture(image_raw_data:PackedByteArray, image_size:int) -> Texture2D:
#	var gif_frames:SpriteFrames = GifManager.sprite_frames_from_buffer(image_raw_data,1)
#	var texture:Texture2D = gif_frames.get_frame_texture(gif_frames.get_animation_names()[0],0)
#
#	return texture
	
func has_jpg_signature(raw_image_data:PackedByteArray)->bool:
	var marker = [255, 216]
	for i in range(2):
		if raw_image_data[i] != marker[i]:
			return false
	return true
	
func has_png_signature(raw_image_data:PackedByteArray)->bool:
	var marker = [137, 80, 78, 71, 13, 10, 26, 10]
	for i in range(8):
		if raw_image_data[i] != marker[i]:
			return false
	return true
	
func has_webp_signature(raw_image_data:PackedByteArray)->bool:
	var marker = [82, 73, 70, 70, 0, 0, 0, 0, 87, 69, 66, 80]
	for i in range(12):
		if raw_image_data[i] != marker[i]:
			return false
	return true
	
func load_3d_model(model_link:String) -> GLTFState:
	var http_request = HTTPRequest.new()
	add_child(http_request)
	
	var request = http_request.request_raw(model_link)
	if request != OK:
		push_error("An error occurred in the HTTP request.")
		return null
	
	var raw_response = await http_request.request_completed
	http_request.queue_free()
	
	var response_dict = parse_http_response(raw_response)
	
	if response_dict["response_code"] != 200:
		push_error("Failed to fetch Token Model")
		return null
	
	var state:GLTFState = GLTFState.new()
	var gltf_document:GLTFDocument = GLTFDocument.new()
	var glb_load_request
	var flags = 8	
	#flags |= EditorSceneFormatImporter.IMPORT_USE_NAMED_SKIN_BINDS
	glb_load_request = gltf_document.append_from_buffer(response_dict["body"], "", state, flags)
	
	if glb_load_request != OK:
		return null
	
	return state
