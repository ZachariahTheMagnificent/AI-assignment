// Program : Patrol Solution

#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <string>
#include <ostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFw/glfw3.h>
#include <ft2build.h>
#include <vector>
#include "Vector3.h"
#include FT_FREETYPE_H
using namespace std;

#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

//Variable to store the time of day in the game from 0-100(>50 == nighttime)
float current_time = 0.0f;

double last_frame_timestamp;
double delta_time;

class Map
{
public:
	float GetSizeX() const
	{
		return map_size_x;
	}

	float GetSizeY() const
	{
		return map_size_y;
	}

private:
	Vector3 position;
	float map_size_x;
	float map_size_y;
};

Map map;

class Base
{
public:
	void Update(const double time)
	{
		if (time == 1.f)
		{
			++num_bows;
		}
	}

	//Used by archers
	void GrabBow()
	{
		--num_bows;
	}

	unsigned GetNumBows()
	{
		return num_bows;
	}

	float StartingRegionX()
	{
		return position.x - half_size_x;
	}

	float StartingRegionY()
	{
		return position.y - half_size_y;
	}

	float EndingRegionX()
	{
		return position.x + half_size_x;
	}

	float EndingRegionY()
	{
		return position.y + half_size_y;
	}

private:
	Vector3 position;
	float half_size_x;
	float half_size_y;
	unsigned num_bows;
};

Base base;

//Defenders of the base
class Archer
{
public:
	enum class A_STATE
	{
		UNRECRUITED,
		GRAB_TOOL,
		HUNT,
		DEFEND,
		REPOSITION,
		END
	};

	//Set a random position outside the walls for archers to roam to when hunting
	void SetRoamingPosition()
	{
		//get the perimeter of the base.
		const float base_starting_region_x = base.StartingRegionX();
		const float base_starting_region_y = base.StartingRegionY();
		const float base_ending_region_x = base.EndingRegionX();
		const float base_ending_region_y = base.EndingRegionY();

		while (true)
		{
			//generate a value within the map perimeter.
			roam_position.x = Math::RandFloatMinMax(0, map.GetSizeX());

			//If value is outside base
			if (roam_position.x < base_starting_region_x || roam_position.x > base_ending_region_x)
			{
				break;
			}
		}
		while (true)
		{
			roam_position.y = Math::RandFloatMinMax(0, map.GetSizeY());
			if (roam_position.y < base_starting_region_y || roam_position.y > base_ending_region_y)
			{
				break;
			}
		}
	}

	void Update(const double time)
	{
		switch (state)
		{
		case A_STATE::UNRECRUITED:
			//If there are tools for the archers to grab.
			if (base.GetNumBows() > 0)
			{
				state = A_STATE::GRAB_TOOL;
			}
			break;
		case A_STATE::GRAB_TOOL:
		{
			const Vector3 archer_to_base = (base.GetPosition() - position);
			const float epsilon = 1;

			//if archer is close to base
			if (archer_to_base.LengthSquared() == epsilon * epsilon)
			{
				base.GrabBow();
				//if nighttime
				if (current_time > 50)
				{
					state = A_STATE::REPOSITION;
				}
				else
				{
					SetRoamingPosition();
					state = A_STATE::HUNT;
				}
			}
			const float speed = 7;
			const Vector3 direction = archer_to_base.Normalized();
			position += direction * speed * time;
			break;
		}
		case A_STATE::HUNT:
		{
			const Vector3 archer_to_roam = roam_position - position;
			const float epsilon = 1;
			//if nighttime
			if (current_time > 50)
			{
				state = A_STATE::REPOSITION;
			}
			//If reach roaming position
			else if (archer_to_roam.LengthSquared() == epsilon * epsilon)
			{
				SetRoamingPosition();
			}
			const float speed = 7;
			const Vector3 direction = archer_to_roam.Normalized();
			position += direction * speed * time;

			//rabbit hunting code here
			//for (auto& entity)

			break;
		}
		case A_STATE::DEFEND:
			break;
		case A_STATE::REPOSITION:
			break;
		}
	}

private:
	Vector3 position;
	A_STATE state;
	Vector3 roam_position;
	float base_starting_region_x;
	float base_starting_region_y;
	float base_ending_region_x;
	float base_ending_region_y;
};

class TrackingSystem
{
public:
	static const unsigned TOTAL_ARCHERS = 27;

