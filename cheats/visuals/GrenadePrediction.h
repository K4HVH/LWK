#include "..\..\includes.hpp"

class Vector;
class QAngle;
class CViewSetup;

enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class GrenadePrediction : public singleton<GrenadePrediction>
{
private:
	std::vector<Vector> path;
	std::vector<std::pair<Vector, Color>> others;
	int type = 0;
	int act = 0;

	void Setup(Vector& vecSrc, Vector& vecThrow, const Vector& viewangles);
	void Simulate(CViewSetup* setup);

	int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval);
	bool CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval);

	void TraceHull(Vector& src, Vector& end, trace_t& tr);
	void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground);
	void PushEntity(Vector& src, const Vector& move, trace_t& tr);
	void ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, const Vector& move, float interval);
	int PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
public:
	void Tick(int buttons);
	void View(CViewSetup* setup, weapon_t* weapon);
	void Paint();
};