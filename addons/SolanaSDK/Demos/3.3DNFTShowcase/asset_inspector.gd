extends Node3D

@export var cam := Node3D

@export var allowRotation := true
@export var rotateSpeed := 3.0

@export var allowMove := true
@export var moveSpeed := 5.0

@export var allowZoom := true
@export var zoomSpeed := 5.0
@export var minDistance := 12.0
@export var maxDistance := 25.0

@export var rotateOnDefault := true
@export var defaultRotationSpeed := 10.0

var rotateInputActive := false
var moveInputActive := false

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	#only activate
	if get_child_count() == 0:
		pass
	
	if !rotateInputActive && !moveInputActive && rotateOnDefault:
		rotate(Vector3(0,1,0),defaultRotationSpeed*delta)
		
var prevMousePos := Vector2.ZERO

func _input(event: InputEvent) -> void:	
	if rotateInputActive:
		if event is InputEventMouseMotion:
			var currentMousePosition = event.global_position
			if prevMousePos == Vector2.ZERO:
				prevMousePos = currentMousePosition
				return
				
			var mouseDelta = Vector2()  # Initialize to zero
			
			if currentMousePosition != prevMousePos:
				mouseDelta = currentMousePosition - prevMousePos
				
			#print(mouseDelta)
			rotate(Vector3(0,1,0),rotateSpeed*0.001*mouseDelta.x)
			prevMousePos = currentMousePosition		
		
func _unhandled_input(event: InputEvent) -> void:
	if allowZoom:
		var currZoom = 0
		if event is InputEventMouseButton:
			if event.button_index == MOUSE_BUTTON_LEFT:
				rotateInputActive = event.pressed
				if rotateInputActive:
					prevMousePos = Vector2.ZERO
			if event.button_index == MOUSE_BUTTON_RIGHT:
				moveInputActive = event.pressed
			if event.button_index == MOUSE_BUTTON_WHEEL_UP:
				currZoom -= 1
			if event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
				currZoom += 1
				
		if currZoom != 0:
			var distanceToAsset = cam.transform.origin.distance_to(transform.origin)
			print(distanceToAsset)
			if currZoom<0 && distanceToAsset <= minDistance:
				return
			if currZoom>0 && distanceToAsset >= maxDistance:
				return
				
			var dirToAsset = (cam.transform.origin - transform.origin).normalized()
			cam.transform.origin += dirToAsset * currZoom * zoomSpeed
			
			
		
		
	
