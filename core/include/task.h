#include "vmm.h"
#include "mm.h"
#include "list.h"

#ifndef _TASK_STRUCT_H_
#define _TASK_STRUCT_H_

#define TASK_GDT0 3 //because the first 2 GDT is defined in stage2.asm
#define TASK_MAX 2048

#define PN_MAX_LEN 16

#define TASK_MAX_PITS 100

#define DEFAULT_TASK_TICKS 5000

enum task_status_type {
    TASK_STATUS_NONE = 0,
    TASK_STATUS_INIT,
    TASK_STATUS_RUNNABLE,
    TASK_STATUS_RUNNING,
    TASK_STATUS_WAIT, //reset ticks
    TASK_STATUS_SLEEPING,
    TASK_STATUS_DESTROY
};

typedef struct _tss_struct_ {
    uint32_t ts_link;    // Old ts selector
    uint32_t ts_esp0;    // Stack pointers and segment selectors
    uint16_t ts_ss0;    //   after an increase in privilege level
    uint16_t ts_padding1;
    uint32_t ts_esp1;
    uint16_t ts_ss1;
    uint16_t ts_padding2;
    uint32_t ts_esp2;
    uint16_t ts_ss2;
    uint16_t ts_padding3;
    uint32_t ts_cr3;    // Page directory base
    uint32_t ts_eip;    // Saved state from last task switch
    uint32_t ts_eflags;
    uint32_t ts_eax;    // More saved state (registers)
    uint32_t ts_ecx;
    uint32_t ts_edx;
    uint32_t ts_ebx;
    uint32_t ts_esp;
    uint32_t ts_ebp;
    uint32_t ts_esi;
    uint32_t ts_edi;
    uint16_t ts_es;        // Even more saved state (segment selectors)
    uint16_t ts_padding4;
    uint16_t ts_cs;
    uint16_t ts_padding5;
    uint16_t ts_ss;
    uint16_t ts_padding6;
    uint16_t ts_ds;
    uint16_t ts_padding7;
    uint16_t ts_fs;
    uint16_t ts_padding8;
    uint16_t ts_gs;
    uint16_t ts_padding9;
    uint16_t ts_ldt;
    uint16_t ts_padding10;
    uint16_t ts_t;        // Trap on task switch
    uint16_t ts_iomb;    // I/O map base address
}tss_struct;

typedef struct context{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
}context_struct;

typedef void (*task_entry_fun)(void *);

typedef struct _task_struct_ {
    uint32_t pid;

    mm_struct *mm;
    //tss_struct *tss;
    context_struct *context;

    uint32_t prio;
    char name[PN_MAX_LEN];

    struct _task_struct_ *parent;
    uint32_t ticks;
    uint32_t elapsed_ticks;
    uint32_t remainder_ticks;
    int16_t status;

    struct list_head rq_ll;
    int sleep_ticks;

    task_entry_fun _entry;
    void * _entry_data;

}task_struct;

typedef struct task_queue_group {
    struct list_head runningq;
    struct list_head runnableq;
    struct list_head sleepingq;
    struct list_head waitq;
}task_queue_group;

//task_struct task_table[TASK_MAX];
task_struct *current_task; //the default pid is 0

task_struct *GET_CURRENT_TASK();
void task_init(struct boot_info *binfo);
task_struct* task_alloc();

void (*task_runnable)(void *args);
task_struct* task_create(void* runnable);
int task_start(task_struct *task);
void wake_up_task(task_struct *task);
void dormant_task(task_struct *task);

#endif
