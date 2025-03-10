extends CheckBox
class_name CheckboxLock

@export var lock_active:bool=true

@export var token_address:String
@export var unlock_amount:float

@export var available_label:Label


func _ready() -> void:
	self.visibility_changed.connect(on_visibility_changed)

func on_visibility_changed() -> void:
	if self.visible and lock_active:
		set_interactable(false)
		try_unlock()
		
func set_interactable(state:bool) -> void:
	disabled = !state
		
func try_unlock() -> void:
	if !SolanaService.wallet.is_logged_in():
		return
		
	var account_address = SolanaService.wallet.get_pubkey()
	var token_balance:float
	if token_address.length()==0:
		token_balance = await SolanaService.get_balance(account_address.to_string())
	else:
		token_balance = await SolanaService.get_balance(account_address.to_string(),token_address)
	set_interactable(token_balance>=unlock_amount)
	available_label.text = "%s Available" % int(token_balance)
	
func deselect() -> void:
	button_pressed=false
