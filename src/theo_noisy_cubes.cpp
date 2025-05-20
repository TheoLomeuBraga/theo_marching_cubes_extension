#include "theo_noisy_cubes.h"
#include "MarchingCubeCpp/noise.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void TheoNoisyCubes::_bind_methods()
{

	ClassDB::bind_method(D_METHOD("get_noise_scale"), &TheoNoisyCubes::get_noise_scale);
	ClassDB::bind_method(D_METHOD("set_noise_scale", "p_scale"), &TheoNoisyCubes::set_noise_scale);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "noise_scale",PROPERTY_HINT_RANGE, "0.01,10.0,0.01,or_greater,or_less"), "set_noise_scale", "get_noise_scale");

	
	ClassDB::bind_method(D_METHOD("get_noise_thicknes"), &TheoNoisyCubes::get_noise_thicknes);
	ClassDB::bind_method(D_METHOD("set_noise_thicknes", "p_thicknes"), &TheoNoisyCubes::set_noise_thicknes);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "noise_thicknes",PROPERTY_HINT_RANGE, "-1.0,1.0,0.01,or_greater,or_less"), "set_noise_thicknes", "get_noise_thicknes");

}

TheoNoisyCubes::TheoNoisyCubes()
{

	noise_scale = 1.0f;
	noise_thicknes = 0.0f;
	has_changed = true;
}

TheoNoisyCubes::~TheoNoisyCubes()
{
	// Add your cleanup here.
}

void TheoNoisyCubes::set_noise_scale(const float p_scale){
	noise_scale = p_scale;
	has_changed = true;
}

float TheoNoisyCubes::get_noise_scale() const {
	return noise_scale;
}

void TheoNoisyCubes::set_noise_thicknes(const float p_thicknes){
	noise_scale = p_thicknes;
	has_changed = true;
}

float TheoNoisyCubes::get_noise_thicknes() const {
	return noise_thicknes;
}

void TheoNoisyCubes::generate_noise(){

	
	
	Vector3 grid_size = get_grid_size();

	for (int x = 0; x < grid_size.x; x++) {
        for (int y = 0; y < grid_size.y; y++) {
            for (int z = 0; z < grid_size.z; z++) {
                int32_t index = (z * grid_size.y * grid_size.x) + (y * grid_size.x) + x;
				set_volume_on_area(grid_size, (PerlinNoise::GetValue(x, y, z) * noise_scale) + noise_thicknes);
            }
        }
    }
	

}

void TheoNoisyCubes::process_TheoNoisyCubes(){
	generate_noise();
}


void TheoNoisyCubes::_process(double delta) 
{
	
	if (has_changed){
		process_TheoNoisyCubes();
	}
	
	TheoMarchingCubes::_process(delta);
	
}
