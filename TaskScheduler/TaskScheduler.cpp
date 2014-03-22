// Do not remove the include below
#include "TaskScheduler.h"

#define MAX_TASKS 5
#define MAX_TIMED_TASKS 2
#include <TimedTaskGenerator.h>

#define MAX_DELAY 2000
#define MIN_DELAY 100
#define NUM_LOOPS 10

class MyTask: public Task
{
public:
	MyTask(TimedTaskGenerator& timedTaskGenerator, uint8_t pin = 13):
		_pin(pin),
		_state(LOW),
		_timedTaskGenerator(timedTaskGenerator),
		_nextTime(MAX_DELAY),
		_loop(NUM_LOOPS)
	{
		pinMode(_pin, OUTPUT);
	}
	~MyTask() {}

	void execute(TaskManager& manager)
	{
		_state = (_state == LOW ? HIGH : LOW);
		digitalWrite(_pin, _state);
		_timedTaskGenerator.addOneShotTask(this, _nextTime);
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
	TimedTaskGenerator& _timedTaskGenerator;
	uint32_t _nextTime;
	uint8_t _loop;
};

TaskManager scheduler;
TimedTaskGenerator timedTasks(scheduler, MAX_TIMED_TASKS);
MyTask myTask(timedTasks);

//The setup function is called once at startup of the sketch
void setup()
{
//	timedTasks.addPeriodicTask(&myTask, 10000, 0);
	timedTasks.addOneShotTask(&myTask, 0);
}

// The loop function is called in an endless loop
void loop()
{
	scheduler.runAll();
}
