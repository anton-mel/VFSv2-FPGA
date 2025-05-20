#ifndef IP_H
#define IP_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

void set_ip(int n);
void clear_ip(int n);
bool is_ip_set(int n);
int find_a_free_ip(void);
int ip_init(void);

#endif /* IP_H */ 