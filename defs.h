// proc.c
int             clone(void(*fcn)(void*), void *arg, void *stack);
int             join(void);
void            set_priority(int pid, int priority);
int             get_priority(int pid);
int             sem_init(int sem_id, int value);
int             sem_wait(int sem_id);
int             sem_signal(int sem_id);