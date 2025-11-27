extends Node
class_name PageLoader

@export var pages:Array[Node]
@export var prev_page_button:BaseButton
@export var next_page_button:BaseButton
@export var page_label:Label

var curr_page:int = 0
var total_pages:int = 0

signal on_page_changed(new_page:int)

func _ready() -> void:
	freeze_input(true)
	if pages.size()>0:
		setup(0,pages.size())
		
	prev_page_button.pressed.connect(set_prev_page)
	next_page_button.pressed.connect(set_next_page)
	
	if page_label!=null:
		on_page_changed.connect(update_label)

func setup(start_page:int,page_count:int) -> void:
	for page in pages:
		page.visible=false
		
	curr_page = start_page
	total_pages = page_count
	set_page(start_page)
	freeze_input(false)

func set_prev_page() -> void:
	if curr_page == 0:
		return	
	set_page(curr_page-1)
	
func set_next_page() -> void:
	if curr_page == total_pages-1:
		return	
	set_page(curr_page+1)
	
func set_page(page_id:int) -> void:
	if pages.size()>0:
		pages[curr_page].visible=false
		pages[page_id].visible=true
		
	curr_page = page_id
	prev_page_button.disabled = (curr_page == 0)
	
	if curr_page >= total_pages-1:
		next_page_button.disabled=true
	else:
		next_page_button.disabled =false
	
	if page_label!=null:
		update_label()
	
	on_page_changed.emit(curr_page)

	
func update_label(_page:int=0) -> void:
	page_label.text = "%s/%s" % [curr_page+1,total_pages]
	
func freeze_input(freeze:bool) -> void:
	prev_page_button.visible = !freeze
	next_page_button.visible = !freeze
	
	
