#pragma once
#include "Vector3.h"

class Map
{
public:
	Map(const float position_x, const float position_y, const float map_size_x, const float map_size_y)
	:
	current_time(0),
	position(position_x, position_y, 0),
	map_size_x(map_size_x),
	map_size_y(map_size_y)
	{
	}

	float StartingRegionX()
	{
		return position.x - map_size_x / 2;
	}

	float StartingRegionY()
	{
		return position.y - map_size_y / 2;
	}

	float EndingRegionX()
	{
		return position.x + map_size_x / 2;
	}

	float EndingRegionY()
	{
		return position.y + map_size_y / 2;
	}

	float GetCurrTime()
	{
		return current_time;
	}

	void Update(const double delta_time)
	{
		current_time += delta_time;

		if (current_time >= NIGHT_TIME_END)
		{
			current_time = DAY_TIME_START;
		}
	}

	bool IsDaytime()
	{
		return true;
		return current_time >= DAY_TIME_START && current_time < DAY_TIME_END;
	}

private:
	const double DAY_TIME_START = 0;
	const double DAY_TIME_END = 50;
	const double NIGHT_TIME_END = 100;
	float current_time;
	Vector3 position;
	float map_size_x;
	float map_size_y;
};