//singly linked list: 
//adapted from
//https://gist.github.com/ArnonEilat/4470948#file-singlylinkedlist-c-L4

#include <stdio.h>
#include <stdlib.h>

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

struct node* device_list;
/************************************/

/*************Device Manager**************/
void init(struct node* head) {
    head = NULL;
}

void register_device(struct device* device_option) {
    struct node* temp = (struct node*) malloc(sizeof (struct node));
    if (temp == NULL) {
        exit(0); // no memory available
    }

    //first element of list (top/head of list)
    temp->dev = device_option;

    //second element of list
    temp->next = device_list;

    device_list = temp;
}

int open(struct device *dev){
	dev->open(dev);
    return 1;
}
/***********************************/


/*************Device 1**************/
static void dev1_open(struct device *dev){
	dev->devop_init_registered = 0;
}

static struct device dev1 = {
        .name   = "device_1",
        .open   = dev1_open,
};
/***********************************/

/*************User**************/
int main() {
    init(device_list);
    printf("Add device to the list:\n");
    register_device(&dev1);
    open(&dev1);
    return 0;
}
/***********************************/