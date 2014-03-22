/*
 * TaskManager.h
 *
 *  Created on: 15 mars 2014
 *      Author: Jean-François Poilprêt
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
	virtual void execute(TaskManager& manager) = 0;
};

class TaskManager
{
public:
	TaskManager():_maxTasks(MAX_TASKS), _head(0), _numTasks(0) {}
	void pushTaskImmediate(Task* task);
	void pushTask(Task* task);
	int runAll();
	bool runOne();

private:
	Task* pull();

	const uint8_t _maxTasks;
	volatile uint8_t _head;
	volatile uint8_t _numTasks;
	Task* volatile _tasks[MAX_TASKS];
};

#endif /* TASKMANAGER_H_ */
