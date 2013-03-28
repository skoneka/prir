#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


#define WL 50 //Word Length
#define LL 5*WL //Line Length

#define SCAN_LOG_IP 0
#define SCAN_LOG_TIME 1

int scan_log(char *fn, int scan_type) {
	char date[WL],time[WL],ip[WL],msg[WL];
	FILE *f = fopen(fn, "r");
	while ( fscanf(f, "%s %s %s %s", &date, &time, &ip, &msg) != EOF ) {
		raise(SIGINT);
	}
}


int main() {
	scan_log("access-log", "r");
}
