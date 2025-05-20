#ifndef THEO_MARCHING_CUBES_H
#define THEO_MARCHING_CUBES_H

#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/geometry_instance3d.hpp>
#include <godot_cpp/classes/surface_tool.hpp>

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

		MC::MC_FLOAT* _field_data;
		Ref<SurfaceTool> _surface_tool;
		Ref<ArrayMesh> _mesh;

		TypedArray<Material> material_array;

	public:
		TheoMarchingCubes();
		~TheoMarchingCubes();

		void process_TheoMarchingCubes();

		void _process(double delta) override;

		// grid
		void set_grid_size(const Vector3i &p_size);
		Vector3i get_grid_size() const;
		bool is_cord_valid(const Vector3i &p_position) const;

		void clean_grid_data(const Vector3i &size, MC::MC_FLOAT* data);
		void clean_grid();

		// volume
		void set_volume_on_area(const Vector3i &p_position, float p_volume);
		float get_volume_on_area(const Vector3i &p_position) const;
		void set_volume_type_on_area(const Vector3i &p_position, int32_t p_volume_type);
		int32_t get_volume_type_on_area(const Vector3i &p_position) const;

		//test
		Ref<Mesh> get_mesh() const;
		void set_mesh(const Ref<Mesh> &p_mesh);

		// geometry
		//void set_smooth_faces(bool p_smooth_faces);
		//bool get_smooth_faces() const;

		// material
		TypedArray<Material> get_material_array() const;
		void set_material_array(const TypedArray<Material> &mats);

		// collision
		//void set_use_collision(bool p_operation);
		//bool is_using_collision() const;
		//void set_collision_layer(uint32_t p_layer);
		//uint32_t get_collision_layer() const;
		//void set_collision_mask(uint32_t p_mask);
		//uint32_t get_collision_mask() const;
		//void set_collision_mask_value(int32_t p_layer_number, bool p_value);
		//bool get_collision_mask_value(int32_t p_layer_number) const;
		//void set_collision_layer_value(int32_t p_layer_number, bool p_value);
		//bool get_collision_layer_value(int32_t p_layer_number) const;
		//void set_collision_priority(float p_priority);
		//float get_collision_priority() const;
	};

}

#endif