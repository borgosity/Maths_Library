#pragma once

#include <random>
#include <math.h>
#include <string>

#include <GLFW/glfw3.h>

#include "Application2D.h"
#include "Wall.h"
#include "GameDef.h"

#include "SpriteBatch.h"
#include "Texture.h"
#include "Font.h"



#define M_PI       3.14159265358979323846   // pi


Wall::Wall()
{
	m_WallSprite = new Texture("./bin/textures/Wall.png");
	m_fontDebug = new Font("./bin/font/consolas.ttf", 15);
	m_debug = false;
	m_mass = 10;
	Reset();
}

Wall::Wall(float mass = 10.0f)
{
	m_WallSprite = new Texture("./bin/textures/Wall.png");
	m_fontDebug = new Font("./bin/font/consolas.ttf", 15);
	m_debug = false;
	m_mass = mass;
	Reset();
}


Wall::~Wall()
{
}

void Wall::Update(float dt)
{
	Application2D* app = Application2D::getInstance();

	if (app->IsKeyDown(GLFW_KEY_LEFT))
	{
		m_rotation += TURN_LEFT;
		m_direction = 4;
	}
	if (app->IsKeyDown(GLFW_KEY_RIGHT))
	{
		m_rotation += TURN_RIGHT;
		m_direction = 3;
	}
	if (app->IsKeyDown(GLFW_KEY_UP))
	{
		m_velocity.m_y += SPEED;
	}
	else
	{
		SlowDown(FRICTION);
	}
	if (app->IsKeyDown(GLFW_KEY_DOWN))
	{
		if (m_velocity.m_y != 0)
		{
			SlowDown(SPEED);
		}
	}
	if (app->IsKeyDown(GLFW_KEY_D))
	{
		if (m_debug)
		{
			m_debug = false;
		}
		else
		{
			m_debug = true;
		}
	}

	// check Walls movement and set flags
	DirectionCheck();

	// check if Wall has reached the edge of the game zone
	EdgeDectection();
	
	// set current postion as last position
	m_prevPos = m_position;

	//MoveWall(m_direction);
	Matrix3 m(0);
	m.setRotateZ(m_rotation);// *(M_PI / 2));
	Vector3 v = m * m_velocity;
	m_position += v * dt;

}

void Wall::Draw(SpriteBatch * batch)
{
	Matrix3 transpose(0);
	transpose.CreateTranslation(Vector3(m_position.m_x, m_position.m_y, 0));
	Matrix3 rotation(0);
	rotation.setRotateZ(m_rotation);// *(M_PI / 2));
	Matrix3 translation = transpose;
	
	//***************************************
	// rotate around 0/0 world cordinates
	//translation = rotation * transpose;
	//batch->drawSpriteTransformed3x3(m_WallSprite, (float*)translation, 100, 100);
	
	//*************************************** 
	translation = transpose * rotation;
	batch->drawSpriteTransformed3x3(m_WallSprite, (float*)translation, 100, 100);

	//batch->drawSprite(m_WallSprite, m_position.m_x, m_position.m_y, 100, 100, 3.14159f * 0.25f);

	//*********************************************************************************************************
	// debugging
	if (m_debug)
	{
		// -- DIRECTION OF TURN
		if (m_direction == 3)
		{
			batch->drawText(m_fontDebug, "Turned Right |", 10, 20);
		}
		else if (m_direction == 4)
		{
			batch->drawText(m_fontDebug, "Turned Left  |", 10, 20);
		}
		else
		{
			batch->drawText(m_fontDebug, "Waiting...   |", 10, 20);
		}
		// -- POSITION
		std::string position = "X = " + std::to_string(m_position.m_x) + ", Y = " + std::to_string(m_position.m_y) + "  |";
		batch->drawText(m_fontDebug, position.c_str(), 200, 20);
		// -- ROTATION
		std::string rotation = "R = " + std::to_string(m_rotation);
		batch->drawText(m_fontDebug, rotation.c_str(), 600, 20);
		// -- VELOCITY
		std::string velocity = "V = " + std::to_string(m_velocity.m_y);
		batch->drawText(m_fontDebug, velocity.c_str(), 800, 20);
	}
	//**********************************************************************************************************/
}

