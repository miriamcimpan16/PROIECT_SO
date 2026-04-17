#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include <sys/stat.h> //pt stat,lstat,mkdir si chmod
#include<fcntl.h> // pt open
#include<unistd.h> // pt close write si read
#include<sys/types.h>
#include<time.h> //pentru time()
#define MAX 100
//stat(path,&struct_stat) --> 0 daca exista | -1 daca nu
//open pentru creare de fisiere
//structura pentru reports.dat
typedef struct report
{
  int id;
  char nume[MAX];
  float lat,log;
  char issue[MAX];
  int severity;
  time_t timestamp;
  char description[MAX];
}report;
//path-urile o sa le pun ca variabile globale
char path_reports[MAX]="";
char path_cfg[MAX]="";
char path_log[MAX]="";
void writing_in_reports(char district[])
{
  // trebuie sa verific cu st+mode si stat daca a permisiunea de scriere, nu stiu cum sa fac asta
  snprintf(path_reports,MAX,"%s/reports.dat",district);
  int fd  = open(path_reports,O_WRONLY|O_APPEND|O_CREAT);
  if(fd == -1)
    {
      printf("eroare la deschiderea fisierului");
      exit(EXIT_FAILURE);
    }
  report element;
  element.id = 1;
  strcpy(element.nume,"test");
  element.lat = 5.4; element.log = 4.2;
  strcpy(element.issue, "road");
  element.severity = 2;
  element.timestamp = time(NULL);
  strcpy(element.description, "groapa mare in asfalt");
 
		       if((write(fd,&element,sizeof(element)) == -1))
     {
       printf("eroare la scriere");
       exit(EXIT_FAILURE);
			   
     }
  close(fd);
  
}
void creare_reports(char district[])
{
  
  //CREARE FISIERULUI REPORTS.DAT
      
      snprintf(path_reports,MAX,"%s/reports.dat",district);
      int fd = open(path_reports,O_CREAT | O_RDWR,0664);
      //O_CREAT-> spune daca fisierul exista sau nu, daca nu exista este create
      //O_RDWR->deschide fisierul pentru citire si pentru scriere
      if(fd == -1)
	{
	  printf("eroare la creare reports.dat");
	  exit(EXIT_FAILURE);
	}
      close(fd);
      chmod(path_reports,0664); // pun permsiunile inca odata, desi le am pus la open, pt sigurata,deoarece pot fi modificate de catre sistem
 
}
void creare_district(char district[])
{
 
  //CREAREA FISIERULUI DISTRICT.CFG
      
      snprintf(path_cfg, MAX, "%s/district.cfg", district); 
      int fd = open(path_cfg, O_CREAT | O_RDWR, 0640);
      if(fd == -1) {
	perror("eroare la creare district.cfg"); 
	exit(EXIT_FAILURE);
      }
      close(fd);
      chmod(path_cfg, 0640);
 
}
void creare_logged(char district[])
{
 
    //CREAREA FISIERULUI LOGGED_DISTRICT
      
      snprintf(path_log, MAX, "%s/logged_district", district);
      int fd = open(path_log, O_CREAT | O_RDWR, 0644);
        if (fd == -1)
        {
            perror("eroare la creare logged_district");
            exit(EXIT_FAILURE);
        }
        close(fd);
        chmod(path_log, 0644);
  
}
void comanda_add(char district[])
{
  struct stat st;
  if(stat(district,&st) == -1)
    {
      //folderul-->cu permisiunile: 0750
      if((mkdir(district,0750)) == -1)
	{
	  printf("eroare la crearea dosarului");
	  exit(EXIT_FAILURE);
	}
      
	//district/reports.dat
	//district/district.cfg
	//district/logged_district
      creare_reports(district);
      creare_district(district);
      creare_logged(district);
    
      
      
      
    }
  else
    {
      printf("Directorul exista deja\n");
    }
  writing_in_reports(district);
  
}
int main(int argc,char *argv[])
{
  
  for(int i = 0;i<argc;i++)
    {
      if(strcmp(argv[i],"--add") == 0 && ((i+1) < argc))
	{
	  if(argv[i+1] != NULL){ // verific daca utilizatorul nu a pus --add simplu, fara nimic dupa
	    comanda_add(argv[i+1]);
	    i++;//trecem la urm cuvant
	  }
	  else
	    {
	      printf("eroare la argument");
	      exit(EXIT_FAILURE);
	      
	    }
	}
    }
}
