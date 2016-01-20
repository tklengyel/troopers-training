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

static addr_t reset_addr;

event_response_t singlestep_cb(vmi_instance_t vmi, vmi_event_t *event) {
	// TODO 6: write trap and disable singlestepping
	vmi_write_8_va(vmi, reset_addr, 0, &trap_instruction);
	return (1u << VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP);
}

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event) {

	int i;
	for (i=0; i < NUMBER_OF_SYSCALLS; i++)
		if ( syscalls[i].addr == event->interrupt_event.gla )
			break;

	printf("Received a trap event for syscall %s!\n", syscalls[i].name);

	// TODO 4: replace trap with backup byte
	vmi_write_8_va(vmi, syscalls[i].addr, 0, &backup_byte[i]);
	reset_addr = syscalls[i].addr;

	event->interrupt_event.reinject = 0;

	// TODO 5: turn on singlestepping
	return (1u << VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP);
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

	vmi_instance_t vmi;
	// Initialize the libvmi library.
	if (vmi_init(&vmi, VMI_XEN | VMI_INIT_COMPLETE | VMI_INIT_EVENTS, argv[1]) == VMI_FAILURE) {
		printf("Failed to init LibVMI library.\n");
		return;
	}

	vmi_pause_vm(vmi);

	int i;
	for (i=0; i < NUMBER_OF_SYSCALLS; i++) {
		printf("%s at 0x%lx\n", syscalls[i].name, syscalls[i].addr);

		// TODO 2: backup first byte of a syscall and write trap
		if(!strcmp("sys_gettimeofday", syscalls[i].name)) {
			vmi_read_8_va(vmi, syscalls[i].addr, 0, &backup_byte[i]);
			vmi_write_8_va(vmi, syscalls[i].addr, 0, &trap_instruction);
		}
	}

	// TODO 3: enable listening for interrupt and singlestep events and start events loop
	vmi_event_t trap_event, singlestep_event;
	SETUP_INTERRUPT_EVENT(&trap_event, 0, trap_cb);
	trap_event.interrupt_event.intr = INT3;
	SETUP_SINGLESTEP_EVENT(&singlestep_event, 1, singlestep_cb, 0);
	vmi_register_event(vmi, &trap_event);
	vmi_register_event(vmi, &singlestep_event);

	vmi_resume_vm(vmi);
	while(!interrupted){
	        status_t status = vmi_events_listen(vmi,500);
        	if (status != VMI_SUCCESS) {
            		printf("Error waiting for events, quitting...\n");
            		interrupted = -1;
        	}
	}

    // TODO 4: remove traps and close LibVMI
	for (i=0; i < NUMBER_OF_SYSCALLS; i++) {
			vmi_write_8_va(vmi, syscalls[i].addr, 0, &backup_byte[i]);
    }

	vmi_destroy(vmi);
}
