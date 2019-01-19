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

void itoa(int n, char s[])
{
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

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

int setVolume(int vol)
{

    char message[400];
    char tempstring[400];

    strcpy(message, "amixer set PCM ");
    itoa(vol, tempstring);
    strcat(message, tempstring);
    strcat(message, "%");
    system(message);

    return(0);

}

int volumeUp(int vol) {

    if ((vol+2) > 100) {
        setVolume(100);
    } else {
        setVolume(vol+2);
    }

    return(0);

}

int volumeDown(int vol) {

    if ((vol-2) < 0) {
        setVolume(0);
    } else {
        setVolume(vol-2);
    }

    return(0);

}

int main(int argc, char* argv[])
{

    FILE *fp= NULL;
    fp = fopen ("/home/pi/BRPiPlayer/brpiplayer.log", "a+");

    int volume;
    volume = 80;
    char message[400];
    char tempstring[400];
    struct tm *timenow;
    time_t now = time(NULL);
    int pid;
    int radioPid;
    int xmmsPid;
    char radioStart[250];
    strcpy(radioStart, "nohup /usr/bin/mpg321 http://icecast2.play.cz/cropardubice128.mp3 &");
    char radioSleepStart[250];
    strcpy(radioSleepStart, "nohup /home/pi/BRPiPlayer/killradio 3600 &");
    char radioStop[250];
    strcpy( radioStop, "killall mpg321 killradio");
    char xmmsStart[250];
    strcpy(xmmsStart, "xmms2 play");
    char xmmsSleepStart[250];
    strcpy(xmmsSleepStart, "nohup /home/pi/BRPiPlayer/killxmms 3600 &");
    char xmmsStop[250];
    strcpy(xmmsStop, "xmms2 pause && killall killxmms");
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

    bplog(fp, "### starting brpiplayer!");
    strcpy(message, "amixer set PCM ");
    itoa(volume, tempstring);
    strcat(message, tempstring);
    strcat(message, "%");
    bplog(fp, message);
    system(message);

    while (1) {
        bytes = read(device, &ev, sizeof(ev));
        // left button
        if(bytes > 0) {
            if (ev.code == 272 && ev.value == 1) {
                bplog(fp, "* left button pressed");
                radioPid = system("pidof -x killradio > /dev/null");
                strcpy(message, "searching for pid of killradio: ");
                itoa(radioPid, tempstring);
                strcat(message, tempstring);
                bplog(fp, message);
                if (radioPid == 0) {
                    bplog(fp, "pid is 0");
                    bplog(fp, "# stopping radio");
                    system(radioStop);
                } else {
                    bplog(fp, "pid not 0 - continue");
                }
                xmmsPid = system("pidof -x killxmms > /dev/null");
                strcpy(message, "searching for pid of killxmms: ");
                itoa(xmmsPid, tempstring);
                strcat(message, tempstring);
                bplog(fp, message);
                if (xmmsPid == 256) {
                    bplog(fp, "pid is 256");
                    bplog(fp, "# starting xmms");
                    system(xmmsStart);
                    system(xmmsSleepStart);
                } else if (xmmsPid == 0) {
                    bplog(fp, "pid is 0");
                    bplog(fp, "# stopping xmms");
                    system(xmmsStop);
                }
            }
            // right button
            if (ev.code == 273 && ev.value == 1) {
                bplog(fp, "* right button pressed");
                xmmsPid = system("pidof -x killxmms > /dev/null");
                strcpy(message, "searching for pid of killxmms: ");
                itoa(xmmsPid, tempstring);
                strcat(message, tempstring);
                bplog(fp, message);
                if (xmmsPid == 0) {
                    bplog(fp, "pid is 0");
                    bplog(fp, "# stopping xmms");
                    system(xmmsStop);
                } else {
                    bplog(fp, "pid not 0 - continue");
                }
                radioPid = system("pidof -x killradio > /dev/null");
                strcpy(message, "searching for pid of killradio: ");
                itoa(radioPid, tempstring);
                strcat(message, tempstring);
                bplog(fp, message);
                if (radioPid == 256) {
                    bplog(fp, "pid is 256");
                    bplog(fp, "# starting radio");
                    system(radioStart);
                    system(radioSleepStart);
                } else if (radioPid == 0) {
                    bplog(fp, "pid is 0");
                    bplog(fp, "# stopping radio");
                    system(radioStop);
                }
            }
            // middle button
            if (ev.code == 274 && ev.value == 1) {
                bplog(fp, "* middle button pressed");
            }
            if (ev.code == 8 && ev.value == -1) {
                bplog(fp, "* moved wheel down");
                strcpy(message, "volume down: ");
                itoa((volume-2), tempstring);
                strcat(message, tempstring);
                bplog(fp, message);
                volumeDown(volume);
                volume = volume - 2;
                if (volume < 0) volume = 0;
            }
            if (ev.code == 8 && ev.value == 1) {
                bplog(fp, "* moved wheel up");
                strcpy(message, "volume up: ");
                itoa((volume+2), tempstring);
                strcat(message, tempstring);
                bplog(fp, message);
                volumeUp(volume);
                volume = volume + 2;
                if (volume > 100) volume = 100;
            }
        }
    }
    fclose(fp);

    return(0);

}
