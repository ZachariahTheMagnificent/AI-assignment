#pragma once
#include "Vector3.h"
#include "Rabbit.h"

class Arrow
{
public:
	Arrow(RabbitSystem& rabbit_system, const Vector3& position, const Vector3& direction)
	:
	rabbit_system(rabbit_system),
	position(position),
	direction(direction),
	radius(5),
	speed(20),
	max_life(5),
	current_life(0)
	{
	}

	Arrow& operator=(const Arrow& arrow)
	{
		position = arrow.position;
		direction = arrow.direction;
		radius = arrow.radius;
		speed = arrow.speed;
		max_life = arrow.max_life;
		current_life = arrow.current_life;
		return *this;
	}

	const Vector3& GetPosition() const
	{
		return position;
	}

	void Update(const double delta_time)
	{
		if (!IsDead())
		{
			current_life += delta_time;
			position += direction * speed * delta_time;

			for (auto& rabbit : rabbit_system.GetRabbits())
			{
				if (rabbit.GetState() != Rabbit::A_STATE::DEAD)
				{
					if ((rabbit.Pos() - position).LengthSquared() <= radius * radius)
					{
						rabbit.UponDeath();
					}
				}
			}
		}
	}

	bool IsDead() const
	{
		return current_life >= max_life;
	}

private:
	RabbitSystem& rabbit_system;
	Vector3 position;
	Vector3 direction;
	float radius;
	float speed;
	float max_life;
	float current_life;
};

class ArrowSystem
{
public:
	ArrowSystem(RabbitSystem& rabbit_system)
	:
	rabbit_system(rabbit_system)
	{
	}

	void Update(const double delta_time)
	{
		for (auto& arrow : arrows)
		{
			arrow.Update(delta_time);
		}

		std::vector<Arrow> temp_vector;
		for (auto& arrow : arrows)
		{
			if (!arrow.IsDead())
			{
				temp_vector.push_back(arrow);
			}
		}
		temp_vector.swap(arrows);
	}

	void CreateArrow(const Vector3& position, const Vector3& direction)
	{
		arrows.push_back(Arrow(rabbit_system, position, direction));
	}

	std::vector<Arrow>& GetArrows()
	{
		return arrows;
	}

private:
	RabbitSystem& rabbit_system;
	std::vector<Arrow> arrows;
};