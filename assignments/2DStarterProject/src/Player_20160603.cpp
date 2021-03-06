#pragma once

#include <random>
#include <math.h>
#include <string>

#include <GLFW/glfw3.h>

#include "Application2D.h"
#include "Player.h"
#include "GameDef.h"

#include "SpriteBatch.h"
#include "Texture.h"
#include "Font.h"



#define M_PI       3.14159265358979323846   // pi


Player::Player()
{
	m_playerSprite = new Texture("./bin/textures/player.png");
	m_fontDebug = new Font("./bin/font/consolas.ttf", 15);
	m_fontHUD = new Font("./bin/font/consolas.ttf", 25);
	m_debug = false;
	m_mass = 10.0f;
	m_size = 100.0f;
	m_currentSize = m_size;
	m_health = 100;
	m_alive = true;
	Reset();
}

Player::Player(float size = 100.0f, float mass = 10.0f)
{
	m_playerSprite = new Texture("./bin/textures/player.png");
	m_fontDebug = new Font("./bin/font/consolas.ttf", 15);
	m_fontHUD = new Font("./bin/font/consolas.ttf", 25);
	m_debug = false;
	m_mass = mass;
	m_size = size;
	m_currentSize = m_size;
	m_health = PLAYER_HEALTH;
	m_alive = true;
	Reset();
}


Player::~Player()
{
}

void Player::Update(float dt)
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

	// check players movement and set flags
	DirectionCheck();

	// check if player has reached the edge of the game zone
	EdgeDectection();
	
	// set current postion as last position
	m_prevPos = m_position;

	//MovePlayer(m_direction);
	Matrix3 m(0);
	m.setRotateZ(m_rotation);// *(M_PI / 2));
	Vector3 v = m * m_velocity;
	m_position += v * dt;

}

void Player::Draw(SpriteBatch * batch)
{
	Matrix3 transpose(0);
	transpose.CreateTranslation(Vector3(m_position.m_x, m_position.m_y, 0));
	Matrix3 rotation(0);
	rotation.setRotateZ(m_rotation);// *(M_PI / 2));
	Matrix3 translation = transpose;
	
	//***************************************
	// rotate around 0/0 world cordinates
	//translation = rotation * transpose;
	//batch->drawSpriteTransformed3x3(m_playerSprite, (float*)translation, 100, 100);
	
	//*************************************** 
	// Display Players Health
	std::string health = "Health " + std::to_string(m_health);
	batch->drawText(m_fontHUD, health.c_str(), 1100, 700);
	
	// rotate Player around local co-ordinates
	translation = transpose * rotation;
	batch->drawSpriteTransformed3x3(m_playerSprite, (float*)translation, m_currentSize, m_currentSize);

	//batch->drawSprite(m_playerSprite, m_position.m_x, m_position.m_y, 100, 100, 3.14159f * 0.25f);

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

void Player::Reset()
{
	m_position.m_x = HALF_SW;
	m_position.m_y = HALF_SH;
	m_velocity.m_x = 0;
	m_velocity.m_y = 0;
	m_rotation = 0.0;
	m_direction = 0;
	m_currentSize = m_size;
	m_health = PLAYER_HEALTH;
	m_alive = true;
	
}

void Player::ResetVelocity()
{
	m_velocity.m_x = 0;
	m_velocity.m_y = 0;
	m_velocity.m_z = 0;
}

void Player::ApplyCollision(int damage)
{
	if (m_currentSize < (PLAYER_SIZE * 2) && m_currentSize > (PLAYER_SIZE / 2))
	{
		if (m_health > 0)
		{
			m_currentSize += SIZE_PENALTY;
			m_rotation += TURN_LEFT;
			m_health -= damage/2;
		}
		else
		{
			m_alive = false;
			// play plink death sequence
		}

	}
	else if (m_currentSize <= (PLAYER_SIZE / 2))
	{
		if (m_health > 0)
		{
			m_health -= damage;
		}
		else
		{
			m_alive = false;
			// play plink death sequence
		}
	}
	else if (m_currentSize >= (PLAYER_SIZE * 2))
	{
		m_alive = false;
		// play pop death sequence
	}
}

void Player::SetRotation(float rotate)
{
	m_rotation += rotate;
}

void Player::SetSize(float size)
{
	m_size += size;
}

Vector3 Player::GetPosition()
{
	return m_position;
}

const float Player::GetSize()
{
	return m_currentSize;
}

bool Player::IsAlive()
{
	return m_alive;
}

int Player::RandomDir()
{
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, 8);
	int tempDir = distribution(generator);

	return tempDir;
}

void Player::DirectionCheck()
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

void Player::EdgeDectection()
{
	//m_rotation = 0;
	Vector3 bump(2, 2, 0); // bump player off edge so they don't get stuck
	// right/eastern edge check
	if ((m_position.m_x + PLAYER_EDGE) >= SCREEN_W)
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
	else if ((m_position.m_y + PLAYER_EDGE) >= SCREEN_H)
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
	else if ((m_position.m_x - PLAYER_EDGE) <= 0)
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
	else if ((m_position.m_y - PLAYER_EDGE) <= 0)
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

void Player::SlowDown(int extra)
{
	if ((m_velocity.m_y -= m_mass + extra) < 0)
	{
		m_velocity.m_y = 0;
	}
}

void Player::Bounce(int extra)
{
	m_velocity.m_y += m_mass + extra;
}

