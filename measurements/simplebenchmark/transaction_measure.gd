extends Control

var round_one_frames = 0
var round_two_frames = 0
var measurment_counter = 0

const WORKLOAD_FACTOR = 50

const filename: String = "output.txt"

func create_transaction():
	var tx = Transaction.new()
	var payer = Keypair.new_random()
	var receiver = Pubkey.new_random()
	var ix = SystemProgram.transfer(payer, receiver, 1000)
	tx.set_payer(payer)
	tx.add_instruction(ix)
	tx.update_latest_blockhash("11111111111111111111111111111111")
	tx.sign()
	tx.serialize()

func write_results():
	var benchmark_entry := {}
	benchmark_entry["name"] = "Simple Transaction Performance Index"
	benchmark_entry["unit"] = "Percent"
	benchmark_entry["value"] = float(round_two_frames) / float(round_one_frames)
	var benchmark_data := [benchmark_entry]
	
	var file = FileAccess.open(filename, FileAccess.WRITE)
	file.store_string(JSON.stringify(benchmark_data))
	file.close()

func _process(delta: float) -> void:
	if measurment_counter == 0:
		round_one_frames += 1
		return
	elif measurment_counter == 1:
		for i in range(WORKLOAD_FACTOR):
			create_transaction()
		round_two_frames += 1
	else:
		write_results()
		queue_free()
		get_tree().quit(0)

func _on_round_timer_timeout() -> void:
	measurment_counter += 1
