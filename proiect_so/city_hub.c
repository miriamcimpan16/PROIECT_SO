#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<signal.h>
char *list_district[100];
pid_t hub_mon_pid = 0;
void start_monitor_logic() {
    int pfd[2];//pfd[0] -> citit pfd[1] -> scris
    if (pipe(pfd) < 0) {
        write(STDERR_FILENO, "Eroare la pipe\n", 15);
        return;
    }

    hub_mon_pid = fork(); 
    //ii pasa doar ce trimite monitorul

    if (hub_mon_pid == 0) { 
        setpgid(0,0); //tot ce creeaza hub_mon face parte din grup
       //aici se afla procesul copil
      
        pid_t monitor_pid = fork(); // citeste pipe-ul in fundal
        
        if (monitor_pid == 0) { 
            
            close(pfd[0]); // Inchide citirea, el doar scrie
            dup2(pfd[1], STDOUT_FILENO); // Redirectioneaza output-ul in pipe
            close(pfd[1]);

            execl("./monitor_reports", "./monitor_reports", NULL);
            perror("Eroare la execl");
            exit(1);
        } else {
            
            
            close(pfd[1]); // Inchide scrierea, el doar asculta
            char buffer[512];
            ssize_t n;

            // Citim mesajele monitorului pe masura ce vin
            while ((n = read(pfd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[n] = '\0';
                write(STDOUT_FILENO, "\n[MONITOR]: ", 12);
                write(STDOUT_FILENO, buffer, n);
                write(STDOUT_FILENO, "\ncity_hub > ", 11); // Punem promptul inapoi
            }
            if(n == 0) write(STDOUT_FILENO,"Monitorul s-a oprit\n",21);
            close(pfd[0]);
            exit(0);
        }
    } else {
        //aici este procesul parinte
        close(pfd[0]);
        close(pfd[1]);
        
        write(STDOUT_FILENO, "Monitorul ruleaza in fundal...\n", 31);
    }
}
void calculate_scores_logic(char list_district[]){
    int pfd[2];//pfd[0] -> citit pfd[1] -> scris
    if (pipe(pfd) < 0) {
        write(STDERR_FILENO, "Eroare la pipe\n", 15);
        return;
    }
   pid_t scorer = fork();
   if(scorer == 0){
    close(pfd[0]);
    dup2(pfd[1],STDOUT_FILENO); // tot ce se scrie in scorer in write ajunge in pipe
    close(pfd[1]);
   }
   else{

   }
}
int main(void) {
    char comanda[100];
    const char prompt[] = "city_hub > ";

    while (1) {
        write(STDOUT_FILENO, prompt, sizeof(prompt) - 1);
        
        ssize_t n = read(STDIN_FILENO, comanda, sizeof(comanda) - 1);
        if (n <= 0) break;
        comanda[n - 1] = '\0'; // Scoatem \n

        if (strcmp(comanda, "start_monitor") == 0) {
            start_monitor_logic();
        } else if (strcmp(comanda, "exit") == 0) {
            if(hub_mon_pid > 0)
            {
                //atat hub_mon cat si monitoru pornit de el(-)
                kill(-hub_mon_pid,SIGINT);
            }
            write(STDOUT_FILENO,"Inchidere Hub\n",14);
            break;
        }
        else if(strncmp(comanda,"calculate_scores",16) == 0)
        {
           int nr_districte = 0;
           strtok(comanda," ");
           char *p = strtok(NULL," ");
           while(p != NULL && nr_districte < 100){
            list_district[nr_districte] = p;
            nr_districte++;
            p = strtok(NULL, " ");
           }
           for(int i = 0;i<nr_districte;i++)
           {
            
           }
           
        }
    }
    return 0;
}