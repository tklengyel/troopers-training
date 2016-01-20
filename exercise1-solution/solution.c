/*
 * Copyright (C) 2016 Tamas K Lengyel
 * ACADEMIC PUBLIC LICENSE
 * For details please read the LICENSE file.
 */

/*
 * gcc solution.c -lvmi -o solution
 */
#include <stdio.h>
#include <stdlib.h>
#include <libvmi/libvmi.h>

int main(int argc, char **argv) {

    if(argc != 4) {
        printf("Usage: %s <domain name> <pid> <stack_addr>\n", argv[0]);
        return 1;
    }

    if (!strncmp(argv[3], "0x", 2)) {
        printf("Don't need to specify 0x for stack address\n");
        return 1;
    }

    const char *name = argv[1];
    vmi_pid_t pid = atoi(argv[2]);
    addr_t stack_addr = strtoull (argv[3], NULL, 16);

    vmi_instance_t vmi;
    /* initialize the libvmi library */
    if (vmi_init(&vmi, VMI_AUTO | VMI_INIT_COMPLETE, name) == VMI_FAILURE) {
        printf("Failed to init LibVMI library.\n");
        return;
    }

    addr_t heap_addr;
    vmi_read_addr_va(vmi, stack_addr, pid, &heap_addr);
    int value;
    vmi_read_32_va(vmi, heap_addr, pid, &value);

    printf("The value at 0x%lx is %i\n", heap_addr, value);

    value = 1337;

    vmi_write_32_va(vmi, heap_addr, pid, &value);

    /* cleanup any memory associated with the LibVMI instance */
    vmi_destroy(vmi);

    return 0;
}
