#include "SceneKinematics.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <vector>
#include "Vector3.h"
#include <sstream>

SceneKinematics::SceneKinematics()
:
m_worldHeight(300.f),
m_worldWidth(400.f)
{
}

SceneKinematics::~SceneKinematics()
{
}


void SceneKinematics::Init ( )
{
	// Black background
	glClearColor ( 0.0f, 0.0f, 0.4f, 0.0f );
	// Enable depth test
	glEnable ( GL_DEPTH_TEST );
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc ( GL_LESS );

	glEnable ( GL_CULL_FACE );

	glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glGenVertexArrays ( 1, &m_vertexArrayID );
	glBindVertexArray ( m_vertexArrayID );

	m_programID = LoadShaders ( "Shader//comg.vertexshader", "Shader//comg.fragmentshader" );

	// Get a handle for our uniform
	m_parameters [ U_MVP ] = glGetUniformLocation ( m_programID, "MVP" );
	//m_parameters[U_MODEL] = glGetUniformLocation(m_programID, "M");
	//m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters [ U_MODELVIEW ] = glGetUniformLocation ( m_programID, "MV" );
	m_parameters [ U_MODELVIEW_INVERSE_TRANSPOSE ] = glGetUniformLocation ( m_programID, "MV_inverse_transpose" );
	m_parameters [ U_MATERIAL_AMBIENT ] = glGetUniformLocation ( m_programID, "material.kAmbient" );
	m_parameters [ U_MATERIAL_DIFFUSE ] = glGetUniformLocation ( m_programID, "material.kDiffuse" );
	m_parameters [ U_MATERIAL_SPECULAR ] = glGetUniformLocation ( m_programID, "material.kSpecular" );
	m_parameters [ U_MATERIAL_SHININESS ] = glGetUniformLocation ( m_programID, "material.kShininess" );
	m_parameters [ U_LIGHTENABLED ] = glGetUniformLocation ( m_programID, "lightEnabled" );
	m_parameters [ U_NUMLIGHTS ] = glGetUniformLocation ( m_programID, "numLights" );
	m_parameters [ U_LIGHT0_TYPE ] = glGetUniformLocation ( m_programID, "lights[0].type" );
	m_parameters [ U_LIGHT0_POSITION ] = glGetUniformLocation ( m_programID, "lights[0].position_cameraspace" );
	m_parameters [ U_LIGHT0_COLOR ] = glGetUniformLocation ( m_programID, "lights[0].color" );
	m_parameters [ U_LIGHT0_POWER ] = glGetUniformLocation ( m_programID, "lights[0].power" );
	m_parameters [ U_LIGHT0_KC ] = glGetUniformLocation ( m_programID, "lights[0].kC" );
	m_parameters [ U_LIGHT0_KL ] = glGetUniformLocation ( m_programID, "lights[0].kL" );
	m_parameters [ U_LIGHT0_KQ ] = glGetUniformLocation ( m_programID, "lights[0].kQ" );
	m_parameters [ U_LIGHT0_SPOTDIRECTION ] = glGetUniformLocation ( m_programID, "lights[0].spotDirection" );
	m_parameters [ U_LIGHT0_COSCUTOFF ] = glGetUniformLocation ( m_programID, "lights[0].cosCutoff" );
	m_parameters [ U_LIGHT0_COSINNER ] = glGetUniformLocation ( m_programID, "lights[0].cosInner" );
	m_parameters [ U_LIGHT0_EXPONENT ] = glGetUniformLocation ( m_programID, "lights[0].exponent" );
	// Get a handle for our "colorTexture" uniform
	m_parameters [ U_COLOR_TEXTURE_ENABLED ] = glGetUniformLocation ( m_programID, "colorTextureEnabled" );
	m_parameters [ U_COLOR_TEXTURE ] = glGetUniformLocation ( m_programID, "colorTexture" );
	// Get a handle for our "textColor" uniform
	m_parameters [ U_TEXT_ENABLED ] = glGetUniformLocation ( m_programID, "textEnabled" );
	m_parameters [ U_TEXT_COLOR ] = glGetUniformLocation ( m_programID, "textColor" );

	// Use our shader
	glUseProgram ( m_programID );

	lights [ 0 ].type = Light::LIGHT_DIRECTIONAL;
	lights [ 0 ].position.Set ( 0, 20, 0 );
	lights [ 0 ].color.Set ( 1, 1, 1 );
	lights [ 0 ].power = 1;
	lights [ 0 ].kC = 1.f;
	lights [ 0 ].kL = 0.01f;
	lights [ 0 ].kQ = 0.001f;
	lights [ 0 ].cosCutoff = cos ( Math::DegreeToRadian ( 45 ) );
	lights [ 0 ].cosInner = cos ( Math::DegreeToRadian ( 30 ) );
	lights [ 0 ].exponent = 3.f;
	lights [ 0 ].spotDirection.Set ( 0.f, 1.f, 0.f );

	glUniform1i ( m_parameters [ U_NUMLIGHTS ], 0 );
	glUniform1i ( m_parameters [ U_TEXT_ENABLED ], 0 );

	glUniform1i ( m_parameters [ U_LIGHT0_TYPE ], lights [ 0 ].type );
	glUniform3fv ( m_parameters [ U_LIGHT0_COLOR ], 1, &lights [ 0 ].color.r );
	glUniform1f ( m_parameters [ U_LIGHT0_POWER ], lights [ 0 ].power );
	glUniform1f ( m_parameters [ U_LIGHT0_KC ], lights [ 0 ].kC );
	glUniform1f ( m_parameters [ U_LIGHT0_KL ], lights [ 0 ].kL );
	glUniform1f ( m_parameters [ U_LIGHT0_KQ ], lights [ 0 ].kQ );
	glUniform1f ( m_parameters [ U_LIGHT0_COSCUTOFF ], lights [ 0 ].cosCutoff );
	glUniform1f ( m_parameters [ U_LIGHT0_COSINNER ], lights [ 0 ].cosInner );
	glUniform1f ( m_parameters [ U_LIGHT0_EXPONENT ], lights [ 0 ].exponent );

	camera.Init ( Vector3 ( 0, 0, 1 ), Vector3 ( 0, 0, 0 ), Vector3 ( 0, 1, 0 ) );

	for ( int i = 0; i < NUM_GEOMETRY; ++i )
	{
		meshList [ i ] = NULL;
	}
	meshList [ GEO_AXES ] = MeshBuilder::GenerateAxes ( "reference", 1000, 1000, 1000 );
	meshList [ GEO_BALL ] = MeshBuilder::GenerateSphere ( "ball", Color ( 1, 1, 1 ), 10, 10, 1.f );
	meshList [ GEO_CUBE ] = MeshBuilder::GenerateCube ( "cube", Color ( 1, 1, 1 ), 2.f );
	meshList [ GEO_MOTHERBASE ] = MeshBuilder::GenerateQuad ( "motherbase", Color ( 0, 1, 0 ), 2.f );
	meshList [ GEO_MOTHERBASE ]->textureID = LoadTGA ( "Image//base.tga" );
	meshList [ GEO_ARROW ] = MeshBuilder::GenerateSphere ( "arrow", Color ( 1, 1, 1 ), 10, 10, 1.f );
	//meshList[GEO_ARROW]->textureID = LoadTGA("Image//arrow.tga");
	meshList [ GEO_ARCHER ] = MeshBuilder::GenerateQuad ( "archer", Color ( 1, 0, 1 ), 2.f );
	meshList [ GEO_ARCHER ]->textureID = LoadTGA ( "Image//Archers.tga" );
	meshList [ GEO_HEALER ] = MeshBuilder::GenerateQuad ( "healer", Color ( 1, 0, 1 ), 2.f );
	meshList [ GEO_HEALER ]->textureID = LoadTGA ( "Image//healer.tga" );
	meshList [ GEO_UNRECRUITED_ARCHER ] = MeshBuilder::GenerateQuad ( "unrecruited archer", Color ( 1, 1, 0 ), 2.f );
	meshList [ GEO_UNRECRUITED_ARCHER ]->textureID = LoadTGA ( "Image//peasant.tga" );
	meshList [ GEO_WORKER ] = MeshBuilder::GenerateQuad ( "worker", Color ( 1, 0, 0 ), 2.f );
	meshList [ GEO_WORKER ]->textureID = LoadTGA ( "Image//Engineer.tga" );
	meshList [ GEO_RABBIT ] = MeshBuilder::GenerateQuad ( "rabbit", Color ( 1, 1, 1 ), 2.f );
	meshList [ GEO_RABBIT ]->textureID = LoadTGA ( "Image//Rabbit.tga" );
	meshList [ GEO_DEAD_RABBIT ] = MeshBuilder::GenerateQuad ( "dead rabbit", Color ( 0, 0, 0 ), 2.f );
	meshList [ GEO_MONSTER ] = MeshBuilder::GenerateQuad ( "monster", Color ( 1, 1, 1 ), 2.f );
	meshList [ GEO_MONSTER ]->textureID = LoadTGA ( "Image//zombies.tga" );
	meshList [ GEO_TREASURE ] = MeshBuilder::GenerateQuad ( "treasure", Color ( 1, 1, 1 ), 2.f );
	meshList [ GEO_TREASURE ]->textureID = LoadTGA ( "Image//treasure.tga" );
	meshList [ GEO_LEADER ] = MeshBuilder::GenerateQuad ( "leader", Color ( 1, 1, 1 ), 2.f );
	meshList [ GEO_LEADER ]->textureID = LoadTGA ( "Image//leader.tga" );
	meshList [ GEO_WALL ] = MeshBuilder::GenerateQuad ( "wall", Color ( 0, 1, 0 ), 2.f );
	meshList [ GEO_TEXT ] = MeshBuilder::GenerateText ( "text", 16, 16 );
	meshList [ GEO_TEXT ]->textureID = LoadTGA ( "Image//calibri.tga" );
	meshList [ GEO_TEXT ]->material.kAmbient.Set ( 1, 0, 0 );

	bLightEnabled = false;

	//Physics code here
	m_speed = 1.f;

	m_gravity.Set ( 0, -9.8f, 0 ); //init gravity as 9.8ms-2 downwards
	Math::InitRNG ( );

	m_ghost = new GameObject ( GameObject::GO_BALL );
	//Exercise 1: construct 10 GameObject with type GO_BALL and add into m_goList

	//underwatch.GetArcher ( ).pos = Vector3 ( 250, 50, 0 );
	//underwatch.GetHealer ( ).pos = Vector3 ( 280, 45, 0 );

	archers.Create ( Vector3 ( 100, 50, 0 ) );
	leaders.Create ( Vector3 ( 250, 50, 0 ) );
	healers.Create ( Vector3 ( 50, 50, 0 ) );
	tanks.Create ( Vector3 ( 25, 50, 0 ) );

	std::vector<Vector3> node_positions;
	node_positions.push_back ( Vector3 { 200, 300, 0 } );
	node_positions.push_back ( Vector3 { 200, 10, 0 } );

	std::vector<Line> line_boundaries;
	line_boundaries.push_back ( Line { Vector3 { 200, 70, 0 }, Vector3 { 0, -40, 0 } } );

	//path_finding_system.Build ( node_positions, line_boundaries );

	treasure.resize ( 1 );

	RandomiseTreasureLocation ( );
}

