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
#define descr 256
//stat(path,&struct_stat) --> 0 daca exista | -1 daca nu
//open pentru creare de fisiere
//structura pentru reports.dat
typedef struct report
{
  int id;
  char nume[MAX];
  float lat,lon;
  char issue[MAX];
  int severity;
  time_t timestamp;
  char description[descr];
}report;
//path-urile o sa le pun ca variabile globale
char path_reports[MAX]="";
char path_cfg[MAX]="";
char path_log[MAX]="";
char role[MAX] = "";
char user[MAX] = "";
float lat = 0,lon = 0;
int severity = 0;
char description[descr] = "";
char category[MAX] = "";
char district[MAX] = "";
void writing_in_reports(char district[],char role[],char user[])
{
  snprintf(path_reports,MAX,"%s/reports.dat",district);
  //verificarea daca owner are drept de scriere
  struct stat st;
  if(stat(path_reports,&st) == -1)
    {
      printf("eroare");
      exit(EXIT_FAILURE);
    }
  if(strcmp(role,"manager") == 0){
  if(!(st.st_mode & 0200))
    {
      printf("Manager ul nu are drept de scriere");
      exit(EXIT_FAILURE);
    }
  }
   if(strcmp(role,"inspector") == 0){
  if(!(st.st_mode & 0020))
    {
      printf("Inspector ul nu are drept de scriere");
      exit(EXIT_FAILURE);
    }
  }
  int fd  = open(path_reports,O_WRONLY|O_APPEND);
  if(fd == -1)
    {
      printf("eroare la deschiderea fisierului");
      exit(EXIT_FAILURE);
    }
  report element;
  int nr_rapoarte = st.st_size / sizeof(report);
  element.id = nr_rapoarte + 1; // cate rapoarte exista deja
  strcpy(element.nume,user);
  element.lat = lat; element.lon = lon;
  strcpy(element.issue, category);
  element.severity = severity;
  element.timestamp = time(NULL);
  strcpy(element.description, description);
 
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
      int fd = open(path_reports,O_CREAT | O_RDWR|O_EXCL,0664);
      //O_CREAT-> spune daca fisierul exista sau nu, daca nu exista este create
      //O_RDWR->deschide fisierul pentru citire si pentru scriere
      //O_EXCL -> esueaza daca fisierul exista deja
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
void comanda_add(char district[],char role[],char user[])
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
  writing_in_reports(district,role,user);
  //crearea legaturii
  char symlink_path[MAX];
  snprintf(symlink_path, MAX, "active_reports-%s", district);
  struct stat lst;
  if (lstat(symlink_path, &lst) == -1)  // doar dacă nu există deja
    symlink(path_reports, symlink_path);
  
}
int main(int argc,char *argv[])
{
  
  char command[MAX] = "";
  for(int i = 0;i<argc;i++)
    {

    if(strcmp(argv[i],"--role") == 0)
	{
	  strcpy(role,argv[i+1]);
	}
    if (strcmp(argv[i], "--user") == 0)
      {
        strcpy(user, argv[i+1]);
      }
    if (strcmp(argv[i], "--lat") == 0)
        lat = atof(argv[++i]);
    if (strcmp(argv[i], "--lon") == 0)
        lon = atof(argv[++i]);
    if (strcmp(argv[i], "--category") == 0)
        strcpy(category, argv[++i]);
    if (strcmp(argv[i], "--severity") == 0)
        severity = atoi(argv[++i]);
    if (strcmp(argv[i], "--description") == 0)
        strcpy(description, argv[++i]);
     if(strcmp(argv[i],"--add") == 0)
	{
	  strcpy(district,argv[++i]);
	   strcpy(command,"add");
	  
	 
	}
      
    }
   if(strlen(district) > 0)
    {
      if(strcmp(command,"add") == 0)
	{
	  comanda_add(district,role,user);
	}
    }
  return 0;
}
