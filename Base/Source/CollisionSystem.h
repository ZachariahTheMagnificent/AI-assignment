#pragma once
#include <vector>
#include <memory>
#include <float.h>
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
		std::vector<Vector3>::iterator position = positions.begin ( );
		std::vector<Vector3>::iterator direction = directions.begin ( );
		for ( std::vector<float>::iterator speed = speeds.begin ( ), end = speeds.end ( ); speed != end; ++position, ++direction, ++speed )
		{
			*position += *direction * *speed * delta_time;
		}
	}
};

struct CollisionSystem final
{
public:
	std::vector<bool> CheckCollision ( std::vector<Vector3>& positions, std::vector<float>& radius_squareds, std::vector<bool>& results )
	{
		const std::size_t size = positions.size ( );
		results.resize ( ( size * size + size ) / 2 );

		std::vector<Vector3>::iterator position1 = positions.begin ( );
		std::vector<bool>::iterator result = results.begin ( );
		for ( std::vector<float>::iterator radius_squared1 = radius_squareds.begin ( ), end = radius_squareds.end ( ); radius_squared1 != end; ++position1, ++radius_squared1 )
		{
			std::vector<Vector3>::iterator position2 = position1 + 1;
			for ( std::vector<float>::iterator radius_squared2 = radius_squared1 + 1; radius_squared2 != end; ++position2, ++radius_squared2, ++result )
			{
				*result = ( *position1 - *position2 ).LengthSquared ( ) <= *radius_squared1 + *radius_squared2;
			}
		}

		return results;
	}
	static std::vector<bool> CheckCollision ( std::vector<Vector3>& positions1, std::vector<float>& radius_squareds1, std::vector<Vector3>& positions2, std::vector<float>& radius_squareds2, std::vector<bool>& results )
	{
		results.resize ( positions1.size ( ) * positions2.size ( ) );

		const std::vector<Vector3>::iterator position_begin2 = positions2.begin ( );
		const std::vector<float>::iterator radius_squared_begin2 = radius_squareds2.begin ( );

		std::vector<Vector3>::iterator position1 = positions1.begin ( );
		std::vector<float>::iterator end2 = radius_squareds2.end ( );
		std::vector<bool>::iterator result = results.begin ( );
		for ( std::vector<float>::iterator radius_squared1 = radius_squareds1.begin ( ), end1 = radius_squareds1.end ( ); radius_squared1 != end1; ++position1, ++radius_squared1 )
		{
			std::vector<Vector3>::iterator position2 = position_begin2;
			for ( std::vector<float>::iterator radius_squared2 = radius_squared_begin2; radius_squared2 != end2; ++position2, ++radius_squared2, ++result )
			{
				*result = ( *position1 - *position2 ).LengthSquared ( ) <= *radius_squared1 + *radius_squared2;
			}
		}

		return results;
	}
};

struct PathNodeTravelInfo final
{
	void ConnectTo ( const std::vector<PathNodeTravelInfo>::const_iterator travel_info, const float new_distance_travelled_squared, const float new_total_travel_cost )
	{
		calculated = true;
		if ( new_total_travel_cost <= total_travel_cost )
		{
			distance_travelled_squared = new_distance_travelled_squared;
			total_travel_cost = new_total_travel_cost;
			previous_travel_info = travel_info;
		}
	}

