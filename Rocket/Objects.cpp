#include "stdafx.h"
#include <cmath>
#include "Objects.h"

GL_Color GL_Color::white(1.0f, 1.0f, 1.0f, 0.0f);
GL_Color GL_Color::black(0.0f, 0.0f, 0.0f, 0.0f);

void Rocket::setContext(ContextInterface *_context)
{
	if (_context) 
		context = _context; 
	else throw IncorrectArgException("context is NULL");
}

void Rocket::start()
{
	if (!isStart)
	{
		isStart = true;
		sendEvent(1, getId(), updateTime);
	}
}

void Rocket::setVelComponents(float angle, float velocity)
{
	velY = velocity * std::cosf(PI*angle/180);
	velX = velocity * std::sinf(PI*angle/180);
}

void Rocket::setDrawingContext(DrawingContextInterface *_dContext)
{
	if (_dContext) 
		dContext = _dContext; 
	else throw IncorrectArgException("dContext is NULL");
}

void Rocket::wakeup()
{
	if (!dContext) throw NullPointerException();

	cHeight = dContext->getHeight();
	cWidth = dContext->getWidth();

	x = cWidth / 2.0f;
	y = (GLfloat)r;

	velocity = r;

	setVelComponents(angle, velocity);

	isInit= true;
	updateTime = 20;
}

void Rocket::draw()
{
	dContext->drawDisk(x,y,r);
}

void Rocket::recieveEvent(const Event* e)
{
	int label = e->getLabel();

	switch(label)
	{
	case 1:
		y += velY;
		x += velX;

	
		if (x <= r || x >= cWidth - r)
		{
			velX = -velX;
		}

		if (y <= r || y >= cHeight -r)
		{
			velY = - velY;
		}

		dContext->redrawScene();
		
		sendEvent(1, getId(), updateTime);

		break;
	}
}