#pragma once
#include <vector>
#include <memory>
#include <float.h>
#include <functional>
#include "CheckIfVectorsCollide.h"
#include "Vector3.h"

class CountdownSystem final
{
public:
	void Update ( std::vector<double>& timers, const double delta_time )
	{
		for ( double& timer : timers )
		{
			timer -= delta_time;
		}
	}
};

class PhysicsSystem final
{
public:
	void Update ( std::vector<Vector3>& positions, std::vector<Vector3>& directions, std::vector<float>& speeds, const double delta_time )
	{
		for ( std::size_t index = 0, size = positions.size ( ); index < size; ++index )
		{
			positions [ index ] += directions [ index ] * speeds [ index ] * delta_time;
		}
	}
};

struct CollisionSystem final
{
public:
	template<class CollisionCallback>
	void CheckCollision ( std::vector<Vector3>& positions, std::vector<float>& radiuses, CollisionCallback& OnCollision )
	{
		const std::size_t size = positions.size ( );

		for ( std::size_t index1 = 0; index1 < size; ++index1 )
		{
			for ( std::size_t index2 = 0; index2 < size; ++index, ++index2 )
			{
				const float distance_squared = ( positions [ index1 ] - positions [ index2 ] ).LengthSquared ( );
				const float total_radius = radiuses [ index1 ] + radiuses [ index2 ];
				const float total_radius_squared = total_radius * total_radius;
				if ( distance_squared <= total_radius_squared )
				{
					OnCollision ( index1, index2, distance_squared );
				}
			}
		}
	}

	template<class CollisionCallback>
	void CheckCollision ( std::vector<Vector3>& positions1, std::vector<float>& radiuses1, std::vector<Vector3>& positions2, std::vector<float>& radiuses2, CollisionCallback& OnCollision )
	{
		std::size_t size1 = positions1.size ( );
		std::size_t size2 = positions2.size ( );

		for ( std::size_t index1 = 0; index1 < size1; ++index1 )
		{
			for ( std::size_t index2 = 0; index2 < size2; ++index2 )
			{
				const float distance_squared = ( positions1 [ index1 ] - positions2 [ index2 ] ).LengthSquared ( );
				const float total_radius = radiuses1 [ index1 ] + radiuses2 [ index2 ];
				const float total_radius_squared = total_radius * total_radius;
				if ( distance_squared <= total_radius_squared )
				{
					OnCollision ( index1, index2, distance_squared );
				}
			}
		}
	}
};

struct PathNodeTravelInfo final
{
	void ConnectTo ( const std::vector<PathNodeTravelInfo>::const_iterator travel_info_it, const float new_distance_travelled_squared, const float new_total_travel_cost )
	{
		calculated = true;
		if ( new_total_travel_cost <= total_travel_cost )
		{
			distance_travelled_squared = new_distance_travelled_squared;
			total_travel_cost = new_total_travel_cost;
			previous_travel_info_it = travel_info_it;
		}
	}

	float distance_travelled_squared { FLT_MAX };
	float distance_to_goal_squared { FLT_MAX };
	float total_travel_cost { FLT_MAX };
	bool calculated { false };
	std::vector<PathNodeTravelInfo>::const_iterator previous_travel_info_it;
};

class PathNode final
{
public:
	PathNode ( ) = default;

	void AddNeighbour ( std::vector<PathNodeTravelInfo>::iterator travel_info, const Vector3 position )
	{
		neighbour_positions_.push_back ( position );
		neighbour_travel_info_.push_back ( travel_info );
		new_distance_travelled_squared_cache_.push_back ( float { } );
	}

