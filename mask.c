#include<stdio.h>
#include<string.h>

int main(int agrc, char *agrv[])
{
	char mask[] = "255.255.0.0";
	unsigned int mask_t[4] = {0};
	unsigned int mask_tmp[4] = {0};
	
	sscanf(mask,"%d.%d.%d.%d", &mask_t[0], &mask_t[1], &mask_t[2], &mask_t[3]);
	
	printf("%d.%d.%d.%d\n", mask_t[0], mask_t[1], mask_t[2], mask_t[3]);

	int i = 0;	
	for(i = 0; i < 4; i++){
		mask_tmp[i] = mask_t[i] ^ 255;	
	}	
	
	printf("%d.%d.%d.%d\n", mask_tmp[0], mask_tmp[1], mask_tmp[2], mask_tmp[3]);
}
