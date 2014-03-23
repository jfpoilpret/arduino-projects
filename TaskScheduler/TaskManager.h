/*
 * TaskManager.h
 *
 *  Created on: 23 mars 2014
 *      Author: Jean-François Poilpret
 */

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include <Arduino.h>

#ifndef MAX_TASKS
#define MAX_TASKS 10
#endif

class TaskManager;

class Task
{
public:
	virtual ~Task() {}
	virtual void execute(int8_t id, uint32_t millis, TaskManager& manager) = 0;
};

class TaskConfig
{
public:
	TaskConfig(Task& task, uint32_t when = 0): task(&task), when(when), period(0), times(1) {}
	TaskConfig(Task& task, uint32_t when, uint16_t period, uint16_t times = 0)
		:task(&task), when(when), period(period), times(times) {}

private:
	TaskConfig(): task(0), when(0), period(0), times(0) {}

	Task* task;
	uint32_t when;
	uint16_t period;
	uint16_t times;

	friend class TaskManager;
};

class TaskManager
{
public:
	TaskManager():_maxTasks(MAX_TASKS) {}
	~TaskManager();

	int8_t addTask(const TaskConfig& config);
	int8_t updateTask(int8_t id, const TaskConfig& config);
	int8_t removeTask(int8_t id);

	uint32_t milliseconds();
	uint8_t runTasks();

private:
	struct RunnableTask
	{
		RunnableTask(int8_t id, Task* task):id(id), task(task) {}
		int8_t id;
		Task* task;
	};

	void initTimer();
	void launchIfNeeded();
	RunnableTask nextRunnableTask();

	const uint8_t _maxTasks;
	TaskConfig _tasks[MAX_TASKS];
	Task* _runnableTasks[MAX_TASKS];

	static RunnableTask NO_TASK;
	static TaskConfig EMPTY_TASK;

	friend void updateTime();
};

#endif /* TASKMANAGER_H_ */