void SceneKinematics::Update ( double dt )
{
	//Keyboard Section
	if ( Application::IsKeyPressed ( '1' ) )
		glEnable ( GL_CULL_FACE );
	if ( Application::IsKeyPressed ( '2' ) )
		glDisable ( GL_CULL_FACE );
	if ( Application::IsKeyPressed ( '3' ) )
		glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
	if ( Application::IsKeyPressed ( '4' ) )
		glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

	if ( Application::IsKeyPressed ( '+' ) )
	{
		//Exercise 6: adjust simulation speed
	}
	if ( Application::IsKeyPressed ( '-' ) )
	{
		//Exercise 6: adjust simulation speed
	}
	if ( Application::IsKeyPressed ( 'c' ) )
	{
		//Exercise 9: clear screen
	}
	if ( Application::IsKeyPressed ( ' ' ) )
	{
		//underwatch.damageArcher(1);
	}
	if ( Application::IsKeyPressed ( 'V' ) )
	{
		//underwatch.damageHealer(1);
	}

	//Mouse Section
	static bool bLButtonState = false;
	//Exercise 10: ghost code here
	if ( !bLButtonState && Application::IsMousePressed ( 0 ) )
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;

		double x, y;
		Application::GetCursorPos ( &x, &y );
		int w = Application::GetWindowWidth ( );
		int h = Application::GetWindowHeight ( );

		//Exercise 10: spawn ghost ball
	}
	else if ( bLButtonState && !Application::IsMousePressed ( 0 ) )
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		//Exercise 4: spawn ball

		//Exercise 10: replace Exercise 4 code and use ghost to determine ball velocity

		//Exercise 11: kinematics equation
		//v = u + a * t
		//t = (v - u ) / a

		//v * v = u * u + 2 * a * s
		//s = - (u * u) / (2 * a)

		//s = u * t + 0.5 * a * t * t
		//(0.5 * a) * t * t + (u) * t + (-s) = 0
	}

	static bool bRButtonState = false;
	if ( !bRButtonState && Application::IsMousePressed ( 1 ) )
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
		//Exercise 7: spawn obstacles using GO_CUBE
	}
	else if ( bRButtonState && !Application::IsMousePressed ( 1 ) )
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}

	//Physics Simulation Section
	fps = ( float ) ( 1.f / dt );

	//Exercise 11: update kinematics information
	for ( std::vector<GameObject *>::iterator it = m_goList.begin ( ); it != m_goList.end ( ); ++it )
	{
		GameObject *go = ( GameObject * ) *it;
		if ( go->active )
		{
			if ( go->type == GameObject::GO_BALL )
			{
				//Exercise 2: implement equation 1 & 2

				//Exercise 12: replace Exercise 2 code and use average speed instead
			}

			//Exercise 8: check collision with GO_CUBE

			//Exercise 5: unspawn ball when outside window
		}
	}

	physics_system.Update ( arrows.positions, arrows.directions, arrows.speeds, dt );
	physics_system.Update ( monsters.positions, monsters.directions, monsters.speeds, dt );
	physics_system.Update ( archers.positions, archers.directions, archers.speeds, dt );
	physics_system.Update ( leaders.positions, leaders.directions, leaders.speeds, dt );
	physics_system.Update ( healers.positions, healers.directions, healers.speeds, dt );
	physics_system.Update ( tanks.positions, tanks.directions, tanks.speeds, dt );

	for ( std::size_t index = 0, size = arrows.positions.size ( ); index < size; ++index )
	{
		if ( !arrows.deads [ index ] )
		{
			arrows.time_until_deaths [ index ] -= dt;
		}
	}

	for ( std::size_t index = 0, size = monsters.positions.size ( ); index < size; ++index )
	{
		monsters.directions [ index ] = Vector3 { };

		switch ( monsters.states [ index ] )
		{
			case MonsterState::ATTACK:
			{
				monsters.time_until_next_attacks [ index ] -= dt;
				if ( monsters.TargetIsDead [ index ] ( ) )
				{
					monsters.states [ index ] = MonsterState::WAIT;
				}
				else if ( monsters.TargetInRange [ index ] ( ) )
				{
					if ( monsters.time_until_next_attacks [ index ] <= 0 )
					{
						monsters.DamageTarget [ index ] ( monsters.default_damage );

						monsters.time_until_next_attacks [ index ] = monsters.default_time_until_next_attack;
					}
				}
				else
				{
					const Vector3 next_destination = path_finding_system.FindNextNodePositionInShortestPath ( monsters.positions [ index ], monsters.GetTargetPosition [ index ] ( ) );
					monsters.directions [ index ] = ( next_destination - monsters.positions [ index ] ).Normalized ( );
				}
			}
		}
	}

	for ( std::size_t index = 0, size = archers.positions.size ( ); index < size; ++index )
	{
		archers.directions [ index ] = Vector3 { };

		switch ( archers.states [ index ] )
		{
			case ArcherState::FOLLOW:
			{
				if ( leaders.states.front ( ) != LeaderState::DEAD )
				{
					const Vector3 next_destination = path_finding_system.FindNextNodePositionInShortestPath ( archers.positions [ index ], leaders.positions.front ( ) );
					archers.directions [ index ] = ( next_destination - archers.positions [ index ] ).Normalized ( );
				}
				break;
			}
			case ArcherState::ATTACK:
			{
				archers.time_until_next_arrows [ index ] -= dt;
				if ( archers.time_until_next_arrows [ index ] <= 0 && monsters.states [ leaders.targets.front ( ) ] != MonsterState::DEAD )
				{
					arrows.Create ( archers.positions [ index ], ( monsters.positions [ leaders.targets.front ( ) ] - archers.positions [ index ] ).Normalized ( ) );
					archers.time_until_next_arrows [ index ] = archers.default_time_until_next_arrow;
				}
				break;
			}
			case ArcherState::SPEED_FIRE:
			{
				monsters.directions [ index ] = Vector3 { };
				if ( monsters.states [ leaders.targets.front ( ) ] != MonsterState::DEAD )
				{
					arrows.Create ( archers.positions [ index ], ( monsters.positions [ leaders.targets.front ( ) ] - archers.positions [ index ] ).Normalized ( ) );
				}
				break;
			}
		}
	}

	for ( std::size_t index = 0, size = healers.positions.size ( ); index < size; ++index )
	{
		healers.directions [ index ] = Vector3 { };

		switch ( healers.states [ index ] )
		{
			case HealerState::FOLLOW:
			{
				if ( leaders.states.front ( ) != LeaderState::DEAD )
				{
					const Vector3 next_destination = path_finding_system.FindNextNodePositionInShortestPath ( healers.positions [ index ], leaders.positions.front ( ) );
					healers.directions [ index ] = ( next_destination - healers.positions [ index ] ).Normalized ( );
					break;
				}
			}
			case HealerState::CLOAK:
			{
				break;
			}
			case HealerState::HEAL:
			{
				break;
			}
		}
	}

	for ( std::size_t index = 0, size = leaders.positions.size ( ); index < size; ++index )
	{
		leaders.directions [ index ] = Vector3 { };

		switch ( leaders.states [ index ] )
		{
			case LeaderState::SEARCH_TREASURE:
			{
				const Vector3 next_destination = path_finding_system.FindNextNodePositionInShortestPath ( leaders.positions [ index ], treasure.front ( ) );
				leaders.directions [ index ] = ( next_destination - leaders.positions [ index ] ).Normalized ( );
				break;
			}
			case LeaderState::ATTACK:
			{
				leaders.time_until_next_attacks [ index ] -= dt;

				const float total_radius = monsters.radiuses [ leaders.targets [ index ] ] + leaders.radiuses [ index ];
				const float total_radius_squared = total_radius * total_radius;

				if ( monsters.states [ leaders.targets.front ( ) ] == MonsterState::DEAD )
				{
					if ( archers.states [ index ] != ArcherState::DEAD )
					{
						archers.states [ index ] = ArcherState::FOLLOW;
					}
					if ( archers.states [ index ] != ArcherState::DEAD )
					{
						healers.states [ index ] = HealerState::FOLLOW;
					}
					if ( archers.states [ index ] != ArcherState::DEAD )
					{
						tanks.states [ index ] = TankState::FOLLOW;
					}
					if ( archers.states [ index ] != ArcherState::DEAD )
					{
						leaders.states [ index ] = LeaderState::SEARCH_TREASURE;
					}
				}
				else if ( ( monsters.positions [ leaders.targets [ index ] ] - leaders.positions [ index ] ).LengthSquared ( ) <= total_radius_squared )
				{
					if ( leaders.time_until_next_attacks [ index ] <= 0 )
					{
						monsters.GetDamage ( leaders.targets [ index ], leaders.default_damage );
						leaders.time_until_next_attacks [ index ] = leaders.default_time_until_next_attack;
					}
				}
				else
				{
					const Vector3 next_destination = path_finding_system.FindNextNodePositionInShortestPath ( leaders.positions [ index ], monsters.positions [ leaders.targets [ index ] ] );
					leaders.directions [ index ] = ( next_destination - leaders.positions [ index ] ).Normalized ( );
				}
				break;
			}
		}
	}

	for ( std::size_t index = 0, size = tanks.positions.size ( ); index < size; ++index )
	{
		tanks.directions [ index ] = Vector3 { };

		switch ( tanks.states [ index ] )
		{
			case TankState::FOLLOW:
			{
				const Vector3 next_destination = path_finding_system.FindNextNodePositionInShortestPath ( tanks.positions [ index ], leaders.positions.front ( ) );
				tanks.directions [ index ] = ( next_destination - tanks.positions [ index ] ).Normalized ( );
				break;
			}
			case TankState::ATTACK:
			{
				tanks.time_until_next_attacks [ index ] -= dt;

				const float total_radius = monsters.radiuses [ leaders.targets [ index ] ] + tanks.radiuses [ index ];
				const float total_radius_squared = total_radius * total_radius;

				if ( monsters.states [ leaders.targets.front ( ) ] != MonsterState::DEAD )
				{
					if ( ( monsters.positions [ leaders.targets.front ( ) ] - tanks.positions [ index ] ).LengthSquared ( ) <= total_radius_squared )
					{
						if ( tanks.time_until_next_attacks [ index ] <= 0 )
						{
							monsters.GetDamage ( leaders.targets.front ( ), tanks.default_damage );
							tanks.time_until_next_attacks [ index ] = tanks.default_time_until_next_attack;
						}
					}
					else
					{
						const Vector3 next_destination = path_finding_system.FindNextNodePositionInShortestPath ( tanks.positions [ index ], monsters.positions [ leaders.targets.front ( ) ] );
						tanks.directions [ index ] = ( next_destination - tanks.positions [ index ] ).Normalized ( );
					}
				}
				break;
			}
		}
	}

	collision_system.CheckCollision ( archers.positions, archers.radiuses, monsters.positions, monsters.aggro_radius_squareds,
	[ this ] ( const std::size_t archer_index, const std::size_t monster_index, const float distance_squared )
	{
		if ( monsters.states [ monster_index ] != MonsterState::DEAD )
		{
			bool new_target_is_closer = false;
			if ( monsters.states [ monster_index ] == MonsterState::ATTACK )
			{
				if ( distance_squared <= ( monsters.GetTargetPosition [ monster_index ] ( ) - monsters.positions [ monster_index ] ).LengthSquared ( ) )
				{
					new_target_is_closer = true;
				}
			}
			if ( ( monsters.states [ monster_index ] == MonsterState::WAIT || new_target_is_closer ) && archers.states [ archer_index ] != ArcherState::DEAD )
			{
				monsters.states [ monster_index ] = MonsterState::ATTACK;
				monsters.TargetIsDead [ monster_index ] = [ = ]
				{
					return archers.states [ archer_index ] == ArcherState::DEAD;
				};
				monsters.TargetInRange [ monster_index ] = [ = ]
				{
					const float total_radius = monsters.radiuses [ monster_index ] + archers.radiuses [ archer_index ];
					const float total_radius_squared = total_radius * total_radius;
					return ( monsters.positions [ monster_index ] - archers.positions [ archer_index ] ).LengthSquared ( ) <= total_radius_squared;
				};
				monsters.GetTargetPosition [ monster_index ] = [ = ]
				{
					return archers.positions [ archer_index ];
				};
				monsters.DamageTarget [ monster_index ] = [ = ] ( const float damage )
				{
					archers.healths [ archer_index ] -= damage;
					if ( archers.healths [ archer_index ] <= 0 )
					{
						archers.states [ archer_index ] = ArcherState::DEAD;
					}
				};
			}
			if ( ( leaders.states.front ( ) == LeaderState::SEARCH_TREASURE || leaders.states.front ( ) == LeaderState::TAKE_TREASURE ) )
			{
				leaders.targets.front ( ) = monster_index;
				leaders.states.front ( ) = LeaderState::ATTACK;
				archers.states.front ( ) = ArcherState::ATTACK;
				tanks.states.front ( ) = TankState::ATTACK;
				healers.states.front ( ) = HealerState::CLOAK;
			}
		}
	}
	);

	collision_system.CheckCollision ( leaders.positions, leaders.radiuses, monsters.positions, monsters.aggro_radius_squareds,
	[ this ] ( const std::size_t leader_index, const std::size_t monster_index, const float distance_squared )
	{
		if ( monsters.states [ monster_index ] != MonsterState::DEAD )
		{
			bool new_target_is_closer = false;
			if ( monsters.states [ monster_index ] == MonsterState::ATTACK )
			{
				if ( distance_squared <= ( monsters.GetTargetPosition [ monster_index ] ( ) - monsters.positions [ monster_index ] ).LengthSquared ( ) )
				{
					new_target_is_closer = true;
				}
			}
			if ( ( monsters.states [ monster_index ] == MonsterState::WAIT || new_target_is_closer ) && leaders.states [ leader_index ] != LeaderState::DEAD )
			{
				monsters.states [ monster_index ] = MonsterState::ATTACK;
				monsters.TargetIsDead [ monster_index ] = [ = ]
				{
					return leaders.states [ leader_index ] == LeaderState::DEAD;
				};
				monsters.TargetInRange [ monster_index ] = [ = ]
				{
					const float total_radius = monsters.radiuses [ monster_index ] + leaders.radiuses [ leader_index ];
					const float total_radius_squared = total_radius * total_radius;
					return ( monsters.positions [ monster_index ] - leaders.positions [ leader_index ] ).LengthSquared ( ) <= total_radius_squared;
				};
				monsters.GetTargetPosition [ monster_index ] = [ = ]
				{
					return leaders.positions [ leader_index ];
				};
				monsters.DamageTarget [ monster_index ] = [ = ] ( const float damage )
				{
					leaders.healths [ leader_index ] -= damage;
					if ( leaders.healths [ leader_index ] <= 0 )
					{
						leaders.states [ leader_index ] = LeaderState::DEAD;
					}
				};
			}
			if ( ( leaders.states.front ( ) == LeaderState::SEARCH_TREASURE || leaders.states.front ( ) == LeaderState::TAKE_TREASURE ) )
			{
				leaders.targets.front ( ) = monster_index;
				leaders.states.front ( ) = LeaderState::ATTACK;
				archers.states.front ( ) = ArcherState::ATTACK;
				tanks.states.front ( ) = TankState::ATTACK;
				healers.states.front ( ) = HealerState::CLOAK;
			}
		}
	}
	);

	collision_system.CheckCollision ( healers.positions, healers.radiuses, monsters.positions, monsters.aggro_radius_squareds,
	[ this ] ( const std::size_t healer_index, const std::size_t monster_index, const float distance_squared )
	{
		if ( monsters.states [ monster_index ] != MonsterState::DEAD )
		{
			bool new_target_is_closer = false;
			if ( monsters.states [ monster_index ] == MonsterState::ATTACK )
			{
				if ( distance_squared <= ( monsters.GetTargetPosition [ monster_index ] ( ) - monsters.positions [ monster_index ] ).LengthSquared ( ) )
				{
					new_target_is_closer = true;
				}
			}
			if ( (monsters.states [ monster_index ] == MonsterState::WAIT || new_target_is_closer) && healers.states[ healer_index] != HealerState::DEAD )
			{
				monsters.states [ monster_index ] = MonsterState::ATTACK;
				monsters.TargetIsDead [ monster_index ] = [ = ]
				{
					return healers.states [ healer_index ] == HealerState::DEAD;
				};
				monsters.TargetInRange [ monster_index ] = [ = ]
				{
					const float total_radius = monsters.radiuses [ monster_index ] + healers.radiuses [ healer_index ];
					const float total_radius_squared = total_radius * total_radius;
					return ( monsters.positions [ monster_index ] - healers.positions [ healer_index ] ).LengthSquared ( ) <= total_radius_squared;
				};
				monsters.GetTargetPosition [ monster_index ] = [ = ]
				{
					return healers.positions [ healer_index ];
				};
				monsters.DamageTarget [ monster_index ] = [ = ] ( const float damage )
				{
					healers.healths [ healer_index ] -= damage;
					if ( healers.healths [ healer_index ] <= 0 )
					{
						healers.states [ healer_index ] = HealerState::DEAD;
					}
				};
			}
			if ( ( leaders.states.front ( ) == LeaderState::SEARCH_TREASURE || leaders.states.front ( ) == LeaderState::TAKE_TREASURE ) )
			{
				leaders.targets.front ( ) = monster_index;
				leaders.states.front ( ) = LeaderState::ATTACK;
				archers.states.front ( ) = ArcherState::ATTACK;
				tanks.states.front ( ) = TankState::ATTACK;
				healers.states.front ( ) = HealerState::CLOAK;
			}
		}
	}
	);

	collision_system.CheckCollision ( tanks.positions, tanks.radiuses, monsters.positions, monsters.aggro_radius_squareds,
	[ this ] ( const std::size_t tank_index, const std::size_t monster_index, const float distance_squared )
	{
		if ( monsters.states [ monster_index ] != MonsterState::DEAD )
		{
			bool new_target_is_closer = false;
			if ( monsters.states [ monster_index ] == MonsterState::ATTACK )
			{
				if ( distance_squared <= ( monsters.GetTargetPosition [ monster_index ] ( ) - monsters.positions [ monster_index ] ).LengthSquared ( ) )
				{
					new_target_is_closer = true;
				}
			}
			if ( monsters.states [ monster_index ] == MonsterState::WAIT || new_target_is_closer )
			{
				monsters.states [ monster_index ] = MonsterState::ATTACK;
				monsters.TargetIsDead [ monster_index ] = [ = ]
				{
					return tanks.states [ tank_index ] == TankState::DEAD;
				};
				monsters.TargetInRange [ monster_index ] = [ = ]
				{
					const float total_radius = monsters.radiuses [ monster_index ] + tanks.radiuses [ tank_index ];
					const float total_radius_squared = total_radius * total_radius;
					return ( monsters.positions [ monster_index ] - tanks.positions [ tank_index ] ).LengthSquared ( ) <= total_radius_squared;
				};
				monsters.GetTargetPosition [ monster_index ] = [ = ]
				{
					return tanks.positions [ tank_index ];
				};
				monsters.DamageTarget [ monster_index ] = [ = ] ( const float damage )
				{
					tanks.healths [ tank_index ] -= damage;
					if ( tanks.healths [ tank_index ] <= 0 )
					{
						tanks.states [ tank_index ] = TankState::DEAD;
					}
				};
			}
			if ( ( leaders.states.front ( ) == LeaderState::SEARCH_TREASURE || leaders.states.front ( ) == LeaderState::TAKE_TREASURE ) )
			{
				leaders.targets.front ( ) = monster_index;
				leaders.states.front ( ) = LeaderState::ATTACK;
				archers.states.front ( ) = ArcherState::ATTACK;
				tanks.states.front ( ) = TankState::ATTACK;
				healers.states.front ( ) = HealerState::CLOAK;
			}
		}
	}
	);

	collision_system.CheckCollision ( arrows.positions, arrows.radiuses, monsters.positions, monsters.radiuses,
	[ this ] ( const std::size_t arrow_index, const std::size_t monster_index, const float distance_squared )
	{
		if ( monsters.states [ monster_index ] != MonsterState::DEAD )
		{
			arrows.deads [ arrow_index ] = true;
			monsters.GetDamage ( monster_index, arrows.default_damage );
		}
	}
	);

	collision_system.CheckCollision ( leaders.positions, leaders.radiuses, treasure, leaders.radiuses,
	[ this ] ( const std::size_t leader_index, const std::size_t treasure_index, const float distance_squared )
	{
		RandomiseTreasureLocation ( );
	}
	);
}

