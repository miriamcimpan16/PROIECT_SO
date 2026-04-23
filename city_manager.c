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
#define MAX_PATH 300 //pt caile fisierelor
#define MAX_LOG 500 //pt liniile de log
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
char path_reports[MAX_PATH]="";
char path_cfg[MAX_PATH]="";
char path_log[MAX_PATH]="";
char role[MAX] = "";
char user[MAX] = "";
float lat = 0,lon = 0;
int severity = 0;
int id = 0,id_cautat = 0;
char description[descr] = "";
char category[MAX] = "";
char district[MAX] = "";
int has_permission(mode_t mode,const char *role,char type)
{
  if(strcmp(role,"manager") == 0)
    {
      if (type == 'r') return mode & S_IRUSR;
      if (type == 'w') return mode & S_IWUSR;
      if (type == 'x') return mode & S_IXUSR;
    }
  if (strcmp(role, "inspector") == 0) {
        if (type == 'r') return mode & S_IRGRP;
        if (type == 'w') return mode & S_IWGRP;
        if (type == 'x') return mode & S_IXGRP;
    }
  return 0;
}
//transformarea bitilor in text
void mode_to_string(mode_t mode,char *str)
{
  strcpy(str,"---------");
  //pentru owner
  if(mode & S_IRUSR) str[0] = 'r';
  if(mode & S_IWUSR) str[1] = 'w';
  if (mode & S_IXUSR) str[2] = 'x'; 
  
  //pentru group
  if (mode & S_IRGRP) str[3] = 'r'; 
  if (mode & S_IWGRP) str[4] = 'w'; 
  if (mode & S_IXGRP) str[5] = 'x'; 
  
  //pentru others
  if (mode & S_IROTH) str[6] = 'r'; 
  if (mode & S_IWOTH) str[7] = 'w'; 
  if (mode & S_IXOTH) str[8] = 'x'; 
  str[9] = '\0';
  
}
void format_time(time_t t,char *buf,size_t len)
{
  //pentru a transforma variabila timp intr o data pe an luna zi ora minut..
  struct tm *tm_info = localtime(&t);
  //formateaza intr un string care nu depaseste len
  strftime(buf,len,"%Y-%m-%d %H:%M:%S",tm_info);
}
void check_write_permission(const char *path, const char *role) {
    struct stat st;
    //in st se afla permisiuni,tip fisier,dimensiune
    if (stat(path, &st) == -1) {
        perror("Eroare stat");
        exit(EXIT_FAILURE);
    }
    if(!has_permission(st.st_mode,role,'w'))
      {
	printf("Rolul %s nu are permisiunea de scriere in fisier\n",role);
	exit(EXIT_FAILURE);
      }
}
void check_read_permission(const char *path,const char *role)
{
  struct stat st;
  if(stat(path,&st) == -1){
    printf("eroare la citire");
    exit(EXIT_FAILURE);
  }
  if(!has_permission(st.st_mode,role,'r'))
  {
    printf("Rolul %s nu are permisiunea de citire in fisier\n",role);
    exit(EXIT_FAILURE);
  }
}
void writing_in_reports()
{
  snprintf(path_reports,MAX_PATH,"%s/reports.dat",district);
  //verificarea daca owner are drept de scriere
  check_write_permission(path_reports,role);
  struct stat st;
  if (stat(path_reports, &st) == -1) {
        perror("Eroare la obținerea dimensiunii fișierului\n");
        exit(EXIT_FAILURE);
    }
  int fd  = open(path_reports,O_WRONLY|O_APPEND);
  if(fd == -1)
    {
      printf("Eroare la deschiderea fisierului\n");
      exit(EXIT_FAILURE);
    }
  report element;
  int nr_rapoarte = st.st_size / sizeof(report); //cate rapoarte exista deja
  element.id = nr_rapoarte + 1; //noul id va fi urmatorul numar
  strcpy(element.nume,user);
  element.lat = lat; element.lon = lon;
  strcpy(element.issue, category);
  element.severity = severity;
  element.timestamp = time(NULL);
  strcpy(element.description, description);
 
  if((write(fd,&element,sizeof(element)) == -1))
     {
       printf("Eroare la scrierea in fisier\n");
       exit(EXIT_FAILURE);
			   
     }
  close(fd);
  
}
void writing_in_logged_district()
{
  
  check_write_permission(path_log,role);
  //deschid fisierul pt a putea scrie in el
  int fd_log = open(path_log, O_WRONLY | O_APPEND);
  char log_entry[MAX_LOG];//ce va aparea in prop finala
  time_t now = time(NULL); //timpul curent 
  char time_str[64];//string ul cu formatul timpului
  format_time(now,time_str,sizeof(time_str)); 
  snprintf(log_entry,MAX_LOG,"[%s] role = %s user = %s action = add\n"
  ,time_str,role,user);
  write(fd_log,log_entry,strlen(log_entry));
  close(fd_log);

}
void creare_reports(char district[])
{
  
  //CREARE FISIERULUI REPORTS.DAT
      
  snprintf(path_reports,MAX_PATH,"%s/reports.dat",district);
  int fd = open(path_reports,O_CREAT | O_RDWR|O_EXCL,0664);
      //O_CREAT-> spune daca fisierul exista sau nu, daca nu exista este create
      //O_RDWR->deschide fisierul pentru citire si pentru scriere
      //O_EXCL -> esueaza daca fisierul exista deja
  if(fd == -1)
	{
	  printf("Eroare la crearea fisierului reports.dat\n");
	  exit(EXIT_FAILURE);
	}
  close(fd);
  chmod(path_reports,0664); // pun permsiunile inca odata, desi le am pus la open, pt sigurata,deoarece pot fi modificate de catre sistem
 
}
void creare_district(char district[])
{
 
  //CREAREA FISIERULUI DISTRICT.CFG
      
  snprintf(path_cfg, MAX_PATH, "%s/district.cfg", district); 
  int fd = open(path_cfg, O_CREAT | O_RDWR, 0640);
  if(fd == -1) {
	printf("Eroare la crearea district.cfg\n"); 
	exit(EXIT_FAILURE);
      }
      close(fd);
      chmod(path_cfg, 0640);
 
}
void creare_logged(char district[])
{
 
    //CREAREA FISIERULUI LOGGED_DISTRICT
      
  snprintf(path_log, MAX_PATH, "%s/logged_district", district);
  int fd = open(path_log, O_CREAT | O_RDWR, 0644);
  if (fd == -1)
    {
      printf("Eroare la crearea fisierului logged_district");
      exit(EXIT_FAILURE);
    }
  close(fd);
  chmod(path_log, 0644);
  
}
void creating_the_link()
{
  //cream numele legaturii
  char symlink_path[MAX_PATH];
  snprintf(symlink_path,MAX_PATH,"active_reports-%s",district);
  struct stat lst;
  struct stat target_st;
  //verificam cu lstat() daca leg simbolica exista
  if(lstat(symlink_path,&lst) == 0)
  {
    //daca am ajuns aici, exista
    //cu stat verificam daca destinatia ei exista
    if(stat(symlink_path,&target_st) == -1)
    {
      //destinatia nu mai exista
      unlink(symlink_path); //stergem legatura si apoi o refacem
      if(symlink(path_reports,symlink_path) == -1)
      {
        printf("Eroare la recrearea legaturii simbolice\n");
        exit(EXIT_FAILURE);
      }
    }
  }
    else{
      //daca am ajuns aici, legatura nu exista deloc
      if(symlink(path_reports,symlink_path) == -1)
      {
        printf("Eroare la crearea legaturii simbolice\n");
      }
    }
  
  
}
void comanda_add(char district[],char role[],char user[])
{
  struct stat st;
  if(stat(district,&st) == -1)
    {
      //folderul-->cu permisiunile: 0750
      if((mkdir(district,0750)) == -1)
	{
	  printf("Eroare la crearea dosarului\n");
	  exit(EXIT_FAILURE);
	}
  chmod(district,0750);  
	//district/reports.dat
	//district/district.cfg
	//district/logged_district
  creare_reports(district);
  creare_district(district);
  creare_logged(district);
  }
  writing_in_logged_district();//inregistrez actiunea
  writing_in_reports();
  creating_the_link();
}
void comanda_view(int id)
{
  check_read_permission(path_reports,role);
  //deschid fisierul doar pt citire->O_RDONLY
  int fd = open(path_reports,O_RDONLY);
  if(fd == -1)
  {
    printf("Eroare la deschiderea fisierului\n");
    exit(EXIT_FAILURE);
  }
  report element;
  int gasit = 0;
  while(read(fd,&element,sizeof(report)) > 0)
  {
    if(element.id == id)
    {
      //pt a converti time_t in string
      char *time_str = ctime(&element.timestamp);
      printf("--- Detalii Raport ID: %d ---\n", element.id);
      printf("Inspector: %s\n", element.nume);
      printf("Coordonate: GPS(%.4f, %.4f)\n", element.lat, element.lon);
      printf("Categorie: %s\n", element.issue);
      printf("Severitate: %d\n", element.severity);
      printf("Descriere: %s\n", element.description);
      printf("Adaugat la: %s", time_str);
      gasit = 1;
      break;
    }
  }
  if(!gasit){
    printf("Raportul cu ID %d nu a fost gasit in districtul %s.\n", id, district);
  }
}
void comanda_list()
{ 
     struct stat st;
     char perm_str[MAX];
     //extragerea datelor
     check_read_permission(path_reports,role);
     if(stat(path_reports,&st) == -1)
     {
      printf("eroare la citirea datelor");
      exit(EXIT_FAILURE);
     }
     mode_to_string(st.st_mode,perm_str);
     //ultima modificare
     char time_str[64];
     format_time(st.st_mtime,time_str,sizeof(time_str));
     printf("FISIER: %s\nPERMISIUNI: %s\nDIMENSIUNE: %ld bytes\nULTIMA MODIFICARE: %s\n",
    path_reports,perm_str,st.st_size,time_str
    );
    //citirea rapoartelor
    int fd = open(path_reports, O_RDONLY);
     if(fd == -1) {
         perror("Eroare la deschidere reports.dat");
         exit(EXIT_FAILURE);
     }
     report element;
     int count = 0;
     while(read(fd, &element, sizeof(report)) > 0) {
         printf("ID: %d | Cat: %s | Sev: %d | Inspector: %s\n", element.id, element.issue, element.severity, element.nume);
         count++;
     }
     
     if(count == 0) {
         printf("Niciun raport gasit in acest district.\n");
     }

     close(fd);

}
int main(int argc,char *argv[])
{
  
  char command[MAX] = "";
  for(int i = 0;i<argc;i++)
    {
  if((i+1) < argc){
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
   if(strcmp(argv[i],"--list") == 0)
  {
    strcpy(district, argv[++i]);
    strcpy(command, "list");
  }
  if(strcmp(argv[i],"--view") == 0)
  {
    strcpy(district, argv[++i]);
    id_cautat = atoi(argv[++i]);
    strcpy(command, "view");
  }
      
  }
}
snprintf(path_reports,MAX_PATH,"%s/reports.dat",district);
snprintf(path_log, MAX_PATH, "%s/logged_district", district);
snprintf(path_cfg, MAX_PATH, "%s/district.cfg", district);
   if(strlen(district) > 0)
    {
      if(strcmp(command,"add") == 0)
	{
	  comanda_add(district,role,user);
	}
  if(strcmp(command,"view") == 0)
  {
    comanda_view(id_cautat);
  }
  if(strcmp(command,"list") == 0)
  {
    comanda_list();
  }
    }
  return 0;
}