	void CalculateNeighbours ( std::vector<std::size_t>& calculated_unvisited_nodes_indices, const std::vector<PathNodeTravelInfo>::const_iterator travel_info_begin, const std::vector<PathNodeTravelInfo>::const_iterator self_travel_info, const Vector3 position ) const
	{
		const float distance_travelled_squared = self_travel_info->distance_travelled_squared;
		const std::size_t size = neighbour_positions_.size ( );

		for ( std::size_t index = 0; index < size; ++index )
		{
			new_distance_travelled_squared_cache_ [ index ] = ( neighbour_positions_ [ index ] - position ).LengthSquared ( ) + distance_travelled_squared;
		}

		for ( std::size_t index = 0; index < size; ++index )
		{
			const auto travel_info_it = neighbour_travel_info_ [ index ];

			const float new_distance_travelled_squared = new_distance_travelled_squared_cache_ [ index ];
			const float new_total_travel_cost = new_distance_travelled_squared + travel_info_it->distance_to_goal_squared;

			if ( !travel_info_it->calculated )
			{
				calculated_unvisited_nodes_indices.push_back ( travel_info_it - travel_info_begin );
			}

			travel_info_it->ConnectTo ( self_travel_info, new_distance_travelled_squared, new_total_travel_cost );
		}
	}

private:
	std::vector<Vector3> neighbour_positions_;
	std::vector<std::vector<PathNodeTravelInfo>::iterator> neighbour_travel_info_;

	mutable std::vector<float> new_distance_travelled_squared_cache_;
};

class PathFinder final
{
public:
	void Build ( const std::vector<Vector3>& positions, const std::vector<Line>& line_boundaries )
	{
		const std::size_t num_nodes = positions.size ( );
		const std::size_t num_boundaries = line_boundaries.size ( );

		line_boundaries_.resize ( num_boundaries );

		node_positions_.resize ( num_nodes );
		path_nodes_.resize ( num_nodes );
		calculated_unvisited_nodes_index_cache_.resize ( num_nodes );
		travel_info_cache_.resize ( num_nodes );

		line_boundaries_ = line_boundaries;

		for ( std::size_t index = 0; index < num_nodes; ++index )
		{
			node_positions_ [ index ] = positions [ index ];
		}

		const auto travel_info_begin = travel_info_cache_.begin ( );

		for ( std::size_t index1 = 0; index1 < num_nodes; ++index1 )
		{
			for ( std::size_t index2 = 0; index2 < num_nodes; ++index2 )
			{
				const Vector3 node_position1 = node_positions_ [ index1 ];
				const Vector3 node_position2 = node_positions_ [ index2 ];

				if ( Connected ( node_position1, node_position2 ) )
				{
					path_nodes_ [ index1 ].AddNeighbour ( travel_info_begin + index1, node_position1 );
					path_nodes_ [ index2 ].AddNeighbour ( travel_info_begin + index2, node_position2 );
				}
			}
		}
	}

	Vector3 FindNextNodePositionInShortestPath ( const Vector3 starting_position, const Vector3 goal )
	{
		if ( Connected ( starting_position, goal ) )
		{
			return goal;
		}

		const std::size_t num_nodes = node_positions_.size ( );

		//reset travel_info_buffer
		travel_info_cache_.clear ( );
		travel_info_cache_.resize ( num_nodes );

		//Calculate distance to goal for all nodes
		for ( std::size_t index = 0; index < num_nodes; ++index )
		{
			const Vector3 node_to_goal = goal - node_positions_ [ index ];
			travel_info_cache_ [ index ].distance_to_goal_squared = node_to_goal.LengthSquared ( );
		}

		//Find all nodes connected to starting position and add them to the processing array
		calculated_unvisited_nodes_index_cache_.clear ( );
		for ( std::size_t index = 0; index < num_nodes; ++index )
		{
			const Vector3 node_position = node_positions_ [ index ];

			PathNodeTravelInfo& travel_info = travel_info_cache_ [ index ];

			if ( Connected ( starting_position, node_position ) )
			{
				const Vector3 start_to_node = node_position - starting_position;
				travel_info.distance_travelled_squared = start_to_node.LengthSquared ( );
				travel_info.total_travel_cost = travel_info.distance_travelled_squared + travel_info.distance_to_goal_squared;
				calculated_unvisited_nodes_index_cache_.push_back ( index );
			}
		}

		//loop until there is nothing else to search
		for ( std::size_t size = calculated_unvisited_nodes_index_cache_.size ( ); size != 0; size = calculated_unvisited_nodes_index_cache_.size ( ) )
		{
			//Get the lowest cost node to calculate neighbours and add any unvisited to the processing array. Then remove it.
			std::size_t index_of_node_to_process = calculated_unvisited_nodes_index_cache_.front ( );
			float total_travel_cost = travel_info_cache_ [ index_of_node_to_process ].total_travel_cost;
			std::size_t element_to_delete_index = 0;
			for ( std::size_t index = 1; index < size; ++index )
			{
				const std::size_t node_index = calculated_unvisited_nodes_index_cache_ [ index ];

				const float new_total_travel_cost = travel_info_cache_ [ node_index ].total_travel_cost;
				if ( new_total_travel_cost < total_travel_cost )
				{
					index_of_node_to_process = node_index;
					total_travel_cost = new_total_travel_cost;
					element_to_delete_index = index;
				}
			}

			calculated_unvisited_nodes_index_cache_.erase ( calculated_unvisited_nodes_index_cache_.begin ( ) + element_to_delete_index );

			//break if node is connected to the goal
			const Vector3 position = node_positions_ [ index_of_node_to_process ];
			const auto travel_info_begin = travel_info_cache_.begin ( );
			if ( Connected ( goal, position ) )
			{
				//backtrack to find next node
				std::vector<PathNodeTravelInfo>::const_iterator current = travel_info_begin + index_of_node_to_process;
				while ( current->calculated )
				{
					current = current->previous_travel_info_it;
				}

				const std::size_t index = current - travel_info_begin;
				return node_positions_ [ index ];
			}
			
			path_nodes_ [ index_of_node_to_process ].CalculateNeighbours ( calculated_unvisited_nodes_index_cache_, travel_info_begin, travel_info_begin + index_of_node_to_process, position );
		}

		throw;
	}

private:
	bool Connected ( const Vector3 position1, const Vector3 position2 )
	{
		bool collide = false;

		const Vector3 from_1_to_2 = position2 - position1;

		for ( const Line line : line_boundaries_ )
		{
			if ( CheckIfVectorsCollide ( line.orgin, line.vector, position1, from_1_to_2 ) )
			{
				collide = true;
			}
		}
		return !collide;
	}

