
extends TheoMarchingCubes

func _ready() -> void:
	
	grid_size = Vector3i(100,100,100)
	use_collision = true
	
	for i in range(10,90):
		
		var begin : int = i + 0
		var end : int = i + 8
		
		for x in range(begin,end):
			for y in range(begin,end):
				for z in range(begin,end):
					if x % 3 == 0:
						set_volume_type_on_area(Vector3i(x,y,z),-1)
					else:
						set_volume_type_on_area(Vector3i(x,y,z),0)
						
		for x in range(5,95):
			for y in range(5,10):
				for z in range(5,95):
					set_volume_type_on_area(Vector3i(x,y,z),0)

 
