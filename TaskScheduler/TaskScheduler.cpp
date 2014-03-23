// Do not remove the include below
#include "TaskScheduler.h"

#define MAX_TASKS 5
#include <TaskManager.h>

#define MAX_DELAY 2000
#define MIN_DELAY 100
#define NUM_LOOPS 10

class MyTask: public Task
{
public:
	MyTask(uint8_t pin = 13):
		_pin(pin),
		_state(LOW),
		_nextTime(MAX_DELAY),
		_loop(NUM_LOOPS)
	{
		pinMode(_pin, OUTPUT);
	}
	~MyTask() {}

	void execute(int8_t id, uint32_t millis, TaskManager& manager)
	{
		_state = (_state == LOW ? HIGH : LOW);
		digitalWrite(_pin, _state);
		manager.addTask(TaskConfig(*this, _nextTime));
		--_loop;
		if (_loop == 0)
		{
			_loop = NUM_LOOPS;
			_nextTime /= 2;
			if (_nextTime < MIN_DELAY)
				_nextTime = MAX_DELAY;
		}
	}

private:
	uint8_t _pin;
	uint8_t _state;
	uint32_t _nextTime;
	uint8_t _loop;
};

TaskManager scheduler;
MyTask myTask;

//The setup function is called once at startup of the sketch
void setup()
{
	scheduler.addTask(TaskConfig(myTask));
}

// The loop function is called in an endless loop
void loop()
{
	scheduler.runTasks();
}
