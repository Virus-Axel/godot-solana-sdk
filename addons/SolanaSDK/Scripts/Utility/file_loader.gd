extends Node
class_name FileLoader

@export_file("*.json") var path_to_metadata_whitelist:String

var image_cache:Dictionary
var metadata_cache:Dictionary

var cors_proxy_link:String = "https://proxy.cors.sh/"

var whitelist:Array

func _ready() -> void:
	var json_as_text = FileAccess.get_file_as_string(path_to_metadata_whitelist)
	var json_as_array:Array = JSON.parse_string(json_as_text)
	if json_as_array:
		whitelist = json_as_array

func is_whitelisted(url:String) -> bool:
	for whitelisted_link in whitelist:
		if url.contains(whitelisted_link):
			return true
	return false
	
func load_token_metadata(uri:String,ignore_whitelist:bool=false) -> Dictionary:
	if metadata_cache.has(uri):
		return metadata_cache[uri]
		
	if !is_whitelisted(uri) and !ignore_whitelist:
		return {}
		
	var response:Dictionary
	var request_link:String = try_reroute(uri)
	print(request_link)
	while true:
		response = await HttpRequestHandler.send_get_request(request_link)
		if !response.has("error"):
			metadata_cache[uri] = response["body"]
			break
		else:
			#if fails, try one more time using cors proxy link	
			if request_link == uri:
				request_link = cors_proxy_link+uri
				continue
			else:
				push_error("Failed to fetch Token Metadata for %s" % uri)
				return {}
				
	return response["body"]
	

func load_token_image(image_link:String,size:int=512,ignore_whitelist:bool=false) -> Texture2D:
	if image_cache.has(image_link):
		var image:Image = image_cache[image_link].get_image()
#		still reload the cached image if requesting bigger size
		if size <= image.get_size().x or size <= image.get_size().y:
			return image_cache[image_link]
		else:
			print("Cached image is smaller than requested one. Refetching....")
		
	if !is_whitelisted(image_link) and !ignore_whitelist:
		return null
		
	var response:Dictionary
	var request_link:String = try_reroute(image_link)
	while true:
		response = await HttpRequestHandler.send_get_request(request_link,false)
		if !response.has("error"):
			break
		else:
			#if fails, try one more time using cors proxy link	
			if request_link == image_link:
				request_link = cors_proxy_link+image_link
				continue
			else:
				push_error("Failed to fetch Token Image")
				break
				
	if response.has("error"):
		return null
				
	var content_type:String = parse_image_type(response["headers"])
	if content_type=="":
		push_error("Failed to figure out Image Type")
		return null
	
	var texture:Texture2D
	if content_type == "gif":
		return null
#		texture = parse_gif_data_to_texture(response_dict["body"],size)
	else:
		texture = parse_image_data_to_texture(response["body"],size)
		
	if texture==null:
		return null
	
	image_cache[image_link] = texture
	return texture
	
	
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
		if is_webp_animated(image_raw_data):
			return null
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
	var marker:PackedByteArray = PackedByteArray([255, 216])
	return raw_image_data.slice(0,2) == marker
	
func has_png_signature(raw_image_data:PackedByteArray)->bool:
	var marker:PackedByteArray = PackedByteArray([137, 80, 78, 71, 13, 10, 26, 10])
	return raw_image_data.slice(0,8) == marker
	
func has_webp_signature(raw_image_data:PackedByteArray)->bool:
	var riff_marker:PackedByteArray =  PackedByteArray([82, 73, 70, 70])
	var webp_marker:PackedByteArray = PackedByteArray([87, 69, 66, 80])
	if raw_image_data.slice(0,4) != riff_marker:
		return false
	if raw_image_data.slice(8,12) != webp_marker:
		return false
	return true
	
func is_webp_animated(raw_image_data:PackedByteArray)->bool:
	if raw_image_data.size() < 18:  # Minimum size for VP8X check
		return false
	var vp8x_marker:PackedByteArray = PackedByteArray([86, 80, 56, 88])
	# VP8X marker and ANIM flag (bit 1 in 4th byte of the VP8X chunk)
	if raw_image_data.slice(12,16) == vp8x_marker:
		# Byte 16 contains feature flags; check if ANIM bit is set
		return (raw_image_data[16] & 0b00010000) != 0  # Check if 'A' bit (bit 4) is set
	
	return false
	
func load_3d_model(model_link:String) -> GLTFState:
	var response:Dictionary = await HttpRequestHandler.send_get_request(model_link)
	if response.has("error"):
		return null
	
	var state:GLTFState = GLTFState.new()
	var gltf_document:GLTFDocument = GLTFDocument.new()
	var glb_load_request
	var flags = 8	
	#flags |= EditorSceneFormatImporter.IMPORT_USE_NAMED_SKIN_BINDS
	glb_load_request = gltf_document.append_from_buffer(response["body"], "", state, flags)
	
	if glb_load_request != OK:
		return null
	
	return state
	
func try_reroute(uri:String) -> String:
	if uri.find("ipfs.io") != -1:
		return uri.replace("ipfs.io", "dweb.link")
	return uri
