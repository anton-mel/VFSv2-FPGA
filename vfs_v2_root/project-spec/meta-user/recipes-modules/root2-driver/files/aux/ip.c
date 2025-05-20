#include <linux/kernel.h>
#include <linux/string.h>

#include "ip.h"


uint8_t free_ip[64];

void set_ip(int n) {
    free_ip[n / 8] |= (1 << (n % 8));
}

void clear_ip(int n) {
    free_ip[n / 8] &= ~(1 << (n % 8));
}

bool is_ip_set(int n) {
    return free_ip[n / 8] & (1 << (n % 8));
}

int find_a_free_ip(void) {
    for (int i = 0; i < 64; i++) {
        if (free_ip[i] != 0xFF) {
            for (int j = 0; j < 8; j++) {
                if (!(free_ip[i] & (1 << j))) {
                    return i * 8 + j;
                }
            }
        }
    }
    return -1;
}

int ip_init(void) {
    // Initialize all bits to 0
    memset(free_ip, 0, 64);
    set_ip(0);
    set_ip(1);
    printk(KERN_INFO "IP array Initialized. Host has IP 192.168.1.1\n");
    return 0;
}
