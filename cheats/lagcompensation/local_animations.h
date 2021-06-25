#include "..\..\includes.hpp"

struct Local_data
{
	bool visualize_lag = false;

	c_baseplayeranimationstate* prediction_animstate = nullptr;
	c_baseplayeranimationstate* animstate = nullptr;

	Vector stored_real_angles = ZERO;
	Vector real_angles = ZERO;
	Vector fake_angles = ZERO;
};

class local_animations : public singleton <local_animations>
{
	bool real_server_update = false;
	bool fake_server_update = false;

	float real_simulation_time = 0.0f;
	float fake_simulation_time = 0.0f;

	CBaseHandle* handle = nullptr;

	float spawntime = 0.0f;
	float tickcount = 0.0f;

	float abs_angles = 0.0f;
	float pose_parameter[24];
	AnimationLayer layers[15];

	void update_fake_animations();
	void update_local_animations(c_baseplayeranimationstate* animstate);
public:
	Local_data local_data;

	void run(ClientFrameStage_t stage);
	void update_prediction_animations();
};