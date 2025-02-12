extends Node3D
class_name ModelNFT

@export var loading_template:Node3D
@export var target_size:float=5.0

var nft:Nft
var model:Node3D
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	loading_template.visible=false
	pass # Replace with function body.

func setup(nft:Nft, auto_load_model:bool=false) -> void:
	self.nft = nft
	if auto_load_model:
		try_load_model()

func try_load_model() -> void:
	if nft==null:
		push_error("Couldnt find the NFT to load model of")
		return
	if nft.model_state!=null:
		instantiate_model(nft.model_state)
		return
		
	loading_template.visible=true
	
	await nft.try_load_model()
	if nft.model_state!=null:
		instantiate_model(nft.model_state)
	else:
		print("Couldn't load the model for mint: %s" % nft.mint.to_string())
		
	loading_template.visible=false
			
			
func instantiate_model(state:GLTFState) -> void:
	var gltf_document:GLTFDocument = GLTFDocument.new()
	#print(state.json)
	model = gltf_document.generate_scene(nft.model_state,30,true,true)
	var error = convert_to_mesh_instance(model)
	if error == OK:
		add_child(model)
		scale_model_to_target()
		self.name = nft.metadata.get_token_name()
	else:
		model.queue_free()
	
func convert_to_mesh_instance(p_root: Node):
	if p_root == null:
		return ERR_INVALID_PARAMETER
	var queue: Array = [p_root]
	var delete_queue: Array = []
	while not queue.is_empty():
		var node: Node = queue.front()
		queue.pop_front()

		var mesh_3d: ImporterMeshInstance3D = node as ImporterMeshInstance3D
		if mesh_3d:
			var mesh_instance_node_3d := MeshInstance3D.new()
			var mesh: ImporterMesh = mesh_3d.get_mesh()
			if mesh:
				var array_mesh: ArrayMesh = mesh.get_mesh()
				mesh_instance_node_3d.name = node.name
				mesh_instance_node_3d.transform = mesh_3d.transform
				mesh_instance_node_3d.mesh = array_mesh
				mesh_instance_node_3d.skin = mesh_3d.get_skin()
				mesh_instance_node_3d.skeleton = mesh_3d.get_skeleton_path()
				node.replace_by(mesh_instance_node_3d)
				delete_queue.append(node)
				node = mesh_instance_node_3d
			else:
				mesh_instance_node_3d.queue_free()

		for i in range(node.get_child_count()):
			queue.append(node.get_child(i))

	while not delete_queue.is_empty():
		var node_to_delete: Node = delete_queue.front()
		delete_queue.pop_front()
		node_to_delete.queue_free()

	return OK
	
func scale_model_to_target() -> void:
	var meshes:Array[MeshInstance3D] = get_all_mesh_instances(model)
	var combined_size = get_combined_aabb(meshes)
	var max_dimension = max(combined_size.size.x, combined_size.size.y, combined_size.size.z)
	var scale_factor = target_size / max_dimension
	model.scale *= scale_factor

func get_all_mesh_instances(parent_node: Node3D) -> Array[MeshInstance3D]:
	var mesh_instances:Array[MeshInstance3D]

	# Iterate through all child nodes
	for child in parent_node.get_children():
		# Check if the child is a MeshInstance3D
		if child is MeshInstance3D:
			mesh_instances.append(child)

		# Recursively call the function for child nodes
		mesh_instances += get_all_mesh_instances(child)

	return mesh_instances
	
func get_combined_aabb(meshes:Array[MeshInstance3D]) -> AABB:
	# Initialize an empty AABB
	var combined_aabb = AABB()
	# Iterate through all child nodes
	for mesh in meshes:
		# Get the AABB of the current mesh in global space
		var mesh_aabb = mesh.global_transform * mesh.get_aabb()
		# Expand the combined AABB to include the current mesh's AABB
		combined_aabb = combined_aabb.merge(mesh_aabb)
	return combined_aabb