	float distance_travelled_squared { FLT_MAX };
	float distance_to_goal_squared { FLT_MAX };
	float total_travel_cost { FLT_MAX };
	bool calculated { false };
	std::vector<PathNodeTravelInfo>::const_iterator previous_travel_info;
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
		new_total_travel_cost_cache_.push_back ( float { } );
	}

	void CalculateNeighbours ( std::vector<std::size_t>& calculated_unvisited_nodes_indices, const std::vector<PathNodeTravelInfo>::const_iterator travel_info_begin, const std::vector<PathNodeTravelInfo>::const_iterator self_travel_info, const Vector3 position ) const
	{
		const float distance_travelled_squared = self_travel_info->distance_travelled_squared;
		const std::size_t size = neighbour_positions_.size ( );

		const std::vector<float>::iterator new_distance_travelled_squared_cache_begin = new_distance_travelled_squared_cache_.begin ( );
		const std::vector<float>::iterator new_total_travel_cost_cache_begin = new_total_travel_cost_cache_.begin ( );

		std::vector<Vector3>::const_iterator neighbour_position = neighbour_positions_.begin ( );
		std::vector<float>::iterator new_distance_travelled_squared = new_distance_travelled_squared_cache_begin;
		for ( std::vector<float>::iterator new_total_travel_cost = new_total_travel_cost_cache_begin, end = new_total_travel_cost_cache_.end ( ); new_total_travel_cost != end; ++neighbour_position, ++new_distance_travelled_squared, ++new_total_travel_cost )
		{
			*new_total_travel_cost = *new_distance_travelled_squared = ( *neighbour_position - position ).LengthSquared ( ) + distance_travelled_squared;
		}

		new_distance_travelled_squared = new_distance_travelled_squared_cache_begin;
		std::vector<float>::iterator new_total_travel_cost = new_total_travel_cost_cache_begin;
		for ( const std::vector<PathNodeTravelInfo>::iterator travel_info : neighbour_travel_info_ )
		{
			*new_total_travel_cost += travel_info->distance_to_goal_squared;
			if ( !travel_info->calculated )
			{
				calculated_unvisited_nodes_indices.push_back ( travel_info - travel_info_begin );
			}

			travel_info->ConnectTo ( self_travel_info, *new_distance_travelled_squared, *new_total_travel_cost );

			++new_distance_travelled_squared;
			++new_total_travel_cost;
		}
	}

