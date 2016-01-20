/*
 * gcc solution.c -lvmi -o solution
 */
#include <stdio.h>
#include <stdlib.h>
#include <libvmi/libvmi.h>

void main(int argc, char **argv) {

    vmi_instance_t vmi;
    /* initialize the libvmi library */
    if (vmi_init(&vmi, VMI_AUTO | VMI_INIT_COMPLETE, argv[1]) == VMI_FAILURE) {
        printf("Failed to init LibVMI library.\n");
        return;
    }

    vmi_pid_t pid = atoi(argv[2]);
    addr_t stack_addr = strtoull (argv[3], NULL, 16);

    addr_t heap_addr;
    vmi_read_addr_va(vmi, pid, stack_addr, &heap_addr);
    int value;
    vmi_read_32_va(vmi, pid, heap_addr, &value);

    printf("The value at 0x%lx is %i\n", heap_addr, value);

    value = 1337;

    vmi_write_32_va(vmi, pid, heap_addr, &value);

    /* cleanup any memory associated with the LibVMI instance */
    vmi_destroy(vmi);
}