	std::vector<Line> line_boundaries_;
	std::vector<Vector3> node_positions_;
	std::vector<PathNode> path_nodes_;

	mutable std::vector<std::size_t> calculated_unvisited_nodes_index_cache_;
	mutable std::vector<PathNodeTravelInfo> travel_info_cache_;
};

enum class LeaderState
{
	SEARCH_TREASURE,
	TAKE_TREASURE,
	ATTACK,
	DEAD
};
enum class HealerState
{
	FOLLOW,
	HEAL,
	CLOAK,
	DEAD
};
enum class TankState
{
	FOLLOW,
	ATTACK,
	TAUNT,
	DEAD
};
enum class ArcherState
{
	FOLLOW,
	ATTACK,
	SPEED_FIRE,
	DEAD
};
enum class MonsterState
{
	WAIT,
	ATTACK,
	DEAD
};

struct Arrows final
{
	const float default_speed { 250 };
	const float default_radius { 2 };
	const double default_time_until_death { 5 };
	const double default_dead { false };
	const float default_damage { 5 };

	void Create ( const Vector3 position, const Vector3 direction )
	{
		for ( std::size_t index = 0, size = positions.size ( ); index < size; ++index )
		{
			if ( deads [ index ] )
			{
				positions [ index ] = ( position );
				directions [ index ] = ( direction );
				speeds [ index ] = ( default_speed );
				radiuses [ index ] = ( default_radius );
				deads [ index ] = ( default_dead );
				time_until_deaths [ index ] = ( default_time_until_death );
			}
		}

		positions.push_back ( position );
		directions.push_back ( direction );
		speeds.push_back ( default_speed );
		radiuses.push_back ( default_radius );
		deads.push_back ( default_dead );
		time_until_deaths.push_back ( default_time_until_death );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radiuses;
	std::vector<bool> deads;
	std::vector<double> time_until_deaths;
};

struct Monsters final
{
	const float default_speed { 20 };
	const float default_radius { 5 };
	const float default_agrro_radius_squared { 150 * 150 };
	const double default_time_until_next_attack { 1 };
	const MonsterState default_state { MonsterState::WAIT };
	const float default_health { 100 };
	const float default_damage { 10 };

	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( default_speed );
		radiuses.push_back ( default_radius );
		healths.push_back ( default_health );
		time_until_next_attacks.push_back ( default_time_until_next_attack );
		states.push_back ( default_state );
		aggro_radius_squareds.push_back ( default_agrro_radius_squared );
		TargetInRange.push_back ( [ ]
		{
			return false;
		}
		);
		TargetIsDead.push_back ( [ ]
		{
			return false;
		}
		);
		GetTargetPosition.push_back ( [ ]
		{
			return Vector3 { };
		}
		);
		DamageTarget.push_back ( [ ] ( const float damage )
		{
		}
		);
	}
	void Clear ( )
	{
		positions.clear ( );
		directions.clear ( );
		speeds.clear ( );
		radiuses.clear ( );
		healths.clear ( );
		time_until_next_attacks.clear ( );
		states.clear ( );
		aggro_radius_squareds.clear ( );
	}

