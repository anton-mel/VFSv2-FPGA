#ifndef TREE_H
#define TREE_H

#include <linux/proc_fs.h>
#include <linux/fs.h>

#include "../bram/status/status_bram.h"
#include "../bram/sender/send_bram.h"
#include "../bram/receiver/receive_bram.h"

// Global variables
extern struct paragon_tree_node *root;
extern struct proc_dir_entry *paragon_dir;

// Tree node structure
struct paragon_tree_node {
    int node_id;
    int status;
    struct paragon_tree_node *children[4];
	struct proc_dir_entry *proc_entry;
	struct proc_dir_entry *parameter_entries[10];
};

// Function declarations
int paragon_tree_init(void);
int add_root_hub(void);
// int add_hub_process(int id, int child_index);
// void delete_node(int id);
// void delete_tree(struct paragon_tree_node *node);
// struct paragon_tree_node* find_node_by_id(struct paragon_tree_node *node, int id);
// struct paragon_tree_node* find_parent(struct paragon_tree_node *node, int id);
// struct paragon_tree_node* create_node(struct paragon_tree_node *parent, int child_index);

#endif /* TREE_H */ 