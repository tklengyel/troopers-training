/*
 * Copyright (C) 2016 Tamas K Lengyel
 * ACADEMIC PUBLIC LICENSE
 * For details please read the LICENSE file.
 */

/*
 * gcc cr3.c -lvmi -o cr3
 */
#include <signal.h>
#include <libvmi/libvmi.h>
#include <libvmi/events.h>

/* Signal handler */
static struct sigaction act;
static int interrupted = 0;
static void close_handler(int sig){
	interrupted = sig;
}

event_response_t cr3_event(vmi_instance_t vmi, vmi_event_t *event) {
    // TODO 5: find the PID of the current process being scheduled
    vmi_pid_t pid = vmi_dtb_to_pid(vmi, event->reg_event.value);
    printf("PID %i with DTB 0x%lx being scheduled\n", pid, event->reg_event.value);
    return 0;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s <domain name>\n", argv[0]);
        return 1;
    }

    const char *name = argv[1];

	/* Signal handler to catch CTRL+C, etc.. */
	act.sa_handler = close_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGHUP,  &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT,  &act, NULL);
	sigaction(SIGALRM, &act, NULL);

	// TODO 1: initialize LibVMI
	vmi_instance_t vmi;
    if (vmi_init(&vmi, VMI_XEN | VMI_INIT_COMPLETE | VMI_INIT_EVENTS, argv[1]) == VMI_FAILURE){
        printf("Failed to init LibVMI library.\n");
	    return 1;
    }

	// TODO 2: initialize CR3 event and register it with LibVMI
    vmi_event_t cr3;
    SETUP_REG_EVENT(&cr3, CR3, VMI_REGACCESS_W, 0, cr3_event);
    vmi_register_event(vmi, &cr3);

    while (!interrupted) {
        // TODO 3: run LibVMI events loop
        status_t status = vmi_events_listen(vmi,500);
        if (status != VMI_SUCCESS) {
            printf("Error waiting for events, quitting...\n");
            interrupted = -1;
        }
    }

	// TODO 4: remove event and close LibVMI
    vmi_clear_event(vmi, &cr3, NULL);
    vmi_destroy(vmi);

    return 0;
}
