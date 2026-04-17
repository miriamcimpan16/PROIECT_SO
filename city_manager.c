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
	char path_reports[MAX];
      snprintf(path_reports,MAX,"%s/reports.dat",district);
      int fd = open(path_reports,O_CREAT | O_RDWR,0664);
      if(fd == -1)
	{
	  printf("eroare la creare reports.dat");
	  exit(EXIT_FAILURE);
	}
      close(fd);
      chmod(path_reports,0664);
      
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
