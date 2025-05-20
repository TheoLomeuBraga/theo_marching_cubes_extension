#ifndef THEO_NOISY_CUBES_H
#define THEO_NOISY_CUBES_H

#include <godot_cpp/classes/geometry_instance3d.hpp>
#include "theo_marching_cubes.h"

namespace godot
{

	class TheoNoisyCubes : public TheoMarchingCubes
	{
		GDCLASS(TheoNoisyCubes, TheoMarchingCubes)

	private:
	float noise_scale;
	float noise_thicknes;

	protected:
		static void _bind_methods();

	public:
		TheoNoisyCubes();
		~TheoNoisyCubes();

		void process_TheoNoisyCubes();

		void _process(double delta) override;

		//noise
		void generate_noise();
		

		void set_noise_scale(const float p_scale);
		float get_noise_scale() const;

		void set_noise_thicknes(const float p_thicknes);
		float get_noise_thicknes() const;
	};

}

#endif