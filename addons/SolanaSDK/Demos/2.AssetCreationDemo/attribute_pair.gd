extends Node
class_name AttributePairExample

@export var key_field:InputField
@export var value_field:InputField

func get_attribute() -> Attribute:
	var attribute:Attribute = Attribute.new()
	attribute.key = key_field.get_field_value()
	attribute.value = value_field.get_field_value()
	
	if attribute.key=="" or attribute.value=="":
		return null
	
	return attribute
	
func set_attribute(key:String,value:String) -> void:
	key_field.set_value(key)
	value_field.set_value(value)
	
func reset() -> void:
	key_field.clear()
	value_field.clear()
