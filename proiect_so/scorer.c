#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<signal.h>
#include<fcntl.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#define MAX 100
#define desc 256
typedef struct report
{
  int id;
  char nume[MAX];
  float lat,lon;
  char issue[MAX];
  int severity;
  time_t timestamp;
  char description[desc];
}report;

int main(int argc,char *argv[])
{
    if(argc < 1) return 1;
    char path[256];
    snprintf(path,sizeof(path),"%s/reports.dat",argv[1]);
    int fd = open(path,O_RDONLY);
    if(fd == -1){
       char err[128];
       int len = snprintf(err,sizeof(err),"Eroare la deschiderea fisierului, districtul %s nu are rapoarte\n",argv[1]);
       write(STDOUT_FILENO,err,len);
       return 1;

    }
    report r;
    int nr_total_sev = 0;
    while(read(fd,&r,sizeof(report))>0)
    {
        nr_total_sev += r.severity;
    }
   char buffer[512];
   int lungime = snprintf(buffer,sizeof(buffer),"Pentru districtul %s,scorul este %d\n",argv[1],nr_total_sev);
   write(STDOUT_FILENO,buffer,lungime);
   close(fd);
   return 0;
}