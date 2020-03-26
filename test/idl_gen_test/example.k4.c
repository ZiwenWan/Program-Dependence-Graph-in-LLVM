//singly linked list: 
//adapted from
//https://gist.github.com/ArnonEilat/4470948#file-singlylinkedlist-c-L4
//https://www.geeksforgeeks.org/search-an-element-in-a-linked-list-iterative-and-recursive/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************Global DS**************/
struct device {
    void	(*open)(struct device *dev);
    char    *name;
	int	    devop_init_registered;
} ;

struct node {
    struct device* dev;
    struct node* next;
} ;

struct node* device_list_head;
/************************************/

/*************Device Manager**************/
void init(struct node* head) {
    head = NULL;
}

void register_device(struct device* dev)
{
    /* allocate node */
    struct node* new_node = (struct node*) malloc(sizeof(struct node));
 
    /* put in the key  */
    new_node->dev  = dev;
 
    /* link the old list off the new node */
    new_node->next = device_list_head;
 
    /* move the head to point to the new node */
    device_list_head = new_node;
}

int open(struct device *dev){
	dev->open(dev);
    printf("opened %s\n",dev->name);
    return 1;
}

void device_reset(int * dev_init){
    *dev_init = 0;
}

struct device * lookup_device(char * search_name)
{
    struct node* current = device_list_head;  // Initialize current
    while (current != NULL)
    {
        if (strcmp(current->dev->name,search_name) == 0)
            return current->dev;
        else
        current = current->next;
    }
    return NULL;
    
}
/***********************************/


/*************Device 1**************/
static void dev1_open(struct device *dev){
    device_reset(&dev->devop_init_registered);
}

static struct device dev1 = {
        .name   = "device_1",
        .open   = dev1_open,
};
/***********************************/

/*************Device 2**************/
static void dev2_open(struct device *dev){
	dev->devop_init_registered = 1;
}

static struct device dev2 = {
        .name   = "device_2",
        .open   = dev2_open,
};
/***********************************/

/*************User**************/
int main() {
    struct device * dev;
    init(device_list_head);
    printf("Add device to the list:\n");
    register_device(&dev1);
    register_device(&dev2);

    dev = lookup_device("device_1");
    open(dev);
    dev = lookup_device("device_2");
    open(dev);

    return 0;
}
/***********************************/

