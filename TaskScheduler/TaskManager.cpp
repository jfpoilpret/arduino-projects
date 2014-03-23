/*
 * TaskManager.cpp
 *
 *  Created on: 23 mars 2014
 *      Author: Jean-François Poilpret
 */

#include <TaskManager.h>
#include <ClearInterrupt.h>

#define BIT(x) (0x01 << (x))

#define TIMER_FREQUENCY 1000
//TODO conditional compilation according to Arduino model
#define PRESCALING 128
#define TIMER_COUNTER (F_CPU / PRESCALING / TIMER_FREQUENCY - 1)

// Global variables used within TaskManager source code
static volatile uint32_t _milliseconds = 0;
static TaskManager* _taskManager = 0;

// Utility variables to optimize code
TaskConfig TaskManager::EMPTY_TASK;
TaskManager::RunnableTask TaskManager::NO_TASK(0, 0);

void TaskManager::initTimer()
{
	if (_taskManager == 0)
	{
		// Register this as singleton manager to be used by ISR
		_taskManager = this;
		// Use Timer2 with 1 ms interrupts
		// OC2A & OC2B disconnected, mode 2 (CTC, Clear Timer on Compare match)
		TCCR2A = BIT(WGM21);
		// Don't force output compare (FOCA & FOCB), mode 2, Clock Select clk/128 (CS = 5)
		TCCR2B = BIT(CS22) | BIT(CS20);
		// Set timer counter compare match (when value reached, 1ms has elapsed)
		OCR2A = TIMER_COUNTER;
		// Reset Timer2 counter
		TCNT2 = 0;
		// Set Timer2 interrupt mode (Set interrupt on OCR2A compare match)
		TIMSK2 = BIT(OCIE2A);
	}
}

TaskManager::~TaskManager()
{
	// Disable Timer2 interrupts
	TIMSK2 = 0;
}

int8_t TaskManager::addTask(TaskConfig& config)
{
	if (config.task == 0)
		return -1;
	ClearInterrupt();
	initTimer();
	if (_numTasks == _maxTasks)
		return -1;
	++_numTasks;
	for (int i = 0; i < _maxTasks; i++)
	{
		TaskConfig& entry = _tasks[i];
		if (entry.task == 0)
		{
			// First available task in tasks array, set it
			entry = config;
			entry.when += _milliseconds;
			return i;
		}
	}
	return -1;
}

int8_t TaskManager::updateTask(int8_t id, TaskConfig& config)
{
	if (id < 0 || id >= _maxTasks || config.task == 0)
		return -1;
	ClearInterrupt();
	TaskConfig& entry = _tasks[id];
	if (entry.task == config.task)
	{
		entry = config;
		entry.when += _milliseconds;
		return id;
	}
	return -1;
}

int8_t TaskManager::removeTask(int8_t id)
{
	if (id < 0 || id >= _maxTasks)
		return -1;
	ClearInterrupt();
	TaskConfig& entry = _tasks[id];
	if (entry.task != 0)
	{
		entry = EMPTY_TASK;
		return id;
	}
	return -1;
}

uint32_t TaskManager::milliseconds()
{
	ClearInterrupt();
	return _milliseconds;
}

uint8_t TaskManager::runTasks()
{
	uint8_t count = 0;
	while (true)
	{
		RunnableTask nextTask = nextRunnableTask();
		if (nextTask.task == 0)
			break;
		++count;
		nextTask.task->execute(nextTask.id, milliseconds(), *this);
	}
	return count;
}

TaskManager::RunnableTask TaskManager::nextRunnableTask()
{
	ClearInterrupt();
	for (uint8_t i = 0; i < _maxTasks; i++)
	{
		if (_runnableTasks[i] != 0)
		 {
			Task* task = _runnableTasks[i];
			_runnableTasks[i] = 0;
			return RunnableTask(i, task);
		}
	}
	return NO_TASK;
}

void TaskManager::launchIfNeeded()
{
	for (uint8_t i = 0; i < _maxTasks; i++)
	{
		TaskConfig& entry = _tasks[i];
		_runnableTasks[i] = 0;
		if (entry.task != 0)
		{
			if (entry.when <= _milliseconds)
			{
				// Add this task to executable tasks
				_runnableTasks[i] = entry.task;
				// Remove task if one-shot or last shot
				if (entry.period == 0 || entry.times == 1)
				{
					entry = EMPTY_TASK;
					--_numTasks;
				}
				else
				{
					entry.when += entry.period;
					if (entry.times != 0)
						--entry.times;
				}
			}
		}
	}
}

void updateTime()
{
	_taskManager->launchIfNeeded();
}

// Attach interrupt routine to the Timer Compare Interrupt
ISR(TIMER2_COMPA_vect)
{
	_milliseconds++;
	updateTime();
}
