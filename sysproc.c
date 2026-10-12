int
sys_clone(void)
{
  void (*fcn)(void*);
  void *arg;
  void *stack;

  if(argptr(0, (char**)&fcn, sizeof(void*)) < 0 ||
     argptr(1, (char**)&arg, sizeof(void*)) < 0 ||
     argptr(2, (char**)&stack, sizeof(void*)) < 0)
    return -1;

  return clone(fcn, arg, stack);
}

int
sys_join(void)
{
  return join();
}

int
sys_sem_init(void)
{
  int sem_id, value;
  if(argint(0, &sem_id) < 0 || argint(1, &value) < 0)
    return -1;
  return sem_init(sem_id, value);
}

int
sys_sem_wait(void)
{
  int sem_id;
  if(argint(0, &sem_id) < 0)
    return -1;
  return sem_wait(sem_id);
}

int
sys_sem_signal(void)
{
  int sem_id;
  if(argint(0, &sem_id) < 0)
    return -1;
  return sem_signal(sem_id);
}

int
sys_set_priority(void)
{
  int pid, priority;
  if(argint(0, &pid) < 0 || argint(1, &priority) < 0)
    return -1;
  set_priority(pid, priority);
  return 0;
}

int
sys_get_priority(void)
{
  int pid;
  if(argint(0, &pid) < 0)
    return -1;
  return get_priority(pid);
}