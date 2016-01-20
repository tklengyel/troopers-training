/*
 * Copyright (C) 2016 Tamas K Lengyel
 * ACADEMIC PUBLIC LICENSE
 * For details please read the LICENSE file.
 */

/*
 * gcc syscalls.c -lvmi -o syscalls
 */
#include <signal.h>
#include <libvmi/libvmi.h>
#include <libvmi/events.h>

#include "syscalls.h"

static uint8_t trap = 0xCC;
static uint8_t backup_byte;
static int i;

/* Signal handler */
static struct sigaction act;
static int interrupted = 0;
static void close_handler(int sig){
    interrupted = sig;
}

event_response_t singlestep_cb(vmi_instance_t vmi, vmi_event_t *event) {
	vmi_write_8_va(vmi, syscalls[i].addr, 0, &trap);
	return (1u << VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP);
}

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event) {

    printf("Received a trap event for syscall %s!\n", syscalls[i].name);

    vmi_write_8_va(vmi, syscalls[i].addr, 0, &backup_byte);

    event->interrupt_event.reinject = 0;

    return (1u << VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP);
}

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s <domain name> <syscall_name>\n");
        return 1;
    }

    status_t status;
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

	vmi_instance_t vmi;
	// Initialize the libvmi library.
	if (vmi_init(&vmi, VMI_XEN | VMI_INIT_COMPLETE | VMI_INIT_EVENTS, name) == VMI_FAILURE) {
		printf("Failed to init LibVMI library.\n");
		return;
	}

	vmi_pause_vm(vmi);

    for (i=0; i < NUMBER_OF_SYSCALLS; i++) {
        //printf("%s at 0x%lx\n", syscalls[i].name, syscalls[i].addr);

        if(!strcmp(syscalls[i].name, syscall)) {
            printf("Syscall %s is located at VA 0x%lx\n", syscall, syscalls[i].addr);
			vmi_read_8_va(vmi, syscalls[i].addr, 0, &backup_byte);
			vmi_write_8_va(vmi, syscalls[i].addr, 0, &trap);
            break;
		}
	}

    vmi_event_t trap_event, singlestep_event;
	SETUP_INTERRUPT_EVENT(&trap_event, 0, trap_cb);
	SETUP_SINGLESTEP_EVENT(&singlestep_event, 1, singlestep_cb, 0);
	vmi_register_event(vmi, &trap_event);
	vmi_register_event(vmi, &singlestep_event);

	vmi_resume_vm(vmi);
    while(!interrupted){
	    status = vmi_events_listen(vmi,500);
        if (status != VMI_SUCCESS) {
            printf("Error waiting for events, quitting...\n");
            interrupted = -1;
        }
	}

    // TODO 4: remove traps and close LibVMI
    vmi_write_8_va(vmi, syscalls[i].addr, 0, &backup_byte);

    vmi_destroy(vmi);
    return 0;
}