	Archer* GetArchers()
	{
		return archers;
	}

private:
	Archer archers[TOTAL_ARCHERS];
};

class Monster
{
public:
	enum class M_STATE
	{
		ATK_ARCHER,
		ATK_STRUCTURE,
		FLEE,
		DEAD,
		END
	};
};

enum class TIME
{
	DAY,
	NIGHT,
	END,
};


void Render( GLFWwindow* window );

int RandomInteger( int lowerLimit, int upperLimit )
{
	return rand() % ( upperLimit - lowerLimit + 1 ) + lowerLimit;
}

double GetCurrTime()
{
	LARGE_INTEGER frequency, current_time;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&current_time);

	return double(current_time.LowPart) / double(frequency.LowPart);
}

// long integer to string
string itos( const long value )
{ 
	ostringstream buffer; 
	buffer << value; 
	return buffer.str();
}

struct MyVector
{
	float x, y;
	MyVector() :x(0), y(0){}
	MyVector(float x, float y) :x(x), y(y){}
	void SetPosition(float _x, float _y){ x = _x; y = _y; }
	float GetX(){ return x; }
	float GetY(){ return y; }
	float Magnitude(){ return sqrt(x*x + y*y); }
	MyVector Normalize(){ float length = Magnitude(); return MyVector(x / length, y / length); }
	MyVector operator + (MyVector u){ return MyVector(x + u.x, y + u.y); }
	MyVector operator - (MyVector u){ return MyVector(u.x - x, u.y - y); }
	MyVector operator += (MyVector u){ return MyVector(x + u.x, y + u.y); }
	MyVector operator ~(){ return MyVector(-x, -y); }
	MyVector operator *(float scale){ return MyVector(x*scale, y*scale); }
	float operator * (MyVector  v){ return  x*v.x + y*v.y; }
};

float GetDistance(float x1, float y1, float x2, float y2) { return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)); } // OK

// Within range
bool Detect( MyVector pos1, MyVector pos2, float radius1, float radius2)
{
	bool detect = false;
	float totalRadius = radius1 + radius2;
	float distance = GetDistance(pos1.x, pos1.y, pos2.x, pos2.y);
	if (distance <= totalRadius) detect = true;
	return detect;
}

void RenderCircle( GLfloat x, GLfloat y, GLfloat radius, GLfloat r, GLfloat g, GLfloat b )
{
	int n = 360;
	glColor3f( r, g, b );
	glBegin( GL_POINTS );
	for ( int i = 0; i <= n; i++ )
	{
		float angle = (float) ( i * ( 2.0 * 3.14159 / n ) );
		glVertex2f( x + radius * cos( angle ), y + radius * sin( angle ) );
	}
	glEnd();
}

void RenderFillCircle( GLfloat x, GLfloat y, GLfloat radius, GLfloat r, GLfloat g, GLfloat b )
{
	int n = 360;
	glColor3f ( r, g, b );
	glBegin(GL_TRIANGLE_FAN );
	glVertex2f( x, y );
	for (int i = 0; i <= n; i++)
	{
		float angle = (float) ( i * ( 2.0 * 3.14159 / n ) );
		glVertex2f( x + radius * cos( angle ), y + radius * sin( angle ) );
	}
	glEnd();
}
void RenderSquare(GLfloat r, GLfloat g, GLfloat b)
{
	int n = 360;
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLES);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, -1);
	glVertex2f(-1, 1);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glEnd();
}

// SHADERS //////////////////////////////////////////////////////////////////
const char *VERTEX_SHADER = ""
"#version 410 core\n"
"in vec4 in_Position;\n"
"out vec2 texCoords;\n"
"void main(void) {\n"
"    gl_Position = vec4(in_Position.xy, 0, 1);\n"
"    texCoords = in_Position.zw;\n"
"}\n";

