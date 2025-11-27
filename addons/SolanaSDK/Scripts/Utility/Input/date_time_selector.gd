extends Node
class_name DateTimeSelector

@export var use_utc_time:bool=true
@export_category("Date Settings")
@export var use_date:bool=true
@export var year_option:OptionButton
@export var month_option:OptionButton
@export var day_option:OptionButton
@export var max_year_from_now:int=2

@export_category("Time Settings")
@export var use_time:bool=true
@export var hour_option:OptionButton
@export var minute_option:OptionButton
@export var minute_interval:int = 15

var date_options:Array[OptionButton]

signal on_updated(timestamp:int)
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if use_date:
		date_options.append(year_option)
		date_options.append(month_option)
		date_options.append(day_option)
	if use_time:
		date_options.append(hour_option)
		date_options.append(minute_option)

	for option in date_options:
		option.item_selected.connect(update_date)
		
	generate_date_option_fields()
	set_current_time()
	print(get_selected_timestamp())
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
func update_date(_selected_id:int) -> void:
	var selected_timestamp:int = get_selected_timestamp()
	on_updated.emit(selected_timestamp)
	
func generate_date_option_fields() -> void:
	for option in date_options:
		option.clear()
	
	#for year, we need to find current year and add max_years
	if use_date:
		var time = Time.get_datetime_dict_from_system(use_utc_time)
		for i in range(max_year_from_now):
			year_option.add_item(str(time["year"]+i),i)
			
		var months:Array = ["JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"]
		for i in range(months.size()):
			month_option.add_item(months[i],i)
		
		var year:int = int(year_option.get_item_text(year_option.get_selected_id()))
		var month:int = month_option.get_selected_id()+1
		var day_count:int = days_in_month(month,year)
		
		for i in range(day_count):
			day_option.add_item(str(i+1),i)
	
	if use_time:
		for i in range(24):
			hour_option.add_item(str(i),i)
		
		var minutes_count:int=0
		var min_index:int=0
		while minutes_count<60:
			minute_option.add_item(str(minutes_count),min_index)
			minutes_count+=minute_interval
			min_index+=1
	
func set_current_time() -> void:
	var time = Time.get_datetime_dict_from_system(use_utc_time)
	if use_date:
		year_option.select(0)
		month_option.select(time["month"]-1)
		day_option.select(time["day"]-1)
		
	if use_time:
		hour_option.select(time["hour"])
		
		for i in minute_option.item_count:
			if time["minute"] > int(minute_option.get_item_text(i)):
				continue
			else:
				minute_option.select(i)
				break
	
func get_selected_timestamp(in_utc:bool=true) -> int:
	if !use_date or !use_time:
		return 0
		
	var year_in_seconds:int = int(year_option.get_item_text(year_option.get_selected_id())) *365*24*60*60
	var datetime_dict:Dictionary = {
		"year":int(year_option.get_item_text(year_option.get_selected_id())),
		"month":month_option.get_selected_id()+1,
		"day":day_option.get_selected_id()+1,
		"hour":hour_option.get_selected_id(),
		"minute":int(minute_option.get_item_text(minute_option.get_selected_id())),
	}
	
	var timestamp:int = Time.get_unix_time_from_datetime_dict(datetime_dict)
	if in_utc:
		var offset_from_utc_in_minutes:int = Time.get_time_zone_from_system()["bias"]
		timestamp -= offset_from_utc_in_minutes*60
		
	return timestamp
	
	
func days_in_month(month: int, year: int) -> int:
	var days = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

	# Check if it's February and a leap year
	if month == 2 and is_leap_year(year):
		return 29
	return days[month - 1]

func is_leap_year(year: int) -> bool:
	# Leap year calculation
	if year % 4 == 0:
		if year % 100 == 0:
			return year % 400 == 0
		return true
	return false
	
func is_valid() -> bool:
	return true
