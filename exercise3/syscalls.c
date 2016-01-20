/*
 * Copyright (C) 2016 Tamas K Lengyel
 * ACADEMIC PUBLIC LICENSE
 * For details please read the LICENSE file.
 */

/*
 * gcc syscalls.c -lvmi -o syscalls
 */

/*
 * In this exercise the goal is to write a software breakpoint instruction
 * into the beginning a Linux system call handler. Software breakpoints
 * can be configured to trap to the hypervisor and thus can be hidden
 * form the executing guest.
 * When we receive the breakpoint event, we can remove it, place back
 * the original instruction we overwrote so the guest continues as if nothing
 * happened. Further enabling singlestepping will allow you to re-trap the
 * system call handler.
 */

#include <signal.h>
#include <libvmi/libvmi.h>
#include <libvmi/events.h>

#include "syscalls.h"

static uint8_t trap = 0xCC;
static uint8_t backup_byte[NUMBER_OF_SYSCALLS];
static int i;

/* Signal handler */
static struct sigaction act;
static int interrupted = 0;
static void close_handler(int sig){
    interrupted = sig;
}

event_response_t singlestep_event(vmi_instance_t vmi, vmi_event_t *event) {
    // TODO 9: write trap again and disable singlestepping
    event_response_t response = 0; // Change response value to toggle singlestep
}

event_response_t trap_event(vmi_instance_t vmi, vmi_event_t *event) {

    printf("Received a trap event for syscall %s!\n", syscalls[i].name);

    // TODO 7: replace trap with backup byte and specify reinjection behavior
    // TODO 8: turn on singlestepping
    event_response_t response = 0; // Change response value to toggle singlestep
    return response;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s <domain name> <syscall_name>\n", argv[0]);
        return 1;
    }

    const char *name = argv[1];
    const char *syscall = argv[2];

    /* Signal handler to catch CTRL+C, etc.. */
    act.sa_handler = close_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGHUP,  &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGALRM, &act, NULL);

    // TODO 1: initialize LibVMI

    for (i=0; i < NUMBER_OF_SYSCALLS; i++) {
        //printf("%s at 0x%lx\n", syscalls[i].name, syscalls[i].addr);

        if(!strcmp(syscalls[i].name, syscall)) {
            printf("Syscall %s is located at VA 0x%lx\n", syscall, syscalls[i].addr);
            // TODO 2: backup first byte of a syscall and write trap
            //         You can back up the first byte into backup_byte[i].
            break;
        }
    }

    // TODO 3: enable listening for interrupt events
    // TODO 4: enable listening for singlestep events BUT don't yet turn on singlestepping
    // TODO 5: start events loop
    // TODO 6: remove traps and close LibVMI
    return 0;
}