const char *FRAGMENT_SHADER = ""
"#version 410 core\n"
"precision highp float;\n"
"uniform sampler2D tex;\n"
"uniform vec4 color;\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"void main(void) {\n"
"    fragColor = vec4(1, 1, 1, texture(tex, texCoords).r) * color;\n"
"}\n";

GLuint texUniform, colorUniform;
GLuint texture{ 0 }, sampler{ 0 };
GLuint vbo{ 0 }, vao{ 0 };
GLuint vs{ 0 }, fs{ 0 }, program{ 0 };

///////////////////////////////////////////////////////////////////

static void ErrorCallBack ( int error, const char*description )
{
	fputs( description, stderr );
}

float width, height;
GLFWwindow* window;
void Render(GLFWwindow* window);

static void ResizeCallBack(GLFWwindow *window, int w, int h)
{
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(-w*0.5, w*0.5, -h*0.5, h*0.5);
	//gluPerspective( 60, (float) w / (float) h, 0, 100 );
	glMatrixMode( GL_MODELVIEW );
}

// Free Type //////////////////////////////////////////////////////
FT_Library ft_lib{ nullptr };
FT_Face face{ nullptr };

void RenderText(const string str, FT_Face face, float x, float y, float _sx, float _sy)
{
	float sx = _sx / width;
	float sy = _sy / height;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	const FT_GlyphSlot glyph = face->glyph;
	for (auto c : str) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			continue;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
			glyph->bitmap.width, glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

		const float vx = x + glyph->bitmap_left * sx;
		const float vy = y + glyph->bitmap_top * sy;
		const float w = glyph->bitmap.width * sx;
		const float h = glyph->bitmap.rows * sy;

		struct {
			float x, y, s, t;
		} data[6] = {
			{ vx, vy, 0, 0 },
			{ vx, vy - h, 0, 1 },
			{ vx + w, vy, 1, 0 },
			{ vx + w, vy, 1, 0 },
			{ vx, vy - h, 0, 1 },
			{ vx + w, vy - h, 1, 1 }
		};
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (glyph->advance.x >> 6) * sx;
		y += (glyph->advance.y >> 6) * sy;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
///////////////////////////////////////////////////////////////////

void Cleanup()
{
	FT_Done_Face(face);
	FT_Done_FreeType(ft_lib);
	glDeleteTextures(1, &texture);
	glDeleteSamplers(1, &sampler);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);
}

void DoExit()
{
	Cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
//int state; // Current state value
//const int PATROL = 0; // Possible state definition
//const int CHASE = 1;
const float rabbitSpeed = 2.0f;
const float playerSpeed = 0.0175f;
const float enemySpeed = 0.0176f;
const float playerRadius = 0.25f;
const float enemyRadius = 0.1f;
const float proximity = 0.4f;
int waypointIndex;
bool arrived;
MyVector playerPos, enemyPos, rabbitPos;
vector <MyVector> wayPoints, intrusionPoints;

MyVector nextPoint;
vector <MyVector> stack;


static void KeyCallBack( GLFWwindow *window, int key, int scancode, int action, int mods )
{
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GL_TRUE );
}

void SimulationInit()
{
	srand((unsigned)time(NULL));
	float offset = 2.0;
	wayPoints.push_back(MyVector(-offset, -offset));
	wayPoints.push_back(MyVector(-offset, offset));
	wayPoints.push_back(MyVector(offset, offset));
	wayPoints.push_back(MyVector(offset, -offset));
	intrusionPoints.push_back(MyVector(-1.2f*offset, -0.3f*offset));
	intrusionPoints.push_back(MyVector(-1.2f*offset, 0.3f*offset));
	intrusionPoints.push_back(MyVector(1.2f*offset, 0.3f*offset));
	intrusionPoints.push_back(MyVector(1.2f*offset, -0.3f*offset));
	playerPos.SetPosition(wayPoints[0].GetX(), wayPoints[0].GetY());
	int randomIndex = RandomInteger(1, 3);
	enemyPos.SetPosition(intrusionPoints[randomIndex].GetX(), intrusionPoints[randomIndex].GetY());
	//state = PATROL;
	waypointIndex = 1;
	arrived = false;

}

int main()
{
	// INIT ///////////////////////////////////////////////////////////////
	char *title = "Patrol";
	width = 640;
	height = 480;
	last_frame_timestamp = GetCurrTime();
	
	glfwSetErrorCallback( ErrorCallBack );
	if ( !glfwInit() )
		exit( EXIT_FAILURE );
	glfwWindowHint(GLFW_SAMPLES,4);
	GLFWwindow* window = glfwCreateWindow( (int) width, (int) height, title, NULL , NULL );
	
	if (!window)
	{
		fprintf( stderr, "Failed to create GLFW windows.\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	}
	
	glfwMakeContextCurrent( window );
	glfwSetKeyCallback( window, KeyCallBack );
	glfwSetWindowSizeCallback( window, ResizeCallBack );
	
	GLenum error =  glewInit();
	if ( error != GLEW_OK)
	{
		fprintf(stderr,"Error : %s\n", glewGetErrorString( error ) );
		exit( EXIT_FAILURE );
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60, width / height, 0, 100 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	///////////////////////////////////////////////////////////////////////////
	
	// Initialize and load our freetype face
	if ( FT_Init_FreeType( &ft_lib ) != 0 )
	{
		fprintf( stderr, "Couldn't initialize FreeType library\n" );
		Cleanup();
		exit( EXIT_FAILURE );
	}
	
	if ( FT_New_Face(ft_lib, "arial.ttf", 0, &face ) != 0 )
	{
		fprintf( stderr, "Unable to load arial.ttf\n" );
		Cleanup();
		exit( EXIT_FAILURE );
	}
	// Initialize our texture and VBOs
	glGenBuffers( 1, &vbo );
	glGenVertexArrays( 1, &vao );
	glGenTextures( 1, &texture );
	glGenSamplers(1, &sampler);
	glSamplerParameteri( sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glSamplerParameteri( sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glSamplerParameteri( sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glSamplerParameteri( sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	// Initialize shader
	vs = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vs, 1, &VERTEX_SHADER, 0 );
	glCompileShader( vs );

	fs = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fs, 1, &FRAGMENT_SHADER, 0 );
	glCompileShader( fs );

	program = glCreateProgram();
	glAttachShader( program, vs );
	glAttachShader( program, fs );
	glLinkProgram( program );
	
	// Initialize GL state
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Get shader uniforms
	glUseProgram( program );
	glBindAttribLocation( program, 0, "in_Position" );
	texUniform = glGetUniformLocation( program, "tex" );
	colorUniform = glGetUniformLocation( program, "color" );

	SimulationInit();
	Render( window );
	DoExit( );
}

void RenderObjects()
{	
	//glPushMatrix();
	//glTranslatef( 0.0f, 0.0f, -10.0f );
	//
	//// Player
	//RenderFillCircle(playerPos.GetX(), playerPos.GetY(), playerRadius, 0.0f, 0.0f, 1.0f);
	//RenderCircle(playerPos.GetX(), playerPos.GetY(), playerRadius + proximity, 0.1f, 0.1f, 0.1f);
	//// Enemy
	//RenderFillCircle(enemyPos.GetX(), enemyPos.GetY(), enemyRadius, 0.0f, 1.0f, 0.0f);
	//// Waypoints
	//for (unsigned int i = 0; i < wayPoints.size(); i++)
	//	RenderCircle(wayPoints[i].GetX(), wayPoints[i].GetY(), playerRadius + 0.1f, 1.0f, 0.0f, 0.0f);

	//glPopMatrix();

	glPushMatrix();
	//Walls
	glTranslatef(0.0f, 3.8f, -10.0f);
	glScalef(4.0f, 0.2f,0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();

	glPushMatrix();
	//Walls
	glTranslatef(0.0f, -3.8f, -10.0f);
	glScalef(4.0f, 0.2f, 0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();

	glPushMatrix();
	//Walls
	glTranslatef(4.0f, 0.0f, -10.0f);
	glScalef(0.2f, 4.0f, 0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();

	glPushMatrix();
	//Walls
	glTranslatef(-4.0f, 0.0f, -10.0f);
	glScalef(0.2f, 4.0f, 0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();
//////////////////////////////////////////////////////////////
	glPushMatrix();
	//Walls
	glTranslatef(0.0f, 1.8f, -10.0f);
	glScalef(2.0f, 0.2f, 0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();

	glPushMatrix();
	//Walls
	glTranslatef(0.0f, -1.8f, -10.0f);
	glScalef(2.0f, 0.2f, 0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();

	glPushMatrix();
	//Walls
	glTranslatef(2.2f, 0.0f, -10.0f);
	glScalef(0.2f, 2.0f, 0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();

	glPushMatrix();
	//Walls
	glTranslatef(-2.2f, 0.0f, -10.0f);
	glScalef(0.2f, 2.0f, 0.0f);
	RenderSquare(0, 1, 0);
	glPopMatrix();
///////////////////////////////////////////////////////////////
	glPushMatrix();
	glTranslatef( 0.0f, 0.0f, -10.0f );
	RenderFillCircle(0, 0, 0.5, 0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 3.0f, -10.0f);
	glScalef(0.5f, 0.5f, 0.0f);
	RenderFillCircle(0, 0, 0.5, 1.0f, 1.0f, 1.0f);
	glPopMatrix();


	//for (unsigned int i = 0; i < wayPoints.size(); i++)
	//	RenderCircle(wayPoints[i].GetX(), wayPoints[i].GetY(), playerRadius + 0.1f, 1.0f, 0.0f, 0.0f);
	// Rabbit
	glPushMatrix();
	RenderFillCircle(rabbitPos.GetX(), rabbitPos.GetY(), enemyRadius, 1.0f, 1.0f, 0.0f);
	glPopMatrix();
}

void RunFSM()
{
	/*bool enemyDetected = Detect(playerPos, enemyPos, playerRadius + proximity, enemyRadius);
	switch (state)
	{
		case PATROL:if (enemyDetected)
		{
			state = CHASE;
			stack.push_back(playerPos);
		}
		
		case CHASE: if (!enemyDetected)
		{
			state = PATROL;
		}
		
	}*/

}

void Update()
{
	const double current_frame_timestamp = GetCurrTime();
	delta_time = current_frame_timestamp - last_frame_timestamp;
	last_frame_timestamp = current_frame_timestamp;

	double real_to_game_time_ratio = 1.0;

	current_time += delta_time * real_to_game_time_ratio;

	if (current_time >= 100.0f)
	{
		current_time = 0.0f;
	}

	//if day time, WABBITS
	if (current_time < 50.0f)
	{

	}
	/*if (state != CHASE)
	{
		if (stack.size() == 0)
			nextPoint = wayPoints[waypointIndex];
		else
			nextPoint = stack[stack.size() - 1];
		MyVector direction = (playerPos - nextPoint).Normalize();
		float distance = GetDistance(playerPos.GetX(), playerPos.GetY(), nextPoint.GetX(), nextPoint.GetY());
		if (distance < playerSpeed)
		{
			playerPos = nextPoint;
			arrived = true;
		}
		else
			playerPos = playerPos + direction*playerSpeed;
		if (arrived)
		{
			if (stack.size() == 0)
			{
				if (waypointIndex == wayPoints.size() - 1)
					waypointIndex = 0;
				else
					waypointIndex++;
			}
			else
				stack.clear();
			arrived = false;
		}
	}*/
}

void Render( GLFWwindow* window )
{
	while ( !glfwWindowShouldClose( window) )
	{
		glUseProgram( 0 );
		glClear( GL_COLOR_BUFFER_BIT );

		Update();
		//RunFSM();
		//string stateString = "";
		//MyVector direction;
		/*switch (state)
		{
		case PATROL: stateString = "PATROL";
			break;
		case CHASE: stateString = "CHASE";
			direction = (playerPos - enemyPos).Normalize();
			enemyPos = enemyPos + direction * enemySpeed;
			playerPos = playerPos + direction * playerSpeed;
			break;
		}*/
		RenderObjects();

		// Bind stuff
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture );
		glBindSampler( 0, sampler);
		glBindVertexArray( vao );
		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glUseProgram( program );
		glUniform4f( colorUniform, 1, 1, 1, 1 );
		glUniform1i( texUniform, 0 );

		FT_Set_Pixel_Sizes( face, 0, 50 );

		if (current_time < 10)
		{
			RenderText("DAY", face, -0.8f, 0.925f, 0.55f, 0.55f);
		}
		else
		{
			RenderText("NIGHT", face, -0.8f, 0.925f, 0.55f, 0.55f);
		}

		{
			char buffer[256];
			//int x = 32;
			//sprintf_s(buffer, "Time: %d, %d, %f, %s", x, 47, current_time, "lol");
			sprintf_s(buffer, "TIME: %f", current_time);
			RenderText(buffer, face, -0.6f, 0.925f, 0.55f, 0.55f);
		}

		
		RenderText("State : ", face, -0.95f, 0.925f, 0.55f, 0.55f);
		//RenderText(stateString, face, -0.8f, 0.925f, 0.55f, 0.55f);
		//RenderText("Player - Blue Enemy - Green Red - Patrol Point", face, -0.6f, 0.925f, 0.55f, 0.55f);

		glfwSwapBuffers( window );
		glfwPollEvents();
	}
}