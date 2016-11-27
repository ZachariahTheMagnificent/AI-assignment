#pragma once
#include "Vector3.h"

class Map
{
public:
	Map(const float position_x, const float position_y, const float map_size_x, const float map_size_y)
	{
		position = Vector3(position_x, position_y, 0);
		this->map_size_x = map_size_x;
		this->map_size_y = map_size_y;
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

private:
	float current_time;
	Vector3 position;
	float map_size_x;
	float map_size_y;
};