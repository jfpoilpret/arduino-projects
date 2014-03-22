/*
 * TimedTaskGenerator.h
 *
 *  Created on: 15 mars 2014
 *      Author: Jean-François Poilprêt
 */

#ifndef TIMEDTASKGENERATOR_H_
#define TIMEDTASKGENERATOR_H_

#include <Arduino.h>
#include <TaskManager.h>

#ifndef MAX_TIMED_TASKS
#define MAX_TIMED_TASKS 5
#endif

class TimedTaskGenerator
{
public:
	TimedTaskGenerator(TaskManager& taskManager, uint8_t maxTasks)
		:	_taskManager(taskManager),
		 	_maxTasks(MAX_TIMED_TASKS),
		 	_numTasks(0),
		 	_nextTime(0xFFFFFFFF)
	{
	}
	~TimedTaskGenerator();

	//TODO additional API: multi-shot task
	void addOneShotTask(Task* task, uint32_t when);
	void addPeriodicTask(Task* task, uint16_t period, uint32_t when = 0);
	void removeTask(Task* task);

private:
	struct TimedTask
	{
		Task* task;
		uint32_t when;
		uint16_t period;
	};

private:
	void initTimer();
	void launchIfNeeded(uint32_t current);

	TaskManager& _taskManager;
	const uint8_t _maxTasks;
	volatile uint8_t _numTasks;
	volatile TimedTask _tasks[MAX_TIMED_TASKS];
	volatile uint32_t _nextTime;

	friend void updateTime(uint32_t);
};

#endif /* TIMEDTASKGENERATOR_H_ */