private:
	std::vector<Vector3> neighbour_positions_;
	std::vector<std::vector<PathNodeTravelInfo>::iterator> neighbour_travel_info_;

	mutable std::vector<float> new_distance_travelled_squared_cache_;
	mutable std::vector<float> new_total_travel_cost_cache_;
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
		
		const std::vector<PathNode>::iterator node_begin = path_nodes_.begin ( );
		const std::vector<PathNode>::iterator node_end = path_nodes_.end ( );
		const std::vector<Vector3>::iterator node_position_begin = node_positions_.begin ( );
		const std::vector<Vector3>::iterator node_position_end = node_positions_.end ( );

		std::vector<Vector3>::iterator node_position = node_position_begin;
		for ( const Vector3 position : positions )
		{
			*node_position = position;
			++node_position;
		}

		std::vector<PathNodeTravelInfo>::iterator travel_info1 = travel_info_cache_.begin ( );
		std::vector<Vector3>::iterator node_position1 = node_position_begin;
		for ( std::vector<PathNode>::iterator node1 = node_begin; node1 != node_end; ++node1 )
		{
			std::vector<PathNodeTravelInfo>::iterator travel_info2 = travel_info1 + 1;
			std::vector<Vector3>::iterator node_position2 = node_position1 + 1;
			for ( std::vector<PathNode>::iterator node2 = node1 + 1; node2 != node_end; ++node2 )
			{
				if ( Connected ( *node_position1, *node_position2 ) )
				{
					node1->AddNeighbour ( travel_info2, *node_position1 );
					node2->AddNeighbour ( travel_info1, *node_position2 );
				}
				++travel_info2;
			}
			++travel_info1;
		}
	}

	Vector3 FindNextNodePositionInShortestPath ( const Vector3 starting_position, const Vector3 goal )
	{
		if ( Connected ( starting_position, goal ) )
		{
			return goal;
		}

		const std::size_t num_nodes = node_positions_.size ( );
		travel_info_cache_.clear ( );
		travel_info_cache_.resize ( num_nodes );

		const std::vector<PathNode>::iterator node_begin = path_nodes_.begin ( );
		const std::vector<PathNode>::iterator node_end = path_nodes_.end ( );
		const std::vector<Vector3>::iterator node_position_begin = node_positions_.begin ( );
		const std::vector<Vector3>::iterator node_position_end = node_positions_.end ( );
		const std::vector<PathNodeTravelInfo>::iterator travel_info_begin = travel_info_cache_.begin ( );
		const std::vector<PathNodeTravelInfo>::iterator travel_info_end = travel_info_cache_.end ( );

		//Calculate distance to goal for all nodes
		std::vector<Vector3>::iterator node_position = node_position_begin;
		for ( std::vector<PathNodeTravelInfo>::iterator travel_info = travel_info_begin; travel_info != travel_info_end; ++travel_info, ++node_position )
		{
			const Vector3 node_to_goal = goal - *node_position;
			travel_info->distance_to_goal_squared = node_to_goal.LengthSquared ( );
		}

		//Find all nodes connected to starting position and add them to the processing array
		calculated_unvisited_nodes_index_cache_.clear ( );
		std::size_t node_index = 0;
		for ( node_position = node_position_begin; node_position != node_position_end; ++node_position )
		{
			if ( Connected ( starting_position, *node_position ) )
			{
				const Vector3 start_to_node = *node_position - starting_position;
				travel_info_cache_ [ node_index ].distance_travelled_squared = start_to_node.LengthSquared ( );
				travel_info_cache_ [ node_index ].total_travel_cost = travel_info_cache_ [ node_index ].distance_travelled_squared + travel_info_cache_ [ node_index ].distance_to_goal_squared;
				calculated_unvisited_nodes_index_cache_.push_back ( node_position - node_position_begin );
			}
			++node_index;
		}

		//loop until there is nothing else to search
		while ( calculated_unvisited_nodes_index_cache_.size ( ) != 0 )
		{
			//Get the lowest cost node to calculate neighbours and add any unvisited to the processing array. Then remove it.
			std::size_t index_of_node_to_process = calculated_unvisited_nodes_index_cache_.front ( );
			float total_travel_cost = travel_info_cache_ [ index_of_node_to_process ].total_travel_cost;
			std::vector<std::size_t>::iterator element_to_delete = calculated_unvisited_nodes_index_cache_.begin ( );
			for ( std::vector<std::size_t>::iterator index = element_to_delete + 1, end = calculated_unvisited_nodes_index_cache_.end ( ); index != end; ++index )
			{
				const std::size_t dereferenced_index = *index;
				const float new_total_travel_cost = travel_info_cache_ [ dereferenced_index ].total_travel_cost;
				if ( new_total_travel_cost < total_travel_cost )
				{
					index_of_node_to_process = dereferenced_index;
					total_travel_cost = new_total_travel_cost;
					element_to_delete = index;
				}
			}

			calculated_unvisited_nodes_index_cache_.erase ( element_to_delete );

			//break if node is connected to the goal
			const Vector3 position = node_positions_ [ index_of_node_to_process ];
			if ( Connected ( goal, position ) )
			{
				//backtrack to find next node
				std::vector<PathNodeTravelInfo>::const_iterator current = travel_info_begin + index_of_node_to_process;
				while ( current->calculated )
				{
					current = current->previous_travel_info;
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
	PERSUE,
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
	PERSUE,
	ATTACK,
	DEAD
};

struct Arrows final
{
	void Create ( const Vector3 position, const Vector3 direction )
	{
		positions.push_back ( position );
		directions.push_back ( direction );
		speeds.push_back ( 50 );
		radius_squareds.push_back ( 5 * 5 );
		deads.push_back ( false );
		time_until_deaths.push_back ( 5 );
	}
	void ClearDead ( )
	{
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radius_squareds;
	std::vector<bool> deads;
	std::vector<double> time_until_deaths;
};

struct Monsters final
{
	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( 50 );
		radius_squareds.push_back ( 5 * 5 );
		time_until_next_attacks.push_back ( 0 );
		states.push_back ( MonsterState::WAIT );
		aggro_radius_squareds.push_back ( 150 * 150 );
	}
	void ClearDead ( )
	{
	}
	void Clear ( )
	{
		positions.clear ( );
		directions.clear ( );
		speeds.clear ( );
		radius_squareds.clear ( );
		time_until_next_attacks.clear ( );
		states.clear ( );
		aggro_radius_squareds.clear ( );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radius_squareds;
	std::vector<double> time_until_next_attacks;
	std::vector<MonsterState> states;
	std::vector<float> aggro_radius_squareds;
};

struct Healers final
{
	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( 50 );
		radius_squareds.push_back ( 5 * 5 );
		health.push_back ( 100 );
		time_until_next_arrows.push_back ( 0 );
		states.push_back ( HealerState::FOLLOW );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radius_squareds;
	std::vector<float> health;
	std::vector<double> time_until_next_arrows;
	std::vector<HealerState> states;
};

struct Archers final
{
	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( 50 );
		radius_squareds.push_back ( 5 * 5 );
		health.push_back ( 100 );
		time_until_next_arrows.push_back ( 0 );
		states.push_back ( ArcherState::FOLLOW );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radius_squareds;
	std::vector<float> health;
	std::vector<double> time_until_next_arrows;
	std::vector<ArcherState> states;
};

struct Leaders final
{
	void Create ( const Vector3 position )
	{
		positions.push_back ( position );
		directions.push_back ( Vector3 { } );
		speeds.push_back ( 100 );
		radius_squareds.push_back ( 5 * 5 );
		states.push_back ( LeaderState::SEARCH_TREASURE );
	}

	std::vector<Vector3> positions;
	std::vector<Vector3> directions;
	std::vector<float> speeds;
	std::vector<float> radius_squareds;
	std::vector<LeaderState> states;
};

//class ArrowCollaborator final : public Collaborator
//{
//public:
//	ArrowCollaborator ( TimerSystem& timer_system, PhysicsSystem& physics_system, CollisionSystem& collision_system, const Vector3 position, const Vector3 direction ) : timer_system_ ( timer_system ), physics_system_ ( physics_system ), collision_system_ ( collision_system )
//	{
//		arrow_.position = position;
//		arrow_.direction = direction;
//		arrow_.speed = 50;
//		arrow_.radius_squared = 5 * 5;
//		arrow_.dead = false;
//		arrow_.time_until_death = 5;
//	}
//
//	Arrow& GetArrow ( )
//	{
//		return arrow_;
//	}
//	Arrow GetArrow ( ) const
//	{
//		return arrow_;
//	}
//
//	void Update ( ) override
//	{
//		timer_system_.AddCountdown ( arrow_.time_until_death, this );
//		if ( !arrow_.dead )
//		{
//			physics_system_.AddBody ( PhysicsBody { arrow_.position, arrow_.direction, arrow_.speed }, this );
//		}
//	}
//
//	void OnCountdown ( const double countdown ) override
//	{
//		arrow_.time_until_death = countdown;
//		if ( arrow_.time_until_death <= 0 )
//		{
//			arrow_.dead = true;
//		}
//	}
//
//	void OnChangePosition ( const Vector3 new_position ) override
//	{
//		arrow_.position = new_position;
//
//		collision_system_.AddBody ( CollisionBody { arrow_.position, arrow_.radius_squared }, this );
//	}
//
//	void OnCollision ( const CollisionBody self, const CollisionBody other, Collaborator& collision ) override
//	{
//		arrow_.position = self.position;
//		arrow_.radius_squared = self.radius_squared;
//		
//		collision.OnCollision ( other, arrow_ );
//	}
//	void OnCollision ( const CollisionBody self, Monster& monster ) override
//	{
//		monster.dead = true;
//		arrow_.dead = true;
//	}
//
//private:
//	TimerSystem& timer_system_;
//	PhysicsSystem& physics_system_;
//	CollisionSystem& collision_system_;
//	Arrow arrow_;
//};
//
//class MonsterCollaborator final : public Collaborator
//{
//public:
//	MonsterCollaborator ( PhysicsSystem& physics_system, CollisionSystem& collision_system, const Vector3 position ) : physics_system_ ( physics_system ), collision_system_ ( collision_system )
//	{
//		monster_.position = position;
//		monster_.speed = 50;
//		monster_.radius_squared = 5 * 5;
//		monster_.dead = false;
//	}
//
//	Monster GetMonster ( ) const
//	{
//		return monster_;
//	}
//
//	void Update ( ) override
//	{
//		if ( !monster_.dead )
//		{
//			physics_system_.AddBody ( PhysicsBody { monster_.position, monster_.direction, monster_.speed }, this );
//		}
//	}
//
//	void OnChangePosition ( const Vector3 new_position ) override
//	{
//		monster_.position = new_position;
//
//		collision_system_.AddBody ( CollisionBody { monster_.position, monster_.radius_squared }, this );
//	}
//
//	void OnCollision ( const CollisionBody self, const CollisionBody other, Collaborator& collision ) override
//	{
//		monster_.position = self.position;
//		monster_.radius_squared = self.radius_squared;
//
//		collision.OnCollision ( other, monster_ );
//	}
//	void OnCollision ( const CollisionBody self, Arrow& arrow ) override
//	{
//		monster_.dead = true;
//		arrow.dead = true;
//	}
//
//private:
//	PhysicsSystem& physics_system_;
//	CollisionSystem& collision_system_;
//	Monster monster_;
//};
//
//class ArcherCollaborator final : public Collaborator
//{
//public:
//	ArcherCollaborator ( TimerSystem& timer_system, PhysicsSystem& physics_system, CollisionSystem& collision_system, ArrowSpawningSystem& arrow_spawning_system, const Vector3 position ) : timer_system_ ( timer_system ), physics_system_ ( physics_system ), collision_system_ ( collision_system ), arrow_spawning_system_ ( arrow_spawning_system )
//	{
//		archer_.position = position;
//		archer_.speed = 50;
//		archer_.radius_squared = 5 * 5;
//		archer_.dead = false;
//		archer_.time_until_next_arrow = 0;
//	}
//
//	Archer GetArcher ( ) const
//	{
//		return archer_;
//	}
//
//	void Update ( ) override
//	{
//		if ( !archer_.dead )
//		{
//			if ( archer_.time_until_next_arrow > 0 )
//			{
//				timer_system_.AddCountdown ( archer_.time_until_next_arrow, this );
//			}
//			if ( archer_.state == ArcherState::Move )
//			{
//				physics_system_.AddBody ( PhysicsBody { archer_.position, archer_.direction, archer_.speed }, this );
//			}
//		}
//	}
//
//	void OnChangePosition ( const Vector3 new_position ) override
//	{
//		archer_.position = new_position;
//
//		collision_system_.AddBody ( CollisionBody { archer_.position, archer_.radius_squared }, this );
//	}
//
//	void OnCollision ( const CollisionBody self, const CollisionBody other, Collaborator& collision ) override
//	{
//		archer_.position = self.position;
//		archer_.radius_squared = self.radius_squared;
//
//		collision.OnCollision ( other, archer_ );
//	}
//	virtual void OnCollision ( const CollisionBody self, Monster& monster )
//	{
//		archer_.dead = true;
//	}
//	virtual void OnCollision ( const CollisionBody self, MonsterAggroRadius& monster_radius )
//	{
//		archer_.state = ArcherState::Attack;
//		if ( archer_.time_until_next_arrow <= 0 )
//		{
//			arrow_spawning_system_.AddSpawn ( timer_system_, physics_system_, collision_system_, archer_.position, ( monster_radius.position - archer_.position ).Normalized ( ) );
//			archer_.time_until_next_arrow = 3;
//		}
//	}
//
//private:
//	TimerSystem& timer_system_;
//	PhysicsSystem& physics_system_;
//	CollisionSystem& collision_system_;
//	ArrowSpawningSystem& arrow_spawning_system_;
//	Archer archer_;
//};