// Per-process state
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall/interrupt
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, has been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  
  // --- Lab 7 Extensions ---
  int priority;                // Q4: Priority (1 = highest, 20 = lowest)[cite: 3]
  int wait_ticks;              // Q4: Aging wait counter[cite: 3]
  int is_thread;               // Q1: Flag indicating a user-level thread[cite: 1]
  void *ustack;                // Q1: User stack pointer reference
};