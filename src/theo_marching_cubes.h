#ifndef THEO_MARCHING_CUBES_H
#define THEO_MARCHING_CUBES_H

#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/geometry_instance3d.hpp>
#include <godot_cpp/classes/visual_instance3d.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>

#include "MarchingCubeCpp/MC.h"

namespace godot
{
	

	class TheoMarchingCubes : public GeometryInstance3D
	{
		GDCLASS(TheoMarchingCubes, GeometryInstance3D)
		

	private:
	protected:
		static void _bind_methods();

		bool has_changed;
		Vector3 grid_size;
		MC::MC_FLOAT* _field_data = NULL;
		Ref<SurfaceTool> _surface_tool;
		Ref<ArrayMesh> _mesh;
		Ref<Material> material;

		bool threaded;

		bool use_collision = false;
		uint32_t collision_layer = 1;
		uint32_t collision_mask = 1;
		real_t collision_priority = 1.0;
		Ref<ConcavePolygonShape3D> root_collision_shape;
		RID root_collision_instance;

		RID root_collision_debug_instance;
		Transform3D debug_shape_old_transform;



	public:
		TheoMarchingCubes();
		~TheoMarchingCubes();

		void _update_collision_faces();
		bool _is_debug_collision_shape_visible();
		void _update_debug_collision_shape();
		void _clear_debug_collision_shape();
		void _on_transform_changed();
		PackedVector3Array _get_brush_collision_faces();

		void process_TheoMarchingCubes();

		void _process(double delta) override;
		void _notification(int p_what);

		// grid
		void set_grid_size(const Vector3i &p_size);
		Vector3i get_grid_size() const;
		bool is_cord_valid(const Vector3i &p_position) const;

		void clean_grid_data(const Vector3i &size, MC::MC_FLOAT* data);
		void clean_grid();

		// volume
		void set_volume_type_on_area(const Vector3i &p_position, int p_volume);
		int get_volume_type_on_area(const Vector3i &p_position) const;

		// thread
		//bool get_mult_thread() const;
		//void set_mult_thread(const bool &is_threaded);

		// material
		Ref<Material> get_material() const;
		void set_material(const Ref<Material> &mats);

		// collision
		void set_use_collision(bool p_operation);
		bool is_using_collision() const;
		void set_collision_layer(uint32_t p_layer);
		uint32_t get_collision_layer() const;
		void set_collision_mask(uint32_t p_mask);
		uint32_t get_collision_mask() const;
		void set_collision_mask_value(int32_t p_layer_number, bool p_value);
		bool get_collision_mask_value(int32_t p_layer_number) const;
		void set_collision_layer_value(int32_t p_layer_number, bool p_value);
		bool get_collision_layer_value(int32_t p_layer_number) const;
		void set_collision_priority(float p_priority);
		float get_collision_priority() const;
		RID _get_root_collision_instance() const;
		Ref<ConcavePolygonShape3D> bake_collision_shape();
	};

}

#endif