## Theo Marching Cubes

This is a simple, public domain extension so you can play with marching cubes quickly and easily.

![alt text](https://github.com/TheoLomeuBraga/theo_marching_cubes_extension/blob/master/MC_demo.png "marching cubes demo demo")

## EXAMPLE:

```gdscript

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
						set_volume_type_on_area(Vector3i(x,y,z),1)
					else:
						set_volume_type_on_area(Vector3i(x,y,z),-1)

```

## TODO: 
* Add threading support
* Add nav mesh suport
* Add documentation

## CREDITS:
https://github.com/aparis69/MarchingCubeCpp

## Licence
Double Licence, whatever fits you best: Public domain or MIT License.
