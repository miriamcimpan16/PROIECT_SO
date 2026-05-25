#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

char list_district[100][256];  
int nr_districte = 0;
pid_t hub_mon_pid = 0;

void start_monitor_logic() {
    int pfd[2];
    if (pipe(pfd) < 0) {
        write(STDERR_FILENO, "Eroare la pipe\n", 15);
        return;
    }

    hub_mon_pid = fork();

    if (hub_mon_pid == 0) {
        pid_t monitor_pid = fork();

        if (monitor_pid == 0) {
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            close(pfd[1]);
            char *args[] = {"./monitor_reports", NULL};
            execvp(args[0], args);
            perror("Eroare la execvp");
            exit(1);
        } else {
            close(pfd[1]);
            char buffer[512];
            ssize_t n;
            while ((n = read(pfd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[n] = '\0';
                write(STDOUT_FILENO, "\n[MONITOR]: ", 12);
                write(STDOUT_FILENO, buffer, n);
            }
            if (n == 0) write(STDOUT_FILENO, "Monitorul s-a oprit\n", 20);
            close(pfd[0]);
            //asteptam ca fiul sa termine
            waitpid(monitor_pid,NULL,0);
            exit(0);
        }
    } else {
        //Părintele închide ambele capete — el nu folosește pipe-ul
        close(pfd[0]);
        close(pfd[1]);
        write(STDOUT_FILENO, "Monitorul ruleaza in fundal...\n", 31);
    }
}

void calculate_scores_logic(char list_district[][256]) {
    for (int i = 0; i < nr_districte; i++) {

        int pfd[2];  //pipe nou pentru fiecare district, nu unul singur reutilizat
        if (pipe(pfd) < 0) {
            write(STDERR_FILENO, "Eroare la pipe\n", 15);
            return;
        }

        
        pid_t scorer = fork();
        if (scorer < 0) {
            perror("Eroare la fork pentru scorer");
            continue; //trecem la urmatorul district
        }
        if (scorer == 0) {
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            close(pfd[1]);
            char *args[] = {"./scorer", list_district[i], NULL};
            execvp(args[0], args);
            perror("Eroare la execvp");
            exit(1);
        } else {
            close(pfd[1]);  //închidem doar capătul de scriere al acestui pipe

            char buffer[1024];
            ssize_t n;
            // buclă în loc de un singur read — scorer-ul poate scrie mai mult de 1024 bytes
            while ((n = read(pfd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[n] = '\0';
                write(STDOUT_FILENO, buffer, n);
            }

            close(pfd[0]);
            //ne asiguram ca asteptam exact procesul corect
            waitpid(scorer,NULL,0);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {  
        write(STDERR_FILENO, "Utilizare: ./city_hub <comanda> [districte...]\n", 46);
        return 1;
    }

    char comanda[100];
    strcpy(comanda, argv[1]);

    if (strcmp(comanda, "start_monitor") == 0) {
        start_monitor_logic();
    }
    else if (strcmp(comanda, "calculate_scores") == 0) {
        for (int i = 2; i < argc; i++) {
            strcpy(list_district[nr_districte], argv[i]);  
            nr_districte++;
        }
        calculate_scores_logic(list_district);
    }
    else{
        write(STDERR_FILENO, "Comanda necunoscuta.\n", 21);
        return 1;
    }

    return 0;
}