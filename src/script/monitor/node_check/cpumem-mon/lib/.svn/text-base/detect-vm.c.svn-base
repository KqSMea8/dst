/*
 *run detect-vm  全虚的虚拟机：输出 HVM Virtual Machine.
 *             半虚的虚拟机：输出　PV Virtual Machine.
 *             宿主机        输出  PV Virtual Machine.
 *判断是半虚还是宿主机： cat /proc/xen/capabilities    宿主机输出为 control_d， 半虚为空
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int pv_context;

static void cpuid(uint32_t idx,
                  uint32_t *eax,
                  uint32_t *ebx,
                  uint32_t *ecx,
                  uint32_t *edx)
{
    asm volatile (
        "test %1,%1 ; jz 1f ; ud2a ; .ascii \"xen\" ; 1: cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (idx), "1" (pv_context) );
}

static int check_for_xen(void)
{
    uint32_t eax, ebx, ecx, edx;
    char signature[13];

    cpuid(0x40000000, &eax, &ebx, &ecx, &edx);
    *(uint32_t *)(signature + 0) = ebx;
    *(uint32_t *)(signature + 4) = ecx;
    *(uint32_t *)(signature + 8) = edx;
    signature[12] = '\0';

    if ( strcmp("XenVMMXenVMM", signature) || (eax < 0x40000002) )
        return 0;

    cpuid(0x40000001, &eax, &ebx, &ecx, &edx);
    printf(" %s Virtual Machine.\n",
           pv_context ? "PV" : "HVM");
    //system("cat /proc/xen/capabilities");
    return 1;
}

void s_handler(int act)
{
    printf("Not running on Xen.\n");
}

int main(void)
{
    pid_t pid;
    int status;
    uint32_t dummy;
    struct sigaction act, oldact;

    act.sa_handler = s_handler;
    act.sa_flags = SA_ONESHOT | SA_NOMASK;
    sigaction(SIGILL, &act, &oldact);

    /* Check for execution in HVM context. */
    if ( check_for_xen() )
        return 0;

    /* Now we check for execution in PV context. */
    pv_context = 1;

    /*
     * Fork a child to test the paravirtualised CPUID instruction.
     * If executed outside Xen PV context, the extended opcode will fault.
     */
    pid = fork();
    switch ( pid )
    {
    case 0:
        /* Child: test paravirtualised CPUID opcode and then exit cleanly. */
        cpuid(0x40000000, &dummy, &dummy, &dummy, &dummy);
        exit(0);
    case -1:
        fprintf(stderr, "Fork failed.\n");
        return 0;
    }

    /*
     * Parent waits for child to terminate and checks for clean exit.
     * Only if the exit is clean is it safe for us to try the extended CPUID.
     */
    waitpid(pid, &status, 0);
    if ( WIFEXITED(status) && check_for_xen() )
        return 0;

    printf("Not running on Xen.\n");
    return 0;
}

