#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int bplog(FILE *fp, char logMessage[])
{

    char message[400];
    time_t now = time(NULL);
    struct tm *timenow;

    timenow = localtime(&now);
    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: ", timenow);
    strcat(message, logMessage);
    strcat(message, "\n");
    fprintf(fp, message);
    fflush(fp);
    
	return(0);
	
}

int main(int argc, char* argv[])
{
    // open log file
    FILE *fp= NULL;
    fp = fopen ("/home/pi/BRPiPlayer/brpiplayer.log", "a+");

    // sleep
    int sleepCount;
    if( argc == 2 ) {
        sscanf(argv[1], "%d", &sleepCount);
    } else {
        sleepCount = 0;
    }
    sleep(sleepCount);

    // stop radio
    char killRadio[250];
    strcpy(killRadio, "killall mpg321");
    system(killRadio);
    bplog(fp, "# stopping radio");
    fclose(fp);

    return (0);

}
