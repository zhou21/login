#include<stdio.h>
#include <time.h>

#define LOG_FILE "tmp_fail"

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

int delete_log_file()
{
	int ok = remove(LOG_FILE);
	return ok;
}
