#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_DIR "./log"
#define LOG_FILE "log/fail.log"
#define LOG_STATISTICS "log/statistics.log"
int log_error(unsigned char *error_msg)
{
    time_t time_stamp;
    FILE *fail_fp = NULL;
    if ((fail_fp = fopen(LOG_FILE, "a+")) == NULL){
        perror("tmp_fail");
        return -1;
    }
    time(&time_stamp);
    fprintf(fail_fp,"ERROR: <%s> %s\n", ctime(&time_stamp), error_msg);
    fflush(fail_fp);
    fclose(fail_fp);
    return 0;
}

static int log_add(unsigned char *file, unsigned char *content)
{
    FILE *fp = NULL;
    if ((fp = fopen(file, "a+")) == NULL){
        perror("open file");
        return -1;
    }
    fwrite(content, strlen(content), 1, fp);
    fflush(fp);
    fclose(fp);
}

int statistics(unsigned char *type, int number)
{
    unsigned char content[1024] = "";
    sprintf(content, "%s: %d\n", type, number);
    int ok = log_add(LOG_STATISTICS, content);
    return ok;
}

int init_log()
{
    int log_ok = mkdir(LOG_DIR, 0766);
    if (log_ok < 0){
        perror("log dir");
    }
	int ok = remove(LOG_FILE);
    ok = remove(LOG_STATISTICS);
	return ok;
}

