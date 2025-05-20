#include "theo_marching_cubes.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

#define MC_IMPLEM_ENABLE
#include "MarchingCubeCpp/MC.h"

#include <godot_cpp/core/print_string.hpp>

using namespace godot;

void TheoMarchingCubes::clean_grid_data(const Vector3i &size, MC::MC_FLOAT* data){
	for(int i = 0 ; i < size.x * size.y * size.z ; i++){
		data[i] = -1.0f;
	}
}

void TheoMarchingCubes::clean_grid(){
	clean_grid_data(grid_size,_field_data);
}

void TheoMarchingCubes::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_grid_size"), &TheoMarchingCubes::get_grid_size);
	ClassDB::bind_method(D_METHOD("set_grid_size", "size"), &TheoMarchingCubes::set_grid_size);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "grid_size"), "set_grid_size", "get_grid_size");

	ClassDB::bind_method(D_METHOD("set_volume_on_area", "p_position", "p_volume"), &TheoMarchingCubes::set_volume_on_area);
	ClassDB::bind_method(D_METHOD("get_volume_on_area", "p_position"), &TheoMarchingCubes::get_volume_on_area);

	
	ClassDB::bind_method(D_METHOD("force_update"), &TheoMarchingCubes::process_TheoMarchingCubes);
	ClassDB::bind_method(D_METHOD("clean_grid"), &TheoMarchingCubes::clean_grid);

	ClassDB::bind_method(D_METHOD("get_material_array"), &TheoMarchingCubes::get_material_array);
	ClassDB::bind_method(D_METHOD("set_material_array", "p_material"), &TheoMarchingCubes::set_material_array);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "material_array", PROPERTY_HINT_ARRAY_TYPE, "Material"), "set_material_array", "get_material_array");

}

TheoMarchingCubes::TheoMarchingCubes()
{
	// Initialize any variables here.
	grid_size = Vector3(32, 32, 32);
	_field_data = new MC::MC_FLOAT[int(grid_size.x * grid_size.y * grid_size.z)];

	clean_grid();
	
}

TheoMarchingCubes::~TheoMarchingCubes()
{
	// Add your cleanup here.
	delete _field_data;
}

void TheoMarchingCubes::set_grid_size(const Vector3i &p_size)
{
	

	MC::MC_FLOAT *new_field_data = new MC::MC_FLOAT[int(p_size.x * p_size.y * p_size.z)];
	clean_grid_data(p_size,new_field_data);
	

	// TODO: transference

	delete _field_data;
	_field_data = new_field_data;

	grid_size = p_size;
	has_changed = true;
	
}

Vector3i TheoMarchingCubes::get_grid_size() const
{
	return grid_size;
}

bool TheoMarchingCubes::is_cord_valid(const Vector3i &p_position) const
{
	return (p_position.x >= 0 && p_position.x < grid_size.x) && (p_position.y >= 0 && p_position.y < grid_size.y) && (p_position.z >= 0 && p_position.z < grid_size.z);
}

void TheoMarchingCubes::set_volume_on_area(const Vector3i &p_position, float p_volume)
{
	
	if (_field_data != NULL && is_cord_valid(p_position))
	{
		int32_t index = (p_position.z * grid_size.y * grid_size.x) + (p_position.y * grid_size.x) + p_position.x;
		_field_data[index] = p_volume -1;
		has_changed = true;
	}else{
		print_error("invalid volume area");
	}
}

float TheoMarchingCubes::get_volume_on_area(const Vector3i &p_position) const
{
	
	if (_field_data != NULL && is_cord_valid(p_position))
	{
		int32_t index = (p_position.z * grid_size.y * grid_size.x) + (p_position.y * grid_size.x) + p_position.x;
		return _field_data[index] + 1;
	}
	else
	{
		print_error("invalid volume area");
		return 0.0f;
	}
	
}

void TheoMarchingCubes::set_volume_type_on_area(const Vector3i &p_position, int32_t p_volume_type)
{

	has_changed = true;
}

int32_t TheoMarchingCubes::get_volume_type_on_area(const Vector3i &p_position) const
{
	return 0;
}

void TheoMarchingCubes::process_TheoMarchingCubes()
{

	MC::mcMesh marching_cubes_mesh;
	MC::marching_cube(_field_data, grid_size.x, grid_size.y, grid_size.z, marching_cubes_mesh);

	if (_surface_tool.is_valid()){
		_surface_tool.unref();
	}
	_surface_tool.instantiate();

	if (_mesh.is_valid()){
		_mesh.unref();
	}
	_mesh.instantiate();

	_surface_tool->begin(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES);

	for (int i = 0; i < marching_cubes_mesh.vertices.size(); i++)
	{
		_surface_tool->set_normal(Vector3(-marching_cubes_mesh.normals[i].x, -marching_cubes_mesh.normals[i].y, -marching_cubes_mesh.normals[i].z));
		_surface_tool->add_vertex(Vector3(marching_cubes_mesh.vertices[i].x, marching_cubes_mesh.vertices[i].y, marching_cubes_mesh.vertices[i].z));
	}

	for (int i = 0; i < marching_cubes_mesh.indices.size(); i++)
	{
		_surface_tool->add_index(marching_cubes_mesh.indices[i]);
	}

	if(material_array.size() >= 0){
		_surface_tool->set_material(material_array.get(0));
	}
	

	_surface_tool->commit(_mesh);

	

	set_base(_mesh->get_rid());

	has_changed = false;
	
}

TypedArray<Material> TheoMarchingCubes::get_material_array() const {
	return material_array;
}

void TheoMarchingCubes::set_material_array(const TypedArray<Material> &mats){
	has_changed = true;
	material_array = mats;
}

void TheoMarchingCubes::_process(double delta)
{
	if (has_changed)
	{
		process_TheoMarchingCubes();
	}
	
}
