#ifndef _LINK_H  
#define _LINK_H

typedef struct configure{
	unsigned char feild[128];
	unsigned char value[128];
	
	struct configure *next;
}Config;

extern int delete_link(Config *head);
extern Config *insert_node(Config *head, Config node);
extern int print_link(Config *head);
#endif  
