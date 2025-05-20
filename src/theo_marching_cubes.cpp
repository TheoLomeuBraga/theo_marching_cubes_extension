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

	ClassDB::bind_method(D_METHOD("set_volume_type_on_area", "p_position", "p_volume"), &TheoMarchingCubes::set_volume_type_on_area);
	ClassDB::bind_method(D_METHOD("get_volume_type_on_area", "p_position"), &TheoMarchingCubes::get_volume_type_on_area);

	
	ClassDB::bind_method(D_METHOD("force_update"), &TheoMarchingCubes::process_TheoMarchingCubes);
	ClassDB::bind_method(D_METHOD("clean_grid"), &TheoMarchingCubes::clean_grid);

	ClassDB::bind_method(D_METHOD("get_material"), &TheoMarchingCubes::get_material);
	ClassDB::bind_method(D_METHOD("set_material", "p_material"), &TheoMarchingCubes::set_material);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");

	//colision

	ADD_GROUP("Collision", "collision_");

	ClassDB::bind_method(D_METHOD("set_use_collision", "operation"), &TheoMarchingCubes::set_use_collision);
	ClassDB::bind_method(D_METHOD("is_using_collision"), &TheoMarchingCubes::is_using_collision);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_collision"), "set_use_collision", "is_using_collision");

	ClassDB::bind_method(D_METHOD("set_collision_layer", "layer"), &TheoMarchingCubes::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_layer"), &TheoMarchingCubes::get_collision_layer);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_layer", "get_collision_layer");

	ClassDB::bind_method(D_METHOD("set_collision_mask", "mask"), &TheoMarchingCubes::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &TheoMarchingCubes::get_collision_mask);

	ClassDB::bind_method(D_METHOD("set_collision_mask_value", "layer_number", "value"), &TheoMarchingCubes::set_collision_mask_value);
	ClassDB::bind_method(D_METHOD("get_collision_mask_value", "layer_number"), &TheoMarchingCubes::get_collision_mask_value);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_mask", "get_collision_mask");

	ClassDB::bind_method(D_METHOD("set_collision_layer_value", "layer_number", "value"), &TheoMarchingCubes::set_collision_layer_value);
	ClassDB::bind_method(D_METHOD("get_collision_layer_value", "layer_number"), &TheoMarchingCubes::get_collision_layer_value);

	ClassDB::bind_method(D_METHOD("set_collision_priority", "priority"), &TheoMarchingCubes::set_collision_priority);
	ClassDB::bind_method(D_METHOD("get_collision_priority"), &TheoMarchingCubes::get_collision_priority);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "collision_priority"), "set_collision_priority", "get_collision_priority");
	
	ClassDB::bind_method(D_METHOD("bake_collision_shape"), &TheoMarchingCubes::bake_collision_shape);
	ClassDB::bind_method(D_METHOD("_get_root_collision_instance"), &TheoMarchingCubes::_get_root_collision_instance);

	
	
	
	
	
	
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

void TheoMarchingCubes::set_volume_type_on_area(const Vector3i &p_position, int p_volume)
{
	
	if (_field_data != NULL && is_cord_valid(p_position))
	{
		int32_t index = (p_position.z * grid_size.y * grid_size.x) + (p_position.y * grid_size.x) + p_position.x;
		_field_data[index] = p_volume;
		has_changed = true;
	}else{
		print_error("invalid volume area");
	}
}

int TheoMarchingCubes::get_volume_type_on_area(const Vector3i &p_position) const
{
	
	if (_field_data != NULL && is_cord_valid(p_position))
	{
		int32_t index = (p_position.z * grid_size.y * grid_size.x) + (p_position.y * grid_size.x) + p_position.x;
		return _field_data[index];
	}
	else
	{
		print_error("invalid volume area");
		return 0.0f;
	}
	
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

	if(material.is_valid()){
		_surface_tool->set_material(material);
	}

	int last_material_id = -1;

	for (int i = 0; i < marching_cubes_mesh.indices.size(); i++)
	{

		_surface_tool->add_index(marching_cubes_mesh.indices[i]);
		
	}

	_surface_tool->commit(_mesh);

	set_base(_mesh->get_rid());

	has_changed = false;
	
}

Ref<Material> TheoMarchingCubes::get_material() const {
	return material;
}

void TheoMarchingCubes::set_material(const Ref<Material> &mats){
	material = mats;
	has_changed = true;
}

void TheoMarchingCubes::set_use_collision(bool p_operation){}
bool TheoMarchingCubes::is_using_collision() const {return false;}
void TheoMarchingCubes::set_collision_layer(uint32_t p_layer){}
uint32_t TheoMarchingCubes::get_collision_layer() const{return 0;}
void TheoMarchingCubes::set_collision_mask(uint32_t p_mask){}
uint32_t TheoMarchingCubes::get_collision_mask() const{return 0;}
void TheoMarchingCubes::set_collision_mask_value(int32_t p_layer_number, bool p_value){}
bool TheoMarchingCubes::get_collision_mask_value(int32_t p_layer_number) const {return false;}
void TheoMarchingCubes::set_collision_layer_value(int32_t p_layer_number, bool p_value){}
bool TheoMarchingCubes::get_collision_layer_value(int32_t p_layer_number) const{return false;}
void TheoMarchingCubes::set_collision_priority(float p_priority){}
float TheoMarchingCubes::get_collision_priority() const{return 0.0f;}
RID TheoMarchingCubes::_get_root_collision_instance() const {return RID();}
Ref<ConcavePolygonShape3D> TheoMarchingCubes::bake_collision_shape() {return Ref<ConcavePolygonShape3D>();}

void TheoMarchingCubes::_process(double delta)
{
	if (has_changed)
	{
		process_TheoMarchingCubes();
	}
	
}
