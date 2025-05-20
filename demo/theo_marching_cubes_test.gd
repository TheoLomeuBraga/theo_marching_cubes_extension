
extends TheoMarchingCubes

@export var materials : Array[Material]
func _ready() -> void:
	
	material_array = materials
	
	grid_size = Vector3i(100,100,100)
	
	
	
	for i in range(10,90):
		var begin : int = i + 0
		var end : int = i + 8
		for x in range(begin,end):
			for y in range(begin,end):
				for z in range(begin,end):
					set_volume_on_area(Vector3i(x,y,z),1.0)
	
	force_update()
