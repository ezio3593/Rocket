#include "stdafx.h"
#include <cmath>
#include "Objects.h"

GL_Color GL_Color::white(1.0f, 1.0f, 1.0f, 0.0f);
GL_Color GL_Color::black(0.0f, 0.0f, 0.0f, 0.0f);

void Rocket::start()
{
	if (!isStart)
	{
		isStart = true;
		sendEvent(MOVE, getId(), updateTime);
	}
}

void Rocket::setVelComponents(float angle, float velocity)
{
	velY = velocity * std::cosf(PI*angle/180);
	velX = velocity * std::sinf(PI*angle/180);
}

void Rocket::wakeup()
{
	float width = maxX - minX;
	float heigth = maxY - minY;

	if (width <= 0 || heigth <= 0) throw IncorrectDataException("Incorrect coords limits");

	x = width / 2.0f;
	y = (GLfloat)r;

	velocity = r;

	setVelComponents(angle, velocity);

	isInit= true;
	updateTime = 20;
}

void Rocket::recieveEvent(const Event& e)
{
	int label = e.getLabel();

	switch(label)
	{
	case MOVE:
		y += velY;
		x += velX;

	
		if (x <= minX + r || x >= maxX - r)
		{
			velX = -velX;
		}

		if (y <= minY + r || y >= maxY -r)
		{
			velY = - velY;
		}
		
		sendEvent(MOVE, getId(), updateTime);

		break;
	}
}