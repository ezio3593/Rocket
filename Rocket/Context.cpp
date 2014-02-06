#include "stdafx.h"
#include "Context.h"

Timer::Timer()
{
	isSupported = QueryPerformanceFrequency(&frequency);
	startTime = 0;
}

LONG64 Timer::getSysUptime()
{
	if (isSupported)
	{
		LARGE_INTEGER time;
        QueryPerformanceCounter(&time);
        return (1000LL * time.QuadPart) / frequency.QuadPart;
	} else return -1;
}

unsigned int Context::threadProcess(void *p)
{
	Context* c = reinterpret_cast<Context*>(p);
	
	if (c->dContext)
	{
		c->dContext->makeCurrent();
		c->processEvents();
		c->dContext->unmakeCurrent();
	} else c->processEvents();

	return 0;
}

Context::Context()
{
	objPool = new ObjectPool();
	eventPool = new EventPool();

	isStart = false;
	dContext = NULL;
	threadHandle = NULL;
}

int Context::addObject(KR_Object* obj)
{
	Lock l(objPoolCs);

	if (obj && objPool->find(obj->getId()) == objPool->end())
	{
		objPool->insert(std::pair<id, KR_Object*>(obj->getId(), obj));
		if (!obj->isInitObj()) 
		{
			obj->setContext(this);
			obj->wakeup();
		}
		return 0;
	} else return -1;
}

int Context::deleteObjectById(id objId)
{
	Lock l(cs);
	
	KR_Object* obj = (*objPool)[objId];
	if (obj) 
	{
		objPool->erase(objId);
		return 0;
	} else return -1;
}

void Context::processEvents()
{
	Event* e = NULL;

	int k = 0;

	bool flag;

	{
		Lock l(cs);

		flag = isStart;
	}


	while (flag)
	{

		{
			Lock l(cs);

			if (eventPool->size())
			{
				e = eventPool->top();

				if (timer.getTimeFromStart() >= e->getTimestamp())
				{

					dContext->redrawScene();

					id idNum = e->getDestId();
					
					KR_Object* obj = NULL;

					{
						Lock objL(objPoolCs);
						obj = (*objPool)[idNum];
					}
				
					obj->recieveEvent(e);
					
					delete e;
					eventPool->pop();

				}
			}
		}

		{
			Lock l(cs);
			flag = isStart;
		}
	}
}

int Context::addEvent(Event* e)
{
	if (e)
	{
		eventPool->push(e);
		return 0;
	} else return -1;
}

void Context::addEventFromObj(int label, id source, id dest, LONG64 timeDiff)
{
	Lock l(cs);

	time_t t = timer.getTimeFromStart();
	addEvent(new Event(label, source, dest, t, t + timeDiff));
}

void Context::start()
{

	if (!isStart)
	{
		isStart = true;

		if (!timer.isSupportPerf()) throw ContextException("Perfomance counter isn't supported");

		timer.start();

		if (dContext)
		{
			dContext->unmakeCurrent();
			threadHandle = (HANDLE)_beginthreadex(0, 0, threadProcess, reinterpret_cast<void*>(this), 0, NULL);
		} else
			threadHandle = (HANDLE)_beginthreadex(0, 0, threadProcess, reinterpret_cast<void*>(this), 0, NULL);

		if (!threadHandle) throw ContextException("Cannot create thread");
	}
}

void Context::stop()
{
	if (isStart)
	{
		{
			Lock l(cs);
			
			isStart = false;

			for (ObjectPool::iterator i = objPool->begin(); i != objPool->end(); ++i)
				i->second->stop();
		}

		if (threadHandle)
		{
			WaitForSingleObject(threadHandle, INFINITE);
			threadHandle = NULL;
		}

		while (eventPool->size())
		{
			delete eventPool->top();
			eventPool->pop();
		}
	}
}

int Context::setDrawingContext(DrawingContextInterface* _dContext)
{
	if (!isStart)
	{
		dContext = _dContext;
		return 0;
	} else return -1;
}

Context::~Context()
{
	stop();
	
	if (threadHandle)
	{
		WaitForSingleObject(threadHandle, INFINITE);
		CloseHandle(threadHandle);
	}

	delete objPool;

	while (eventPool->size())
	{
		delete eventPool->top();
		eventPool->pop();
	}

	delete eventPool;
}