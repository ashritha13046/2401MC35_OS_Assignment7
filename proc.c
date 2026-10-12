#define MAX_SEM 10
struct semaphore {
  int value;
  struct spinlock lk;
} semaphores[MAX_SEM];

int
sem_init(int sem_id, int value)
{
  if(sem_id < 0 || sem_id >= MAX_SEM)
    return -1;
  initlock(&semaphores[sem_id].lk, "semaphore");
  acquire(&semaphores[sem_id].lk);
  semaphores[sem_id].value = value;
  release(&semaphores[sem_id].lk);
  return 0;
}

int
sem_wait(int sem_id)
{
  if(sem_id < 0 || sem_id >= MAX_SEM)
    return -1;
  acquire(&semaphores[sem_id].lk);
  semaphores[sem_id].value--;
  if(semaphores[sem_id].value < 0){
    sleep(&semaphores[sem_id], &semaphores[sem_id].lk);
  }
  release(&semaphores[sem_id].lk);
  return 0;
}

int
sem_signal(int sem_id)
{
  if(sem_id < 0 || sem_id >= MAX_SEM)
    return -1;
  acquire(&semaphores[sem_id].lk);
  semaphores[sem_id].value++;
  if(semaphores[sem_id].value <= 0){
    wakeup(&semaphores[sem_id]);
  }
  release(&semaphores[sem_id].lk);
  return 0;
}

int
clone(void (*fcn)(void*), void *arg, void *stack)
{
  int i, pid;
  struct proc *np;
  
  if((np = allocproc()) == 0){
    return -1;
  }

  np->pgdir = proc->pgdir;
  np->sz = proc->sz;
  np->is_thread = 1;
  np->parent = proc;
  np->priority = 10;
  np->wait_ticks = 0;

  for(i = 0; i < NOFILE; i++)
    if(proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  safestrcpy(np->name, proc->name, sizeof(proc->name));
  pid = np->pid;

  np->tf = *proc->tf;
  uint *sp = (uint*)(stack + 4096);
  sp -= 2;
  sp[0] = 0xffffffff; 
  sp[1] = (uint)arg;
  
  np->tf->esp = (uint)sp;
  np->tf->eip = (uint)fcn;

  acquire(&ptable.lock);
  np->state = RUNNABLE;
  release(&ptable.lock);

  return pid;
}

int
join(void)
{
  struct proc *p;
  int havekids, pid;

  acquire(&ptable.lock);
  for(;;){
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != proc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        if(!p->is_thread)
          freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        p->is_thread = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    if(!havekids || proc->killed){
      release(&ptable.lock);
      return -1;
    }

    sleep(proc, &ptable.lock);
  }
}

void
set_priority(int pid, int priority)
{
  struct proc *p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->priority = priority;
      break;
    }
  }
  release(&ptable.lock);
}

int
get_priority(int pid)
{
  struct proc *p;
  int pri = -1;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      pri = p->priority;
      break;
    }
  }
  release(&ptable.lock);
  return pri;
}

// Replace your existing scheduler() function in proc.c with this implementation:
void
scheduler(void)
{
  struct proc *p;
  struct proc *best_p;

  for(;;){
    sti();
    acquire(&ptable.lock);

    best_p = 0;
    
    // Find RUNNABLE process with highest priority (lowest numerical value) and aging
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;
      
      if(!best_p || p->priority < best_p->priority){
        best_p = p;
      }
    }

    // Aging mechanism & Round-Robin tie-breaking simulation pass
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state == RUNNABLE){
        if(p != best_p){
          p->wait_ticks++;
          if(p->wait_ticks > 30 && p->priority > 1){
            p->priority--; // Boost priority by decrementing value
            p->wait_ticks = 0;
          }
        }
      }
    }

    if(best_p && best_p->state == RUNNABLE){
      best_p->wait_ticks = 0;
      proc = best_p;
      switchuvm(best_p);
      best_p->state = RUNNING;
      swtch(&cpu->scheduler, best_p->context);
      switchkvm();
      proc = 0;
    }
    
    release(&ptable.lock);
  }
}