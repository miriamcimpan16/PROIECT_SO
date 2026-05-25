#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define PID_FILE ".monitor_pid"

void handle_signal(int sig) {
   if (sig == SIGUSR1) {
   const char msg[] = "EVENT:Raport nou detectat!\n";
  write(STDOUT_FILENO, msg, sizeof(msg)-1); 
  
  } else if (sig == SIGINT) {
  const char msg[] = "\nSTATUS:Inchidere monitor si stergere fisier.\n";
  write(STDOUT_FILENO, msg, sizeof(msg)-1);
  unlink(PID_FILE); // Sterge fisierul .monitor_pid
  _exit(0);
}
}

int main(void)
{
  //verficam daca un alt monitor ruleaza deja
  int fd2 = open(PID_FILE,O_RDONLY,0644);
  if(fd2 != -1)
  {
    char pid_buf[16];
        ssize_t n = read(fd2, pid_buf, sizeof(pid_buf) - 1);
        close(fd2);
        
        if (n > 0) {
            pid_buf[n] = '\0';
            int existing_pid = atoi(pid_buf);
            
            // Verificăm dacă procesul cu acel PID chiar există în sistem
            if (kill(existing_pid, 0) == 0) {
                // Trimitem eroarea formatată clar prin pipe către city_hub
                char err_msg[128];
                int err_len = snprintf(err_msg, sizeof(err_msg), 
                                       "ERROR: Monitorul ruleaza deja cu PID-ul %d\n", existing_pid);
                write(STDOUT_FILENO, err_msg, err_len);
                exit(EXIT_FAILURE); // Ne oprim aici
            }
            else{
              //pid-ul este vechi(procesul a murit), stergem fisieru ramas orfan
              unlink(PID_FILE);
            }
        }
  }
  //crearea fisierului PID pt noul proces
  int fd = open(PID_FILE,O_WRONLY|O_CREAT|O_TRUNC,0644);
  if(fd == -1)
    {
      printf("eroare la creare fisierului");
      exit(EXIT_FAILURE);
    }
  char buffer[16];
  int len = sprintf(buffer,"%d",getpid());
  write(fd,buffer,len);
  close(fd);
  //setarea semnalelor
  struct sigaction sa;
  memset(&sa,0,sizeof(struct sigaction)); // curatam memoria
  //sigaction(signal type, struct sigaction(new), where to return the old handler info(old))
  sa.sa_handler = handle_signal;
  sigemptyset(&sa.sa_mask); //nu avem semnale blocate in timpul functiei handle_signal
  sa.sa_flags = SA_RESTART; //previne intreruperea altor apeluri de sistem
  //verificam daca legarea semnalului a reusit
 if(sigaction(SIGUSR1, &sa, NULL) < 0)
 {
     perror("Eroare la sigaction pentru SIGUSR1");
     unlink(PID_FILE);
     exit(EXIT_FAILURE);
 }
 //SIGINT
 struct sigaction sa_int;
    memset(&sa_int, 0, sizeof(struct sigaction));
    sa_int.sa_handler = handle_signal;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    //verificam daca legarea semnalului a reusit
    if(sigaction(SIGINT, &sa, NULL) < 0)
    {
        perror("Eroare la sigaction pentru SIGINT");
        unlink(PID_FILE);
        exit(EXIT_FAILURE);
    }

 const char msg[] = "STATUS:Monitor activ. Asteapta SIGUSR1/SIGINT.\n"; 
 write(STDOUT_FILENO,msg,sizeof(msg) - 1);

   while (1) {
  pause(); 
   }

 
  return 0;
}
