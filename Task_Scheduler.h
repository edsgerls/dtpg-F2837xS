/*
 * Task_Scheduler.h
 *
 *  Created on: Jun 28, 2022
 *      Author: edgar
 */

#ifndef TASK_SCHEDULER_H_
#define TASK_SCHEDULER_H_

#include <stdint.h>

typedef void (*task_ptr_T)(void);

typedef struct Task_Scheduler_Tag
{
    task_ptr_T task_entry_point;
    uint16_t   task_period;
    uint16_t   task_offset;
    bool       task_running;
}Task_Scheduler_T;




#endif /* TASK_SCHEDULER_H_ */
