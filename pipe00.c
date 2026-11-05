/*
 * 1. PIPE: Crea un canale di comunicazione
 * 2. PADRE (Producer): Invia una sequenza d
 *    uno al secondo, poi chiude il canale p
 * 3. FIGLIO (Consumer): Resta in ascolto ci
 *    non riceve l'EOF (End Of File) derivan
 *    lato padre, stampando ogni valore rice
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("Errore pipe");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("Errore fork");
        exit(1);
    }

    // =========================
    // PADRE → PRODUCER
    // =========================
    if (pid > 0) {
        close(fd[0]); // chiude lettura

        for (int i = 1; i <= 5; i++) {
            printf("[PADRE] Invio: %d\n", i);

            write(fd[1], &i, sizeof(int));

            sleep(1); // simula produzione lenta (
        }

        printf("[PADRE] Fine invio, chiudo pipe\n");

        close(fd[1]); // fondamentale → segnala
    }

    // =========================
    // FIGLIO → CONSUMER
    // =========================
    else {
        close(fd[1]); // chiude scrittura

        int valore;
        int n;

        printf("\t[FIGLIO] In attesa dati...\n");

        // ciclo continuo di lettura
        while ((n = read(fd[0], &valore, sizeof(int))) > 0) {
            printf("\t[FIGLIO] Ricevuto: %d\n", valore);
        }

        if (n == 0) {
            printf("\t[FIGLIO] EOF ricevuto, termino\n");
        } else { //<0
            perror("\tErrore read");
        }

        close(fd[0]);
    }

    return 0;
}
