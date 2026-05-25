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
  char role[MAX];
  char nume[MAX];
  float lat,lon;
  char issue[MAX];
  int severity;
  time_t timestamp;
  char description[desc];
}report;
typedef struct inspector
{
  char nume[MAX];
  int sev_totala;
}inspector;
inspector vector[MAX];
int nr_inspectori = 0;
int main(int argc,char *argv[])
{
    if(argc < 2) return 1;
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
    while(read(fd,&r,sizeof(report))> 0){
      //adunam scorul doar daca rolul este "inspector"
      if (strcmp(r.role, "inspector") != 0) {
            continue;
      }
      int gasit = 0;
      for(int j =0;j<nr_inspectori;j++){
        if(strcmp(vector[j].nume,r.nume) == 0){
          vector[j].sev_totala += r.severity;
          gasit = 1;
          break;
        }
      }
      if(!gasit){
        strcpy(vector[nr_inspectori].nume,r.nume);
        vector[nr_inspectori].sev_totala = r.severity;
        nr_inspectori++;
      }
    }
   
   close(fd);
   //afisarea rezultatelor
    char out1[256];
    int len1 = snprintf(out1,sizeof(out1),"Districtul:%s\n",argv[1]);
    write(STDOUT_FILENO,out1,len1);
    for(int j = 0;j<nr_inspectori;j++){
    char out[256];
    int len = snprintf(out,sizeof(out),"Inspector:%s | Severitatea totala: %d\n",
    vector[j].nume,vector[j].sev_totala);
    write(STDOUT_FILENO,out,len);
   }
   return 0;
}