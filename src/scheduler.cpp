#include "../h/scheduler.hpp"

List<TCB> Scheduler::threadQueue;

void Scheduler::put(TCB *tcb) {
    threadQueue.addLast( tcb );
}

TCB *Scheduler::get() {
    return threadQueue.getFirst();
}
