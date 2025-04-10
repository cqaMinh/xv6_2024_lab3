#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgpte(void)
{
  uint64 va;
  struct proc *p;  

  p = myproc();
  argaddr(0, &va);
  pte_t *pte = pgpte(p->pagetable, va);
  if(pte != 0) {
      return (uint64) *pte;
  }
  return 0;
}
#endif

#ifdef LAB_PGTBL
int
sys_kpgtbl(void)
{
  struct proc *p;  

  p = myproc();
  vmprint(p->pagetable);
  return 0;
}
#endif


uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_pgaccess(void)
{
  uint64 addr;        // địa chỉ ảo bắt đầu
  int numpages;       // số trang
  uint64 user_mask;   // user-space buffer để ghi kết quả

  argaddr(0, &addr);        // địa chỉ ảo bắt đầu
  argint(1, &numpages);     // số trang
  argaddr(2, &user_mask);   // địa chỉ buffer trong user-space

  if (numpages > 64) // giới hạn tối đa 64 pages (tương ứng 64 bit)
    return -1;

  struct proc *p = myproc();
  uint64 mask = 0;

  for (int i = 0; i < numpages; i++) {
    pte_t *pte = walk(p->pagetable, addr + i * PGSIZE, 0);
    if (pte == 0)
      continue;
    if (*pte & PTE_A) {
      mask |= (1L << i);       // đánh dấu bit tương ứng
      *pte &= ~PTE_A;          // xóa bit PTE_A sau khi đọc
    }
  }

  // Copy bitmask về userspace
  if (copyout(p->pagetable, user_mask, (char *)&mask, sizeof(mask)) < 0)
    return -1;

  return 0;
}