void Wall::Reset()
{
	m_position.m_x = HALF_SW;
	m_position.m_y = HALF_SH;
	m_velocity.m_x = 0;
	m_velocity.m_y = 0;
	m_rotation = 0.0;
	// set random direction of Wall
	//ChangeDirection();
	m_direction = 0;
	
}

void Wall::ResetVelocity()
{
	m_velocity.m_x = 0;
	m_velocity.m_y = 0;
	m_velocity.m_z = 0;
}

int Wall::RandomDir()
{
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, 8);
	int tempDir = distribution(generator);

	return tempDir;
}

void Wall::DirectionCheck()
{
	// check current x position agianst previous x position and set flag
	if (m_position.m_x > m_prevPos.m_x)
	{
		m_xPlus = 1;
	}
	else if (m_position.m_x < m_prevPos.m_x)
	{
		m_xPlus = 0;
	}
	else
	{
		m_xPlus = -1;
	}
	// check current y position agianst previous y position and set flag
	if (m_position.m_y > m_prevPos.m_y)
	{
		m_yPlus = 1;
	}
	else if (m_position.m_y < m_prevPos.m_y)
	{
		m_yPlus = 0;
	}
	else
	{
		m_yPlus = -1;
	}
}

void Wall::EdgeDectection()
{
	//m_rotation = 0;
	Vector3 bump(2, 2, 0); // bump Wall off edge so they don't get stuck
	// right/eastern edge check
	if ((m_position.m_x + WALL_EDGE) >= SCREEN_W)
	{
		if (m_xPlus == 1 && m_yPlus == 1)
		{
			m_rotation += TURN_LEFT * 2;// *(float)M_PI;
			m_direction = 4;
		}
		else if (m_xPlus == 1 && m_yPlus == 0)
		{
			m_rotation += TURN_RIGHT * 2;
			m_direction = 3;
		}
 		else
		{
			m_rotation += TURN_RIGHT;
			m_direction = 3;	
		}
		m_position -= bump;
		Bounce(BOUNCE);
	}
	// top/northern edge check
	else if ((m_position.m_y + WALL_EDGE) >= SCREEN_H)
	{
		if (m_xPlus == 1 && m_yPlus == 1)
		{
			m_rotation += TURN_RIGHT * 2;// *(float)M_PI;
			m_direction = 3;
		}
		else if (m_xPlus == 0 && m_yPlus == 1)
		{
			m_rotation += TURN_LEFT * 2;
			m_direction = 4;
		}
		else
		{
			m_rotation += TURN_LEFT;
			m_direction = 4;
			
		}
		m_position -= bump;
		Bounce(BOUNCE);
	}
	// left/western edge check
	else if ((m_position.m_x - WALL_EDGE) <= 0)
	{
		if (m_xPlus == 0 && m_yPlus == 0)
		{
			m_rotation += TURN_LEFT * 2;// *(float)M_PI;
			m_direction = 4;
		}
		else if (m_xPlus == 0 && m_yPlus == 1)
		{
			m_rotation += TURN_RIGHT * 2;
			m_direction = 3;
		}
		else
		{
			m_rotation += TURN_RIGHT;
			m_direction = 3;
		}
		m_position += bump;
		Bounce(BOUNCE);
	}
	// bottom/southern edge check
	else if ((m_position.m_y - WALL_EDGE) <= 0)
	{
		if (m_xPlus == 0 && m_yPlus == 0)
		{
			m_rotation += TURN_RIGHT * 2;// *(float)M_PI;
			m_direction = 3;
		}
		else if (m_xPlus == 1 && m_yPlus == 0)
		{
			m_rotation += TURN_LEFT * 2;
			m_direction = 4;
		}
		else
		{
			m_rotation += TURN_LEFT;
			m_direction = 4;
		}
		m_position += bump;
		Bounce(BOUNCE);
	}

	if (m_position.m_x > SCREEN_W || m_position.m_x < 0 || m_position.m_y > SCREEN_H || m_position.m_y < 0)
	{
		Reset();
	}
}

void Wall::SlowDown(int extra)
{
	if ((m_velocity.m_y -= m_mass + extra) < 0)
	{
		m_velocity.m_y = 0;
	}
}

void Wall::Bounce(int extra)
{
	m_velocity.m_y += m_mass + extra;
}

