#include "test_fops.h"

// Decoding thread variables
static struct task_struct *decoding_thread;
static bool decoding_active = false;

// Function to continuously send the message every few seconds
int decoding_thread_fn(void *data)
{
    while (!kthread_should_stop()) {
        if (decoding_active && ioread64(sbram_addr) == 0) {
            iowrite64(0x000200000000000f, sbram_addr+8); //header_initialize_decoding = 8'h02 and added 0000f bcs peel_and_finish should be 1
            iowrite64(0x1, sbram_addr); //setting valid bit
            pr_info("Sent message to start decoding\n");
            msleep(500); // Sleep for 500ms
        } else {
            msleep(1000); // Sleep for 1 second if not active
        }
    }
    return 0;
}

// File operation to start/stop the decoding process
ssize_t start_decoding(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos)
{
    char *buffer;
    char message[BUF_SIZE];
    
    if (count >= BUF_SIZE)
        return -EINVAL;

    buffer = kzalloc(BUF_SIZE, GFP_KERNEL);
    if (!buffer)
        return -ENOMEM;

    if (copy_from_user(buffer, user_buffer, count)) {
        kfree(buffer);
        return -EFAULT;
    }

    buffer[count] = '\0'; // Null-terminate the string

    sscanf(buffer, "%s", message); // Read the command (start or stop)

    if (strcmp(message, "start") == 0) {
        if (!decoding_active) {
            decoding_active = true;
            // Start the decoding thread
            decoding_thread = kthread_run(decoding_thread_fn, NULL, "decoding_thread");
            if (IS_ERR(decoding_thread)) {
                pr_err("Failed to create decoding thread\n");
                decoding_active = false;
                kfree(buffer);
                return PTR_ERR(decoding_thread);
            }
            pr_info("Started decoding thread\n");
        }
    } else if (strcmp(message, "stop") == 0) {
        if (decoding_active) {
            decoding_active = false;
            kthread_stop(decoding_thread); // Stop the decoding thread
            pr_info("Stopped decoding thread\n");
        }
    } else {
        pr_err("Invalid command. Use 'start' or 'stop'.\n");
    }

    kfree(buffer);
    return count;
}

ssize_t generate_measurements(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos)
{
    char *input; //buffer for input string
    char *line;
    uint64_t node_id;
    uint64_t message, destination;
    uint64_t sbram_valid;

    if (count >= BUF_SIZE)
        return -EINVAL;

    input = kzalloc(BUF_SIZE, GFP_KERNEL);
    if (!input)
        return -ENOMEM;

    if (copy_from_user(input, user_buffer, count)) {
        kfree(input);
        return -EFAULT;
    }

    input[count] = '\0'; // Null-terminate the string
    printk(KERN_ALERT "Reading data\n"); //error log

    line = input;

    while (sscanf(line, "%llu", &node_id) == 1) {
        pr_info("Processing node ID: %lu\n", node_id);

        // Generate the message
        destination = ((node_id - 2) & 0xFF) << 56;
        printk("destination: 0x%016llx\n", destination);
        message = destination | 0x0023000000000000; // destination = node_id - 2 and header
        // Remaining bits are 0

        //error log - Wait for sbram valid bit to be set to 0 to check that the PL has processed the request
        do {
            sbram_valid = ioread64(sbram_addr);
            msleep(10);  // Small delay to prevent busy waiting
        } while (sbram_valid == 1);

        // Write the message to the message address
        iowrite64(message, sbram_addr + 8); 

        // Set the valid bit to 1
        iowrite64(1, sbram_addr);

        pr_info("Sent message: 0x%016llX to node ID: %lu\n", message, node_id);

        // Move to the next node ID in the input
        line = strchr(line, ' ');
        if (!line)
            break;
        line++; // Skip the space
    }

    kfree(input);
    return count;
}

const struct proc_ops generate_fops = {
    .proc_write = generate_measurements,
};

const struct proc_ops decode_fops = {
    .proc_write = start_decoding,
};
