/*
 * gcc syscalls.c -lvmi -o syscalls
 */
#include <signal.h>
#include <libvmi/libvmi.h>
#include <libvmi/events.h>

#include "syscalls.h"

static uint8_t trap_instruction = 0xCC;
static uint8_t backup_byte[NUMBER_OF_SYSCALLS];

/* Signal handler */
static struct sigaction act;
static int interrupted = 0;
static void close_handler(int sig){
    interrupted = sig;
}

event_response_t singlestep_event(vmi_instance_t vmi, vmi_event_t *event) {
	// TODO 6: write trap and disable singlestepping
}

event_response_t trap_event(vmi_instance_t vmi, vmi_event_t *event) {

	int i;
	for (i=0; i < NUMBER_OF_SYSCALLS; i++)
		if ( syscalls[i].addr == event->interrupt_event.gla )
			break;

	printf("Received a trap event for syscall %s!\n", syscalls[i].name);

	// TODO 4: replace trap with backup byte and specify reinjecting behavior
	// TODO 5: turn on singlestepping
}

void main(int argc, char **argv) {

	/* Signal handler to catch CTRL+C, etc.. */
	act.sa_handler = close_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGHUP,  &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT,  &act, NULL);
	sigaction(SIGALRM, &act, NULL);

	// TODO 1: initialize LibVMI

	int i;
	for (i=0; i < NUMBER_OF_SYSCALLS; i++) {
		printf("%s at 0x%lx\n", syscalls[i].name, syscalls[i].addr);

        // TODO 2: backup first byte of a syscall and write trap (sys_gettimeofday is a good candidate)
	}

	// TODO 3: enable listening for interrupt and singlestep events and start events loop
	// TODO 4: remove traps and close LibVMI
}
