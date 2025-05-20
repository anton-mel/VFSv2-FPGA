/*
 * This file implements the tree structure and operations for the Paragon VFS.
 */

#include "tree.h"
#include "../aux/ip.h"
#include "../fops/proc.h"

// Global pointer to the root of the tree
struct paragon_tree_node *root = NULL;
struct proc_dir_entry *paragon_dir = NULL;

// Function to search for a node by ID
// struct paragon_tree_node* find_node_by_id(struct paragon_tree_node *node, int id) {
//     struct paragon_tree_node *found_node;
//     if (!node) 
//         return NULL;

//     if (node->node_id == id)
//         return node;

//     for (int i = 0; i < 4; i++) {
//         found_node = find_node_by_id(node->children[i], id);
//         if (found_node)
//             return found_node;
//     }

//     return NULL;
// }

// Helper function to find the parent of a node with a given ID
// struct paragon_tree_node* find_parent(struct paragon_tree_node *node, int id) {
//     struct paragon_tree_node *parent;
//     if (!node)
//         return NULL;

//     for (int i = 0; i < 4; i++) {
//         if (node->children[i] && node->children[i]->node_id == id)
//             return node;

//         parent = find_parent(node->children[i], id);
//         if (parent)
//             return parent;
//     }

//     return NULL;
// }

// Modified create_node function
// struct paragon_tree_node* create_node(struct paragon_tree_node *parent, int child_index) {
//     if (!parent->children[child_index]) {
//         // Slot is available, create a new node
//         struct paragon_tree_node *new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);
//         if (!new_node) {
//             printk(KERN_ALERT "Failed to allocate memory for new node\n");
//             return NULL;
//         }
//         new_node->node_id = find_a_free_ip();
//         set_ip(new_node->node_id);
//         new_node->status = 0;
//         for (int j = 0; j < 4; j++)
//             new_node->children[j] = NULL;

//         // Add the new node to the parent's children array
//         parent->children[child_index] = new_node;
//         return new_node;
//     }

//     // No available slot in the parent's children
//     printk(KERN_INFO "There is already a node at the given index\n");
//     return NULL;
// }

// Function to recursively delete a node and all its children
// void delete_tree(struct paragon_tree_node *node) {
//     if (!node)
//         return;
//     printk(KERN_INFO "Started deleting tree\n");
//     for (int i = 0; i < 4; i++) {
//         if (node->children[i]){
//             clear_ip(node->node_id);
//             proc_remove(node->proc_entry);
//             kfree(node);
//             printk(KERN_INFO "Child deleted\n");
//             node->children[i] = NULL; 
//         }
//     }
    
//     clear_ip(node->node_id);
//     proc_remove(node->proc_entry);
//     printk(KERN_INFO "Proc entry deleted\n");
//     proc_remove(node->parameter_entries[2]);
    
//     kfree(node);
// }

// Modified delete_node function to search and delete by ID
// void delete_node(int id) {
//     struct paragon_tree_node *parent;
//     if (!root)
//         return;

//     if (root->node_id == id) {
//         // Special case if we're deleting the root
//         // Assuming deep deletion: delete the root and all its descendants
//         delete_tree(root);
//         root = NULL;
//         return;
//     }

//     parent = find_parent(root, id);
//     if (!parent) {
//         printk(KERN_ALERT "Node with ID %d not found\n", id);
//         return;
//     }

//     for (int i = 0; i < 4; i++) {
//         if (parent->children[i] && parent->children[i]->node_id == id) {
//             // Found the node to delete
//             delete_tree(parent->children[i]); // Assuming deep deletion
//             parent->children[i] = NULL;
//             return;
//         }
//     }
// }

int paragon_tree_init(void) {
    printk(KERN_INFO "Paragon Tree Module Initialized\n");

    ip_init();
    // Initialize the tree by creating the root node
    root = kmalloc(sizeof(*root), GFP_KERNEL);

    if (!root) {
        printk(KERN_ALERT "Failed to allocate memory for root node\n");
        return -ENOMEM;
    }

    //Set root to IP 2
    root->node_id = find_a_free_ip();
    set_ip(root->node_id);

    root->status = 0;  // Assuming a default status, modify as needed
    for (int i = 0; i < 4; i++)
        root->children[i] = NULL;

    return 0;
}

// int add_hub_process(int id, int child_index) {
//     struct paragon_tree_node *parent_node;
//     struct paragon_tree_node *new_node;
//     char hub_name[20]; // Buffer to hold the hub name
//     char status_name[16];

//     parent_node = find_node_by_id(root, id);
//     if (!parent_node) {
//         printk(KERN_ALERT "Parent node with ID %d not found\n", id);
//         return -1;
//     }

//     new_node = create_node(parent_node, child_index);
//     if (!new_node) {
//         printk(KERN_ALERT "Failed to create new node\n");
//         return -1;
//     }

//     //to give each hub a different folder
//     snprintf(hub_name, sizeof(hub_name), "fpga%d", child_index+1);

//     new_node->proc_entry = proc_mkdir(hub_name, parent_node->proc_entry);
//     if (!new_node->proc_entry) {
//         printk(KERN_ALERT "Failed to create proc entry for new node\n");
//         return -1;
//     }

//     snprintf(status_name, sizeof(status_name), "status_%d", new_node->node_id);

//     new_node->parameter_entries[2] = proc_create(status_name, 0666, new_node->proc_entry, &bram_fops);
//     printk(KERN_INFO "fpga %d created\n", new_node->node_id);

//     return 0;
// }

int add_root_hub(void) {
    root->proc_entry = proc_mkdir("root", paragon_dir);
    if (!root->proc_entry) {
        printk(KERN_ALERT "Failed to create proc entry for new node\n");
        return -1;
    }

    // root->parameter_entries[2] = proc_create("status", 0666, root->proc_entry, &status_fops);
    // root->parameter_entries[3] = proc_create("decode", 0666, root->proc_entry, &decode_fops);
    // root->parameter_entries[4] = proc_create("generate", 0666, root->proc_entry, &generate_fops);
    printk(KERN_INFO "hub %d created\n", root->node_id);

    // Add new input/output files
    root->parameter_entries[5] = proc_create("input", 0666, root->proc_entry, &input_fops);
    root->parameter_entries[6] = proc_create("output", 0444, root->proc_entry, &output_fops);
    printk(KERN_INFO "I/O created\n");

    return 0;
}
