/*
 * TimedTaskGenerator.cpp
 *
 *  Created on: 15 mars 2014
 *      Author: Jean-François
 */

#include <Arduino.h>

#include <TimedTaskGenerator.h>
#include <ClearInterrupt.h>

#define BIT(x) (0x01 << (x))

#define TIMER_FREQUENCY 1000
#define PRESCALING 128
#define TIMER_COUNTER (F_CPU / PRESCALING / TIMER_FREQUENCY - 1)

static volatile uint32_t _milliseconds = 0;
static TimedTaskGenerator* _timedTaskGenerator = 0;

void TimedTaskGenerator::initTimer()
{
	if (_timedTaskGenerator == 0)
	{
		// Register this as singleton manager to be used by ISR
		_timedTaskGenerator = this;
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

TimedTaskGenerator::~TimedTaskGenerator()
{
	// Disable interrupts
	TIMSK2 = 0;
}

void TimedTaskGenerator::addOneShotTask(Task* task, uint32_t when)
{
	addPeriodicTask(task, 0, when);
}

void TimedTaskGenerator::addPeriodicTask(Task* task, uint16_t period, uint32_t when)
{
	ClearInterrupt();
	initTimer();
	if (_numTasks == _maxTasks)
		return;
	++_numTasks;
	for (int i = 0; i < _maxTasks; i++)
	{
		volatile TimedTask& entry = _tasks[i];
		if (entry.task == 0)
		{
			// First available task in tasks array, set it
			entry.task = task;
			entry.when = _milliseconds + when;
			entry.period = period;
			return;
		}
	}
}

void TimedTaskGenerator::removeTask(Task* task)
{
	if (task == 0)
		return;
	ClearInterrupt();
	if (_maxTasks == 0)
		return;
	for (int i = 0; i < _maxTasks; i++)
	{
		volatile TimedTask& entry = _tasks[i];
		if (entry.task == task)
		{
			entry.task = 0;
			--_numTasks;
		}
	}
}

//TODO Optimization needed: find the next time one task must run
void TimedTaskGenerator::launchIfNeeded(uint32_t ms)
{
	for (uint8_t i = 0; i < _maxTasks; i++)
	{
		volatile TimedTask& entry = _tasks[i];
		if (entry.task != 0)
		{
			if (entry.when <= ms)
			{
				_taskManager.pushTaskImmediate(entry.task);
				if (entry.period == 0)
				{
					entry.task = 0;
					--_numTasks;
				}
				else
					entry.when += entry.period;
			}
		}
	}
}

void updateTime(uint32_t ms)
{
	_timedTaskGenerator->launchIfNeeded(ms);
}

// Attach interrupt routine to the Timer Compare Interrupt
ISR(TIMER2_COMPA_vect)
{
	_milliseconds++;
	updateTime(_milliseconds);
}