	void GetDamage ( const std::size_t index, const float damage )
	{
		healths [ index ] -= damage;
		if ( healths [ index ] <= 0 )
		{
			states [ index ] = MonsterState::DEAD;
		}
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radiuses;
	std::vector<float> healths;
	std::vector<double> time_until_next_attacks;
	std::vector<MonsterState> states;
	std::vector<float> aggro_radius_squareds;
	std::vector<std::function<bool ( )>> TargetInRange;
	std::vector<std::function<bool ( )>> TargetIsDead;
	std::vector<std::function<Vector3 ( )>> GetTargetPosition;
	std::vector<std::function<void ( const float damage )>> DamageTarget;
};

struct Healers final
{
	const float default_speed { 50 };
	const float default_radius { 5 };
	const float default_health { 100 };
	const float default_percentage_heal { 30 };
	const double default_time_until_next_heal { 5 };
	const double default_time_until_cloak { 3 };
	const HealerState default_state { HealerState::FOLLOW };

	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( default_speed );
		radiuses.push_back ( default_radius );
		healths.push_back ( default_health );
		time_until_next_heals.push_back ( default_time_until_next_heal );
		time_until_cloaks.push_back ( default_time_until_cloak );
		states.push_back ( default_state );
		HealTarget.push_back ( [ ] ( const float heal )
		{
		}
		);
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radiuses;
	std::vector<float> healths;
	std::vector<double> time_until_next_heals;
	std::vector<double> time_until_cloaks;
	std::vector<HealerState> states;
	std::vector<std::function<void ( const float heal )>> HealTarget;
};

struct Archers final
{
	const float default_speed { 70 };
	const float default_radius { 5 };
	const float default_health { 100 };
	const double default_time_until_next_arrow { 1 };
	const double default_time_until_speed_fire_end { 3 };
	const ArcherState default_state { ArcherState::FOLLOW };

	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( default_speed );
		radiuses.push_back ( default_radius );
		healths.push_back ( default_health );
		time_until_next_arrows.push_back ( default_time_until_next_arrow );
		time_until_speed_fire_ends.push_back ( default_time_until_speed_fire_end );
		states.push_back ( default_state );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radiuses;
	std::vector<float> healths;
	std::vector<double> time_until_next_arrows;
	std::vector<double> time_until_speed_fire_ends;
	std::vector<ArcherState> states;
};

struct Tanks final
{
	const float default_speed { 45 };
	const float default_radius { 5 };
	const float default_damage { 40 };
	const float default_health { 500 };
	const double default_time_until_next_attack { 1 };
	const TankState default_state { TankState::FOLLOW };

	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( default_speed );
		radiuses.push_back ( default_radius );
		healths.push_back ( default_health );
		time_until_next_attacks.push_back ( default_time_until_next_attack );
		states.push_back ( default_state );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radiuses;
	std::vector<float> healths;
	std::vector<double> time_until_next_attacks;
	std::vector<TankState> states;
};

struct Leaders final
{
	const float default_speed { 100 };
	const float default_radius { 5 };
	const float default_health { 100 };
	const float default_damage { 20 };
	const double default_time_until_next_attack { 1 };
	const LeaderState default_state { LeaderState::SEARCH_TREASURE };

	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( default_speed );
		radiuses.push_back ( default_radius );
		healths.push_back ( default_health );
		time_until_next_attacks.push_back ( default_time_until_next_attack );
		states.push_back ( default_state );
		targets.push_back ( 0 );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radiuses;
	std::vector<float> healths;
	std::vector<double> time_until_next_attacks;
	std::vector<LeaderState> states;
	std::vector<std::size_t> targets;
};