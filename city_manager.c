#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include <sys/stat.h> //pt stat,lstat,mkdir si chmod
#include<fcntl.h> // pt open
#include<unistd.h> // pt close write si read
#include<sys/types.h>
#define MAX 100
//stat(path,&struct_stat) --> 0 daca exista | -1 daca nu
//open pentru creare de fisiere
void creare_fisiere(char district[])
{
  struct stat st;
  if(stat(district,&st) == -1)
    {
      //folderul-->cu permisiunile: 0750
      mkdir(district,0750);
	//district/reports.dat
	//district/district.cfg
	//district/logged_district
      //CREARE FISIERULUI REPORTS.DAT
      char path_reports[MAX];
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
      //CREAREA FISIERULUI DISTRICT.CFG
      char path_cfg[MAX];
      snprintf(path_cfg, MAX, "%s/district.cfg", district); 
      fd = open(path_cfg, O_CREAT | O_RDWR, 0640);
      if(fd == -1) {
	perror("eroare la creare district.cfg"); 
	exit(EXIT_FAILURE);
      }
      close(fd);
      chmod(path_cfg, 0640);
   
      //CREAREA FISIERULUI LOGGED_DISTRICT
      char path_log[MAX];
      snprintf(path_log, MAX, "%s/logged_district", district);
      fd = open(path_log, O_CREAT | O_RDWR, 0644);
        if (fd == -1)
        {
            perror("eroare la creare logged_district");
            exit(EXIT_FAILURE);
        }
        close(fd);
        chmod(path_log, 0644);
      
      
      
    }
  else
    {
      printf("Directorul exista deja");
    }
}
int main(int argc,char *argv[])
{
  
  for(int i = 0;i<argc;i++)
    {
      if(strcmp(argv[i],"--add") == 0 && ((i+1) < argc))
	{
	  if(argv[i+1] != NULL){ // verific daca utilizatorul nu a pus --add simplu, fara nimic dupa
	    creare_fisiere(argv[i+1]);
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
