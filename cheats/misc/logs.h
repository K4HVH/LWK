#pragma once

#include "..\..\includes.hpp"
#include <deque>

class eventlogs : public singleton <eventlogs> 
{
public:
	void paint_traverse();
	void events(IGameEvent* event);
	void add(std::string text, bool full_display = true);

	bool last_log = false;
private:
	struct loginfo_t
	{
		loginfo_t(float log_time, std::string message, const Color& color)  //-V818
		{
			this->log_time = log_time;
			this->message = message; //-V820
			this->color = color;

			x = 6.0f;
			y = 0.0f;
		}

		float log_time;
		std::string message;
		Color color;
		float x, y;
	};

	std::deque <loginfo_t> logs;
};