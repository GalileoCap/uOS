#include <except.h>

const char* numberToString[] = {
  "#DE", "#DB", "#--", "#BP", "#OF", "#BR", "#UD", "#NM",
  "#DF", "#--", "#TS", "#NP", "#SS", "#GP", "#PF", "#--",
  "#MF", "#AC", "#MC", "#XM#XF", "#VE", "#CP", "#--", "#--",
  "#--", "#--", "#--", "#--", "#HV", "#VC", "#SX", "#--",
  "#--", "#FERR"
};

void printException(const struct except *exception) {
  printf("\t---EXCEPTION CAUGHT (%s)---\n", numberToString[exception->number]);
  printf("\t* number=%X, code=%X, ptr=%p\n", exception->number, exception->code, exception);
  printf("\t* rax=%X, rbx=%X, rcx=%X, rdx=%X\n", exception->rax, exception->rbx, exception->rcx, exception->rdx);
  printf("\t* rsp=%p, rbp=%X, rsi=%X, rdi=%X\n", exception->rsp, exception->rbp, exception->rsi, exception->rdi);
  printf("\t* r8=%X, r9=%X, r10=%X, r11=%X\n", exception->r8, exception->r9, exception->r10, exception->r11);
  printf("\t* r12=%X, r13=%X, r14=%X, r15=%X\n", exception->r12, exception->r13, exception->r14, exception->r15);
  printf("\t* rflags=%X, rip=%p\n", exception->rflags, exception->rip);
  printf("\t---END OF EXCEPTION---\n");
}

void exception_handler(const struct except *exception) {
  printException(exception);
  panic("[exception_handler] TODO: Handle exception\n");
}
