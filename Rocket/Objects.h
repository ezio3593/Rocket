#include <exception>
#include <string>
#include <gl\GL.h>
#include "threads.h"

#pragma once

typedef void* id;

class GL_Color
{
	GLfloat colorR;
	GLfloat colorG;
	GLfloat colorB;
	GLfloat colorA;

public:
	static GL_Color white;
	static GL_Color black;

	GL_Color(GLfloat _colorR = 0.0f, GLfloat _colorG = 0.0f, GLfloat _colorB = 0.0f, GLfloat _colorA = 0.0f):
		colorR(_colorR), colorG(_colorG), colorB(_colorB), colorA(_colorA) {}
	GLfloat getRed() const { return colorR; }
	GLfloat getGreen() const { return colorG; }
	GLfloat getBlue() const { return colorB; }
	GLfloat getAlpha() const { return colorA; }
};

class GL_Rect
{
	GLfloat x1;
	GLfloat y1;
	GLfloat x2;
	GLfloat y2;
	GLfloat angle;
	GLfloat rotX;
	GLfloat rotY;
	GL_Color color;
public:
	GL_Rect(GLfloat _x1, GLfloat _y1, GLfloat _x2, GLfloat _y2, GLfloat _angle = 0.0f, GLfloat _rotX = 0.0f, GLfloat _rotY = 0.0f, GL_Color _color = GL_Color::black):
		x1(_x1), y1(_y1), x2(_x2), y2(_y2), angle(_angle), rotX(_rotX), rotY(_rotY), color(_color) {}
	GLfloat getX1() { return x1; }
	GLfloat getY1() { return y1; }
	GLfloat getX2() { return x2; }
	GLfloat getY2() { return y2; }
	GLfloat getAngle() { return angle; }
	GLfloat getRotateX() { return rotX; }
	GLfloat getRotateY() { return rotY; }
	GL_Color getColor() { return color; }
};

class ContextInterface
{
public:
	virtual void addEventFromObj(int label, id source, id dest, LONG64 timeDiff) = 0;
	virtual CriticalSection& getObjCriticalSection() = 0;
};

class DrawingContextInterface
{
public:
	virtual void redrawScene() = 0;
	virtual LONG getHeight() = 0;
	virtual LONG getWidth() = 0;
	virtual void drawDisk(GLfloat x, GLfloat y, GLdouble r, GLint slices = 20, GLint loops = 1, GL_Color = GL_Color::black) = 0;
	virtual void drawRect(GL_Rect rect) = 0;
	virtual void initScene(GL_Color color = GL_Color::white) = 0;
	virtual void refreshScreen() = 0;
	virtual void makeCurrent() = 0;
	virtual void unmakeCurrent() = 0;
	virtual ~DrawingContextInterface() {}
};

class Event
{
	int label;
	id source;
	id dest;
	LONG64 time;
	LONG64 timestamp;

public:
	Event(int _label, id _source, id _dest, LONG64 _time, LONG64 _timestamp):
		label(_label), source(_source), dest(_dest), time(_time), timestamp(_timestamp) {}
	LONG64 getTimestamp() const {return timestamp;}
	int getLabel() const { return label; }
	id getDestId() const { return dest; }
	~Event() {}
};

class EventComparator
{
public:
	EventComparator() {}
	bool operator()(const Event& e1, const Event& e2) { return e1.getTimestamp() > e2.getTimestamp(); }
	bool operator()(const Event* e1, const Event* e2) { return e1->getTimestamp() > e2->getTimestamp(); }
	~EventComparator() {}
};

class KR_Object
{
public:
	virtual void wakeup() = 0;
	virtual bool isInitObj() = 0;
	virtual void recieveEvent(const Event* e) = 0;
	virtual id getId() = 0;
	virtual void setContext(ContextInterface *_context) = 0;
	virtual void stop() = 0;
};

class GL_DiskObj
{
public:
	virtual float getX() = 0;
	virtual float getY() = 0;
	virtual float getR() = 0;
	virtual bool isInitObj() = 0; 
};

class Rocket: public KR_Object, public GL_DiskObj
{
	const float PI;

	float x;
	float y;
	float velocity;
	float angle;
	float velX;
	float velY;
	int updateTime;

	float r;
	
	float minX;
	float minY;
	float maxX;
	float maxY;

	bool isInit;
	bool isStart;

	ContextInterface *context;
	DrawingContextInterface *dContext;

	void sendEvent(int label, id dest, LONG64 timeDiff) { 
		if (isStart) context->addEventFromObj(label, getId(), dest, timeDiff); }
	void setVelComponents(float angle, float velocity);
public:
	Rocket(float _r = 5.0): r(_r), isStart(false), isInit(false), angle(45.0f), PI(3.14159265359f) {}
	id getId() { return this; }
	void wakeup();
	void start();
	void stop() { isStart = false; }
	void recieveEvent(const Event *e);
	void setContext(ContextInterface *_context);
	void setAngle(float _angle) { angle = _angle; setVelComponents(angle, velocity); }
	void setLimitCoords(float _minX, float _minY, float _maxX, float _maxY) { minX=_minX; minY=_minY; maxX=_maxX; maxY=_maxY; }

	float getX() { return x; }
	float getY() { return y; }
	float getR() { return r; }

	float getAngle() { return angle; }
	bool isInitObj() { return isInit; }
	~Rocket() {}
};

class IncorrectDataException: public std::exception
{
	public:
		IncorrectDataException() : exception() {}
		IncorrectDataException(const char* const& msg) : exception(msg) {}
};

class NullPointerException: public std::exception
{
	public:
		NullPointerException() : exception() {}
		NullPointerException(const char* const& msg) : exception(msg) {}
};