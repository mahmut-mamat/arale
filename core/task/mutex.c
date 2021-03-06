/**************************************************************
 CopyRight     :No
 FileName      :mutex.c
 Author        :Sunli.Wang
 Version       :0.01
 Date          :20171010
 Description   :mutex lock operations
***************************************************************/

#include "mutex.h"
#include "list.h"
#include "klibc.h"
#include "atomic.h"
#include "task.h"
#include "log.h"

/*----------------------------------------------
                public method
----------------------------------------------*/
public mutex *create_mutex()
{
    mutex *lock = (mutex *)kmalloc(sizeof(mutex));
    kmemset(lock,0,sizeof(mutex));
    INIT_LIST_HEAD(&lock->wait_list);
    SPIN_LOCK_INIT(&lock->spin_lock);
    lock->held_pid = -1;
    return lock;
}

public void acquire_lock(mutex *lock)
{
    if(lock == NULL)
    {
        LOGD("acquire null lock \n");
    }

    spin_lock(&lock->spin_lock);
    task_struct *current = GET_CURRENT_TASK();
    if(lock->held_pid == current->pid)
    {
        return;
    }

    if(lock->status == MUTEX_IDLE)
    {
        lock->status = MUTEX_HELD;
        lock->held_pid = current->pid;
        spin_unlock(&lock->spin_lock);
    }
    else
    {
        list_add(&current->lock_ll,&lock->wait_list);
        spin_unlock(&lock->spin_lock);
        task_sleep(current);
    }
}

public void release_lock(mutex *lock)
{
    //LOGD("mutex release lock \n");
    spin_lock(&lock->spin_lock);
    task_struct *current = GET_CURRENT_TASK();
    if(lock->status == MUTEX_IDLE)
    {
        LOGE("double free lock \n");
        spin_unlock(&lock->spin_lock);
        return ;
    }

    if(!list_empty(&lock->wait_list))
    {
        //LOGD("mutex release lock list_empty\n");
        struct list_head *p = lock->wait_list.next;
        task_struct *task = list_entry(p,task_struct,lock_ll);
        list_del(p);
        lock->held_pid = task->pid;
        spin_unlock(&lock->spin_lock);
        task_wake_up(task);
        return;
    }

    //LOGD("mutex release lock list_empty\n");
    lock->status = MUTEX_IDLE;
    lock->held_pid = -1;
    spin_unlock(&lock->spin_lock);
}

public void free_lock(mutex *lock)
{
    if(lock->status  != MUTEX_IDLE)
    {
        acquire_lock(lock);
    }

    free(lock);
}