void SceneKinematics::RenderText(Mesh* mesh, std::string text, Color color)
{
	if(!mesh || mesh->textureID <= 0)
		return;
	
	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneKinematics::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if(!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneKinematics::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if(enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);
		
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{	
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if(mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if(mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneKinematics::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 3: render a sphere with radius 1
		break;
	case GameObject::GO_CUBE:

		break;
	}
}

void SceneKinematics::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Calculating aspect ratio
	//m_worldHeight = 300.f;
	//m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	RenderMesh(meshList[GEO_AXES], false);

	//Vector3 archerPosition = underwatch.GetArcher().pos;
	//Vector3 healerPosition = underwatch.GetHealer().pos;

	//modelStack.PushMatrix();
	//modelStack.Translate(archerPosition.x, archerPosition.y, archerPosition.z);
	//modelStack.Scale(9, 9, 1);
	//RenderMesh(meshList[GEO_ARCHER], false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(healerPosition.x, healerPosition.y, healerPosition.z);
	//modelStack.Scale(9, 9, 1);
	//RenderMesh(meshList[GEO_WORKER], false);
	//modelStack.PopMatrix();

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}
	if (m_ghost->active)
	{
		RenderGO(m_ghost);
	}
	//if (underwatch.GetArcher().getHP() <= 50)
	//{
	//	RenderTextOnScreen(meshList[GEO_TEXT], "Archer:I need healing!", Color(0, 1, 0), 10, 90, 5);
	//}
	else
	{

	}

	//On screen text
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 15, 0, 3);

	//std::ostringstream ss2;
	//ss2.precision(3);
	//ss2 << underwatch.GetArcher().getHP();
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 10, archerPosition.x - 14, archerPosition.y + 15);

	//std::ostringstream ss3;
	//ss3.precision(3);
	//ss3 << underwatch.GetHealer().getHP();
	//RenderTextOnScreen(meshList[GEO_TEXT], ss3.str(), Color(0, 1, 0), 10, healerPosition.x - 14, healerPosition.y + 15);

	
	//Exercise 6: print simulation speed
	
	//Exercise 10: print m_ghost position and velocity information

	//Exercise 11: print kinematics information

	//RenderTextOnScreen(meshList[GEO_TEXT], "Kinematics", Color(0, 1, 0), 3, 0, 0);

	for ( std::size_t index = 0, size = arrows.positions.size ( ); index < size; ++index )
	{
		if ( !arrows.deads [ index ] )
		{
			const Vector3 position = arrows.positions [ index ];
			modelStack.PushMatrix ( );
			modelStack.Translate ( position.x, position.y, position.z );
			modelStack.Scale ( 2, 2, 1 );
			RenderMesh ( meshList [ GEO_ARROW ], false );
			modelStack.PopMatrix ( );
		}
	}

	for ( std::size_t index = 0, size = archers.positions.size ( ); index < size; ++index )
	{
		if ( archers.states [ index ] != ArcherState::DEAD )
		{
			const Vector3 position = archers.positions [ index ];
			modelStack.PushMatrix ( );
			modelStack.Translate ( position.x, position.y, position.z );
			modelStack.Scale ( 9, 9, 1 );
			RenderMesh ( meshList [ GEO_ARCHER ], false );
			modelStack.PopMatrix ( );
		}
	}

	for ( std::size_t index = 0, size = monsters.positions.size ( ); index < size; ++index )
	{
		if ( monsters.states[ index ] != MonsterState::DEAD )
		{
			const Vector3 position = monsters.positions [ index ];
			modelStack.PushMatrix ( );
			modelStack.Translate ( position.x, position.y, position.z );
			modelStack.Scale ( 9, 9, 1 );
			RenderMesh ( meshList [ GEO_MONSTER ], false );
			modelStack.PopMatrix ( );
		}
	}
	
	for ( std::size_t index = 0, size = leaders.positions.size ( ); index < size; ++index )
	{
		if ( leaders.states [ index ] != LeaderState::DEAD )
		{
			const Vector3 position = leaders.positions [ index ];
			modelStack.PushMatrix ( );
			modelStack.Translate ( position.x, position.y, position.z );
			modelStack.Scale ( 9, 9, 1 );
			RenderMesh ( meshList [ GEO_LEADER ], false );
			modelStack.PopMatrix ( );
		}
	}

	for ( std::size_t index = 0, size = healers.positions.size ( ); index < size; ++index )
	{
		if ( healers.states [ index ] != HealerState::DEAD )
		{
			const Vector3 position = healers.positions [ index ];
			modelStack.PushMatrix ( );
			modelStack.Translate ( position.x, position.y, position.z );
			modelStack.Scale ( 9, 9, 1 );
			RenderMesh ( meshList [ GEO_HEALER ], false );
			modelStack.PopMatrix ( );
		}
	}

	for ( std::size_t index = 0, size = tanks.positions.size ( ); index < size; ++index )
	{
		if ( tanks.states [ index ] != TankState::DEAD )
		{
			const Vector3 position = tanks.positions [ index ];
			modelStack.PushMatrix ( );
			modelStack.Translate ( position.x, position.y, position.z );
			modelStack.Scale ( 9, 9, 1 );
			RenderMesh ( meshList [ GEO_WORKER ], false );
			modelStack.PopMatrix ( );
		}
	}

	for ( const Vector3 position : treasure )
	{
		modelStack.PushMatrix ( );
		modelStack.Translate ( position.x, position.y, position.z );
		modelStack.Scale ( 9, 9, 1 );
		RenderMesh ( meshList [ GEO_TREASURE ], false );
		modelStack.PopMatrix ( );
	}
}

void SceneKinematics::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
