#include "theo_marching_cubes.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/physics_server3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/engine.hpp>

#define MC_IMPLEM_ENABLE
#include "MarchingCubeCpp/MC.h"

#include <godot_cpp/core/print_string.hpp>

using namespace godot;

void TheoMarchingCubes::clean_grid_data(const Vector3i &size, MC::MC_FLOAT *data)
{
	for (int i = 0; i < size.x * size.y * size.z; i++)
	{
		data[i] = -1.0f;
	}
}

void TheoMarchingCubes::clean_grid()
{
	clean_grid_data(grid_size, _field_data);
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
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "BaseMaterial3D,ShaderMaterial"), "set_material", "get_material");
	

	// colision

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
	clean_grid_data(p_size, new_field_data);

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
	}
	else
	{
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


void TheoMarchingCubes::_clear_debug_collision_shape() {
	if (root_collision_debug_instance.is_valid()) {
		RenderingServer::get_singleton()->free_rid(root_collision_debug_instance);
		root_collision_debug_instance = RID();
	}
}

bool TheoMarchingCubes::_is_debug_collision_shape_visible() {
	return !Engine::get_singleton()->is_editor_hint() && is_inside_tree() && get_tree()->is_debugging_collisions_hint();
}

void TheoMarchingCubes::_update_debug_collision_shape() {
	if (!use_collision || root_collision_shape.is_null() || !_is_debug_collision_shape_visible()) {
		return;
	}

	ERR_FAIL_NULL(RenderingServer::get_singleton());

	if (root_collision_debug_instance.is_valid()) {
		root_collision_debug_instance = RenderingServer::get_singleton()->instance_create();
	}

	Ref<Mesh> debug_mesh = root_collision_shape->get_debug_mesh();
	RenderingServer::get_singleton()->instance_set_scenario(root_collision_debug_instance, get_world_3d()->get_scenario());
	RenderingServer::get_singleton()->instance_set_base(root_collision_debug_instance, debug_mesh->get_rid());
	RenderingServer::get_singleton()->instance_set_transform(root_collision_debug_instance, get_global_transform());
}

void TheoMarchingCubes::process_TheoMarchingCubes()
{

	MC::mcMesh marching_cubes_mesh;
	MC::marching_cube(_field_data, grid_size.x, grid_size.y, grid_size.z, marching_cubes_mesh);

	if (_surface_tool.is_valid())
	{
		_surface_tool.unref();
	}
	_surface_tool.instantiate();

	if (_mesh.is_valid())
	{
		_mesh.unref();
	}
	_mesh.instantiate();

	_surface_tool->begin(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES);

	for (int i = 0; i < marching_cubes_mesh.vertices.size(); i++)
	{
		_surface_tool->set_normal(Vector3(-marching_cubes_mesh.normals[i].x, -marching_cubes_mesh.normals[i].y, -marching_cubes_mesh.normals[i].z));
		_surface_tool->add_vertex(Vector3(marching_cubes_mesh.vertices[i].x, marching_cubes_mesh.vertices[i].y, marching_cubes_mesh.vertices[i].z));
	}

	if (material.is_valid())
	{
		_surface_tool->set_material(material);
	}

	for (int i = 0; i < marching_cubes_mesh.indices.size(); i++)
	{
		_surface_tool->add_index(marching_cubes_mesh.indices[i]);
	}

	_surface_tool->commit(_mesh);

	set_base(_mesh->get_rid());

	_update_collision_faces();

	has_changed = false;
}

Ref<Material> TheoMarchingCubes::get_material() const
{
	return material;
}

void TheoMarchingCubes::set_material(const Ref<Material> &mats)
{
	material = mats;
	has_changed = true;
}


void TheoMarchingCubes::set_use_collision(bool p_enable) {
	if (use_collision == p_enable) {
		return;
	}

	use_collision = p_enable;

	if (!is_inside_tree()) {
		return;
	}

	if (use_collision) {
		root_collision_shape.instantiate();
		root_collision_instance = PhysicsServer3D::get_singleton()->body_create();
		PhysicsServer3D::get_singleton()->body_set_mode(root_collision_instance, PhysicsServer3D::BODY_MODE_STATIC);
		PhysicsServer3D::get_singleton()->body_set_state(root_collision_instance, PhysicsServer3D::BODY_STATE_TRANSFORM, get_global_transform());
		PhysicsServer3D::get_singleton()->body_add_shape(root_collision_instance, root_collision_shape->get_rid());
		PhysicsServer3D::get_singleton()->body_set_space(root_collision_instance, get_world_3d()->get_space());
		PhysicsServer3D::get_singleton()->body_attach_object_instance_id(root_collision_instance, get_instance_id());
		set_collision_layer(collision_layer);
		set_collision_mask(collision_mask);
		set_collision_priority(collision_priority);
		process_TheoMarchingCubes();
	} else {
		PhysicsServer3D::get_singleton()->free_rid(root_collision_instance);
		root_collision_instance = RID();
		root_collision_shape.unref();
	}
	notify_property_list_changed();
}
bool TheoMarchingCubes::is_using_collision() const
{
	return use_collision;
}

void TheoMarchingCubes::set_collision_layer(uint32_t p_layer)
{
	collision_layer = p_layer;
	if (root_collision_instance.is_valid())
	{
		PhysicsServer3D::get_singleton()->body_set_collision_layer(root_collision_instance, p_layer);
	}
}
uint32_t TheoMarchingCubes::get_collision_layer() const
{
	return collision_layer;
}

void TheoMarchingCubes::set_collision_mask(uint32_t p_mask)
{
	collision_mask = p_mask;
	if (root_collision_instance.is_valid())
	{
		PhysicsServer3D::get_singleton()->body_set_collision_mask(root_collision_instance, p_mask);
	}
}
uint32_t TheoMarchingCubes::get_collision_mask() const
{
	return collision_mask;
}

void TheoMarchingCubes::set_collision_mask_value(int32_t p_layer_number, bool p_value)
{
	ERR_FAIL_COND_MSG(p_layer_number < 1, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_MSG(p_layer_number > 32, "Collision layer number must be between 1 and 32 inclusive.");
	uint32_t mask = get_collision_mask();
	if (p_value)
	{
		mask |= 1 << (p_layer_number - 1);
	}
	else
	{
		mask &= ~(1 << (p_layer_number - 1));
	}
	set_collision_mask(mask);
}
bool TheoMarchingCubes::get_collision_mask_value(int32_t p_layer_number) const
{
	ERR_FAIL_COND_V_MSG(p_layer_number < 1, false, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_V_MSG(p_layer_number > 32, false, "Collision layer number must be between 1 and 32 inclusive.");
	return get_collision_mask() & (1 << (p_layer_number - 1));
}
void TheoMarchingCubes::set_collision_layer_value(int32_t p_layer_number, bool p_value)
{
	ERR_FAIL_COND_MSG(p_layer_number < 1, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_MSG(p_layer_number > 32, "Collision layer number must be between 1 and 32 inclusive.");
	uint32_t layer = get_collision_layer();
	if (p_value)
	{
		layer |= 1 << (p_layer_number - 1);
	}
	else
	{
		layer &= ~(1 << (p_layer_number - 1));
	}
	set_collision_layer(layer);
}
bool TheoMarchingCubes::get_collision_layer_value(int32_t p_layer_number) const
{
	ERR_FAIL_COND_V_MSG(p_layer_number < 1, false, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_V_MSG(p_layer_number > 32, false, "Collision layer number must be between 1 and 32 inclusive.");
	return get_collision_layer() & (1 << (p_layer_number - 1));
}

void TheoMarchingCubes::set_collision_priority(float p_priority)
{
	collision_priority = p_priority;
	if (root_collision_instance.is_valid())
	{
		PhysicsServer3D::get_singleton()->body_set_collision_priority(root_collision_instance, p_priority);
	}
}
float TheoMarchingCubes::get_collision_priority() const
{
	return collision_priority;
}

RID TheoMarchingCubes::_get_root_collision_instance() const
{
	if (root_collision_instance.is_valid())
	{
		return root_collision_instance;
	}

	return RID();
}

/*

*/

PackedVector3Array TheoMarchingCubes::_get_brush_collision_faces() {

	PackedVector3Array collision_faces;

	for (int surface_idx = 0; surface_idx < _mesh->get_surface_count(); surface_idx++)
	{
		Array surface_arrays = _mesh->surface_get_arrays(surface_idx);
		PackedVector3Array vertices = surface_arrays[ArrayMesh::ARRAY_VERTEX];
		PackedInt32Array indices = surface_arrays[ArrayMesh::ARRAY_INDEX];

		if (indices.size() == 0)
		{
			for (int i = 0; i < vertices.size(); i += 3)
			{
				if (i + 2 >= vertices.size())
					break;
				collision_faces.append(vertices[i]);
				collision_faces.append(vertices[i + 1]);
				collision_faces.append(vertices[i + 2]);
			}
		}
		else
		{
			for (int i = 0; i < indices.size(); i += 3)
			{
				if (i + 2 >= indices.size())
					break;
				collision_faces.append(vertices[indices[i]]);
				collision_faces.append(vertices[indices[i + 1]]);
				collision_faces.append(vertices[indices[i + 2]]);
			}
		}
	}

	return collision_faces;
}

void TheoMarchingCubes::_update_collision_faces() {
	if (use_collision && root_collision_shape.is_valid()) {
		root_collision_shape->set_faces(_get_brush_collision_faces());

		if (_is_debug_collision_shape_visible()) {
			_update_debug_collision_shape();
		}
	}
}

Ref<ConcavePolygonShape3D> TheoMarchingCubes::bake_collision_shape()
{
	if (root_collision_shape.is_valid())
	{
		root_collision_shape.unref();
	}
	PackedVector3Array triangle_data;

	for (int surface_idx = 0; surface_idx < _mesh->get_surface_count(); surface_idx++)
	{
		Array surface_arrays = _mesh->surface_get_arrays(surface_idx);
		PackedVector3Array vertices = surface_arrays[ArrayMesh::ARRAY_VERTEX];
		PackedInt32Array indices = surface_arrays[ArrayMesh::ARRAY_INDEX];

		if (indices.size() == 0)
		{
			for (int i = 0; i < vertices.size(); i += 3)
			{
				if (i + 2 >= vertices.size())
					break;
				triangle_data.append(vertices[i]);
				triangle_data.append(vertices[i + 1]);
				triangle_data.append(vertices[i + 2]);
			}
		}
		else
		{
			for (int i = 0; i < indices.size(); i += 3)
			{
				if (i + 2 >= indices.size())
					break;
				triangle_data.append(vertices[indices[i]]);
				triangle_data.append(vertices[indices[i + 1]]);
				triangle_data.append(vertices[indices[i + 2]]);
			}
		}
	}

	root_collision_shape->set_faces(triangle_data);

	return root_collision_shape;
}

void TheoMarchingCubes::_on_transform_changed() {
	if (root_collision_debug_instance.is_valid() && !debug_shape_old_transform.is_equal_approx(get_global_transform())) {
		debug_shape_old_transform = get_global_transform();
		RenderingServer::get_singleton()->instance_set_transform(root_collision_debug_instance, debug_shape_old_transform);
	}
}

void TheoMarchingCubes::_notification(int p_what)
{
	
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			
			if (use_collision) {
				root_collision_shape.instantiate();
				root_collision_instance = PhysicsServer3D::get_singleton()->body_create();
				PhysicsServer3D::get_singleton()->body_set_mode(root_collision_instance, PhysicsServer3D::BODY_MODE_STATIC);
				PhysicsServer3D::get_singleton()->body_set_state(root_collision_instance, PhysicsServer3D::BODY_STATE_TRANSFORM, get_global_transform());
				PhysicsServer3D::get_singleton()->body_add_shape(root_collision_instance, root_collision_shape->get_rid());
				PhysicsServer3D::get_singleton()->body_set_space(root_collision_instance, get_world_3d()->get_space());
				PhysicsServer3D::get_singleton()->body_attach_object_instance_id(root_collision_instance, get_instance_id());
				set_collision_layer(collision_layer);
				set_collision_mask(collision_mask);
				set_collision_priority(collision_priority);
				process_TheoMarchingCubes();
			}
		} break;

		case NOTIFICATION_EXIT_TREE: {
			if (use_collision && root_collision_instance.is_valid()) {
				PhysicsServer3D::get_singleton()->free_rid(root_collision_instance);
				root_collision_instance = RID();
				root_collision_shape.unref();
			}
		} break;

		case NOTIFICATION_TRANSFORM_CHANGED: {
			if (use_collision && root_collision_instance.is_valid()) {
				PhysicsServer3D::get_singleton()->body_set_state(root_collision_instance, PhysicsServer3D::BODY_STATE_TRANSFORM, get_global_transform());
			}
			_on_transform_changed();
		} break;
	}
	
}


void TheoMarchingCubes::_process(double delta)
{
	if (has_changed)
	{
		process_TheoMarchingCubes();
	}
}
