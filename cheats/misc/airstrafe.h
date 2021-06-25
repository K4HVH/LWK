#include "..\..\includes.hpp"
#define M_RADPI 57.295779513082f

class airstrafe : public singleton <airstrafe>
{
public:
	void create_move(CUserCmd* m_pcmd);
};