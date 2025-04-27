extends Node
class_name ChainPatrolAPI

var api_link:String = "https://app.chainpatrol.io/api/v2/"

func is_url_safe(url:String) -> bool:
	var headers:Array = ["Content-type: application/json"]
	var body:Dictionary = {
		"content":url
	}
	var response:Dictionary = await HttpRequestHandler.send_post_request(JSON.stringify(body),headers,api_link+"asset/details")
	if response.size() == 0 or response["body"].has("error"):
		push_error("failed to get response for whether %s is safe or not!" % url)
		return false
	
	print(response)
	return true
