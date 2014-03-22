/*
 * TaskManager.cpp
 *
 *  Created on: 15 mars 2014
 *      Author: Jean-François Poilprêt
 */

#include <TaskManager.h>
#include <ClearInterrupt.h>

void TaskManager::pushTaskImmediate(Task* task)
{
	ClearInterrupt();
	if (_numTasks < _maxTasks)
	{
		_head = (_head - 1) % _maxTasks;
		_tasks[_head] = task;
		_numTasks++;
	}
}

void TaskManager::pushTask(Task* task)
{
	ClearInterrupt();
	if (_numTasks < _maxTasks)
	{
		_tasks[(_head +_numTasks) % _maxTasks] = task;
		_numTasks++;
	}
}

int TaskManager::runAll()
{
	int num = 0;
	while (runOne()) num++;
	return num;
}

bool TaskManager::runOne()
{
	Task* task = pull();
	if (task == 0) return false;
	task->execute(*this);
	return true;
}

Task* TaskManager::pull()
{
	ClearInterrupt();
	if (_numTasks == 0) return 0;
	Task* task = _tasks[_head];
	_head = (_head + 1) % _maxTasks;
	_numTasks--;
	return task;
}
