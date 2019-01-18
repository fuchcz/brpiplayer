#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>


//void INThandler(){
//    exit(0);
//}

//void *myThreadFun(void *vargp)
//{
//    sleep(10);
//    printf("Printing GeeksQuiz from Thread \n");
//    return NULL;
//}

void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

int main(int argc, char** argv) {

    FILE *fp= NULL;
    fp = fopen ("/home/pi/BRPiPlayer/brpiplayer.log", "a+");

    char message[400];
    char tempstring[400];
    struct tm *timenow;
    time_t now = time(NULL);

    int pid;
    int radioPid;
    int xmmsPid;

    char radioStart[250];
    strcpy(radioStart, "/home/pi/BRPiPlayer/radio");
    char radioSleepStart[250];
    strcpy(radioSleepStart, "/home/pi/BRPiPlayer/killradio 3600");
    char radioStop[250];
    strcpy( radioStop, "killall radio mpg321 killradio");

    char xmmsStart[250];
    strcpy(xmmsStart, "xmms2 play");
    char xmmsSleepStart[250];
    strcpy(xmmsSleepStart, "/home/pi/BRPiPlayer/killxmms 3600");
    char xmmsStop[250];
    strcpy(xmmsStop, "xmms2 pause");
    char xmmsStop2[250];
    strcpy(xmmsStop2, "killall killxmms");

    int bytes;

    const char *pDevice = "/dev/input/event0";

    char devname[] = "/dev/input/event0";
    int device = open(pDevice, O_RDONLY);
    if(device == -1)
    {
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }
    struct input_event ev;

    radioPid = 0;
    xmmsPid = 0;

    while (1) {
        bytes = read(device, &ev, sizeof(ev));
        if(bytes > 0) {
            if (ev.code == 272 && ev.value == 1) {

                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: * left button pressed\n", timenow);
                fprintf(fp, message);
                fflush(fp);

//                printf("# LEFT BUTTON!\n");
                radioPid = system("pidof -x killradio > /dev/null");

                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: searching for pid of killradio: ", timenow);
                itoa(radioPid, tempstring);
                strcat(message, tempstring);
                strcat(message, "\n");
                fprintf(fp, message);
                fflush(fp);

                if (radioPid == 0) {

                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid is 0\n", timenow);
                    fprintf(fp, message);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: # stopping radio\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);

//                    printf("stopping radio!\n");
                    system(radioStop);
                } else {

                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid not 0 - continue\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);

                }
                xmmsPid = system("pidof -x killxmms > /dev/null");
//                printf("xmmsPid: %d\n", xmmsPid);

                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: searching for pid of killxmms: ", timenow);
                itoa(xmmsPid, tempstring);
                strcat(message, tempstring);
                strcat(message, "\n");
                fprintf(fp, message);
                fflush(fp);


                if (xmmsPid == 256) {
//                    printf("starting xmms2\n");

                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid is 256\n", timenow);
                    fprintf(fp, message);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: # starting xmms\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);

                    system(xmmsStart);
                    system(xmmsSleepStart);
                } else if (xmmsPid == 0) {
//                    printf("stopping xmms2\n");

                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid is 0\n", timenow);
                    fprintf(fp, message);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: # stopping xmms\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);

                    system(xmmsStop);
                    system(xmmsStop2);
                }
            }
            if (ev.code == 273 && ev.value == 1) {
//                printf("# RIGHT BUTTON!\n");

                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: * right button pressed\n", timenow);
                fprintf(fp, message);
                fflush(fp);

                xmmsPid = system("pidof -x killxmms > /dev/null");

                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: searching for pid of killxmms: ", timenow);
                itoa(xmmsPid, tempstring);
                strcat(message, tempstring);
                strcat(message, "\n");
                fprintf(fp, message);
                fflush(fp);

                if (xmmsPid == 0) {

                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid is 0\n", timenow);
                    fprintf(fp, message);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: # stopping xmms\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);

//                    printf("stopping xmms2!\n");
                    system(xmmsStop);
                    system(xmmsStop2);
                } else {
                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid not 0 - continue\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);
                }
                radioPid = system("pidof -x killradio > /dev/null");
//                printf("radioPid: %d\n", radioPid);

                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: searching for pid of killradio: ", timenow);
                itoa(radioPid, tempstring);
                strcat(message, tempstring);
                strcat(message, "\n");
                fprintf(fp, message);
                fflush(fp);

                if (radioPid == 256) {
//                    printf("starting radio!\n");

                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid is 256\n", timenow);
                    fprintf(fp, message);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: # starting radio\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);

                    system(radioStart);
                    system(radioSleepStart);
                } else if (radioPid == 0) {
//                    printf("stopping radio!\n");

                    now = time(NULL);
                    timenow = localtime(&now);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: pid is 0\n", timenow);
                    fprintf(fp, message);
                    strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: # stopping radio\n", timenow);
                    fprintf(fp, message);
                    fflush(fp);

                    system(radioStop);
                }
            }
            if (ev.code == 274 && ev.value == 1) {
//                printf("# MIDDLE BUTTON!\n");
                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: * middle button pressed\n", timenow);
                fprintf(fp, message);
                fflush(fp);
            }
            if (ev.code == 8 && ev.value == -1) {
//                printf("# WHEEL DOWN!\n");
                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: * moved wheel down \n", timenow);
                fprintf(fp, message);
                fflush(fp);
            }
            if (ev.code == 8 && ev.value == 1) {
//                printf("# WHEEL UP!\n");
                now = time(NULL);
                timenow = localtime(&now);
                strftime(message, sizeof(message), "%Y-%m-%d_%H:%M:%S: * moved wheel up \n", timenow);
                fprintf(fp, message);
                fflush(fp);
            }
//            printf("code: %d\n", ev.code);
//            printf("value: %d\n", ev.value);
//            printf("---\n");
        }
//        if (ev.type == 1 && ev.value == 1) {
//            printf("Key: %i State: %i\n", ev.code, ev.value);
//        }
    }
    fclose(fp);
}
