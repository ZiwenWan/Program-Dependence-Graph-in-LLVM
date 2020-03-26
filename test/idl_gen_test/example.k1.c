//singly linked list: 
//adapted from
//https://gist.github.com/ArnonEilat/4470948#file-singlylinkedlist-c-L4

#include <stdio.h>
#include <stdlib.h>

typedef struct device_operations {
    void	(*open)(struct device_operations *dev);
	int	    devop_init_registered;
} devops_ds;

typedef struct node {
    devops_ds *devops;
    struct node* next;
} NODE;

NODE* devops_list;

void init(NODE* head) {
    head = NULL;
}

NODE* add(NODE* node, devops_ds* devops_option) {
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(0); // no memory available
    }

    //first element of list (top/head of list)
    temp->devops = devops_option;

    //second element of list
    temp->next = node;

    node = temp;
    return node;
}

void reg_devops(void(*fp)(devops_ds*)){
    	fp(devops_list->devops);
}

static void mimix_open(struct device_operations *devops){
	devops->devop_init_registered = 0;
}

static struct device_operations devops = {
        .open   = mimix_open,
};


int main() {
    printf("Add Devops to the list:\n");
    init(devops_list);
    devops_list = add(devops_list, &devops);
    reg_devops(devops_list->devops->open);
    return 0;
}
