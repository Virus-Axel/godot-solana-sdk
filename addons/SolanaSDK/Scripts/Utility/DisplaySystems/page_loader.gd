extends Node
class_name PageLoader

@export var page_label:Label
@export var prev_page_button:BaseButton
@export var next_page_button:BaseButton

var curr_page:int = 0
var total_pages:int = 0

signal on_page_changed(new_page:int)

func _ready() -> void:
	freeze_input(true)
	prev_page_button.pressed.connect(set_prev_page)
	next_page_button.pressed.connect(set_next_page)
	
	on_page_changed.connect(update_label)

func setup(start_page:int,page_count:int) -> void:
	curr_page = start_page
	total_pages = page_count
	update_label()
	update_button_states()
	freeze_input(false)

func set_prev_page() -> void:
	if curr_page == 0:
		return
		
	curr_page -= 1
	on_page_changed.emit(curr_page)
	update_button_states()
	pass
	
func set_next_page() -> void:
	if curr_page == total_pages-1:
		return
		
	curr_page += 1
	on_page_changed.emit(curr_page)
	update_button_states()
	pass
	
func update_button_states() -> void:
	prev_page_button.disabled = (curr_page == 0)
	next_page_button.disabled = (curr_page >= total_pages-1)
	
func update_label(_page:int=0) -> void:
	page_label.text = "%s/%s" % [curr_page+1,total_pages]
	
func freeze_input(freeze:bool) -> void:
	prev_page_button.visible = !freeze
	next_page_button.visible = !freeze
	
	
