/*
 * ============================================================================
 *  PROCESSI, FORK, WAITPID E PIPE - CODICE COMMENTATO
 * ============================================================================
 *
 *  DESCRIZIONE GENERALE
 *  --------------------
 *  Il programma crea una piccola gerarchia di processi:
 *
 *        padre
 *        |-- rosa   (figlio del padre)
 *        |    `-- dalia (figlio di rosa, quindi "nipote" del padre)
 *        `-- viola  (figlio del padre)
 *
 *  - dalia dorme 3 secondi e termina con codice di uscita 12.
 *  - rosa crea dalia, la aspetta, poi dorme 1 secondo e termina con codice 1.
 *  - viola genera un numero casuale (tra 20 e 30) e lo manda al padre
 *    tramite una PIPE, poi dorme 2 secondi e termina con codice 0.
 *  - il padre legge il numero dalla pipe, poi aspetta la terminazione
 *    di rosa e di viola, stampando il loro codice di ritorno.
 *
 *  CONCETTI CHIAVE
 *  ---------------
 *  - fork()    : duplica il processo. Restituisce 0 nel figlio, il PID del
 *                figlio nel padre, -1 in caso di errore.
 *  - pipe()    : crea un canale unidirezionale di comunicazione tra processi.
 *                fd[0] = estremo di LETTURA, fd[1] = estremo di SCRITTURA.
 *  - waitpid() : blocca il processo finche' il figlio indicato non termina
 *                e ne recupera lo "status" di uscita.
 *  - WEXITSTATUS(status) : estrae dallo status il valore passato a exit().
 *  - exit(n)   : termina il processo con codice di uscita n.
 */

#include <stdio.h>      /* printf */
#include <stdlib.h>     /* exit, rand */
#include <sys/wait.h>   /* waitpid, WEXITSTATUS */
#include <time.h>       /* (non usata: servirebbe per srand(time(NULL))) */
#include <unistd.h>     /* fork, pipe, read, write, close, sleep, getpid, getppid */

int main(void){
	/* pRosa, pDalia, pViola: contengono i PID dei figli restituiti da fork().
	 * status: variabile dove waitpid() salva lo stato di terminazione. */
	int pRosa,pDalia,pViola,status;

	/* fd[0] = lato lettura della pipe, fd[1] = lato scrittura.
	 * La pipe va creata PRIMA dei fork(), cosi' viene ereditata dai figli
	 * (ogni figlio riceve una copia dei file descriptor). */
	int fd[2];
	
	
	/* Creazione della pipe. In caso di errore pipe() restituisce -1. */
	if(pipe(fd)==-1){
		printf("errore pipe\n");
		exit(1);
	}
	
	/* getpid() = PID del processo corrente, getppid() = PID del suo padre.
	 * Qui stampa il PID del processo padre e il PID della shell che lo ha lanciato. */
	printf("padre %d by %d\n",getpid(),getppid());

	/* ---------------------------- CREAZIONE DI ROSA -------------------------- */
	/* Dopo fork() esistono DUE processi che eseguono lo stesso codice:
	 *   - nel figlio (rosa) pRosa vale 0
	 *   - nel padre pRosa vale il PID di rosa */
	pRosa=fork();
	if(pRosa==0){
		/* ===== CODICE ESEGUITO SOLO DA ROSA ===== */
		/* Nota: il messaggio dice "delay 1s", ma lo sleep(1) di rosa avviene
		 * piu' in basso, DOPO aver aspettato dalia. */
		printf("\trosa %d by %d, delay 1s\n",getpid(),getppid());

		/* ---------------------- CREAZIONE DI DALIA --------------------------- */
		/* Rosa fa a sua volta fork(): dalia e' figlia di rosa, non del padre. */
		pDalia=fork();
		if(pDalia==0){
			/* ===== CODICE ESEGUITO SOLO DA DALIA ===== */
			printf("\t\tdalia %d by %d, delay 3s\n",getpid(),getppid());
			sleep(3);   /* dalia resta in attesa 3 secondi */
			exit(12);   /* termina con codice di uscita 12 */
		}

		/* ===== DI NUOVO CODICE DI ROSA ===== */
		/* Rosa si blocca finche' dalia non termina (circa 3 secondi). */
		waitpid(pDalia,&status,0);
		/* Recupera il codice passato a exit() da dalia: 12. */
		int valDalia=WEXITSTATUS(status);
		printf("\t\tdalia terminato, return %d\n",valDalia);
		printf("\trosa (%d) ha aspettato dalia (%d) con ret %d\n",getpid(),pDalia,valDalia);
		sleep(1);   /* rosa attende ancora 1 secondo prima di terminare */
		exit(1);    /* rosa termina con codice 1 */
	}
	
	/* ---------------------------- CREAZIONE DI VIOLA ------------------------- */
	/* Questa riga e' eseguita solo dal PADRE: rosa e' terminata con exit() e
	 * non arriva mai qui. Viola e' quindi figlia del padre, "sorella" di rosa. */
	pViola=fork();
	if(pViola==0){
		/* ===== CODICE ESEGUITO SOLO DA VIOLA ===== */
		printf("\tviola %d by %d, delay 2s\n",getpid(),getppid());

		/* Viola SCRIVE soltanto, quindi chiude il lato di lettura che non usa. */
		close(fd[0]);

		/* Numero "casuale" compreso tra 20 e 30:
		 *   rand()%11 -> valore da 0 a 10
		 *   +20       -> valore da 20 a 30
		 * ATTENZIONE: non viene chiamata srand(), quindi la sequenza e' sempre
		 * la stessa a ogni esecuzione (con glibc il primo valore e' di solito 26).
		 * Per avere numeri diversi servirebbe srand(time(NULL)) all'inizio. */
		int rnd=rand()%11+20;

		/* Scrive nella pipe i byte dell'intero (sizeof(int) byte). */
		write(fd[1],&rnd,sizeof(int));
		printf("\tviola invia a padre %d\n",rnd);

		/* Chiude anche il lato di scrittura: ha finito di comunicare. */
		close(fd[1]);

		sleep(2);   /* viola resta in attesa 2 secondi */


		exit(0);    /* termina con codice 0 (successo) */
	}
	
	/* ============================ CODICE DEL PADRE ============================ */

	/* Il padre LEGGE soltanto, quindi chiude il lato di scrittura.
	 * E' importante: una read() su una pipe vede la "fine dati" (EOF) solo
	 * quando TUTTI gli estremi di scrittura sono chiusi. */
	close(fd[1]);

	int input;
	/* read() e' BLOCCANTE: attende finche' nella pipe non ci sono dati.
	 * Restituisce il numero di byte letti (>0 se ha letto qualcosa). */
	if(read(fd[0],&input,sizeof(int))>0){
		printf("padre riceve %d da viola\n",input);
	}
	/* Il padre ha finito con la pipe: chiude anche il lato di lettura. */
	close(fd[0]);
	
	/* Il padre attende la terminazione di rosa.
	 * Rosa termina dopo circa 3s (attesa dalia) + 1s (suo sleep) = ~4 secondi. */
	waitpid(pRosa,&status,0);
	int valRosa=WEXITSTATUS(status);   /* codice di uscita di rosa: 1 */
	printf("\trosa terminato, return %d\n",valRosa);
	printf("padre (%d) ha aspettato rosa (%d) con ret %d\n",getpid(),pRosa,valRosa);

	/* Poi attende viola. Viola e' gia' terminata da un pezzo (dopo ~2s),
	 * quindi e' uno "zombie": waitpid() ritorna subito e ne "raccoglie" lo stato. */
	waitpid(pViola,&status,0);
	int valViola=WEXITSTATUS(status);  /* codice di uscita di viola: 0 */
	printf("\tviola terminato, return %d\n",valViola);
	printf("padre (%d) ha aspettato viola (%d) con ret %d\n",getpid(),pViola,valViola);
	printf("fine padre\n");
		
	/* Nessun return esplicito: da C99 in poi, main() senza return
	 * equivale a return 0. */
} 


/*
 * ============================================================================
 *  TIMELINE DELL'ESECUZIONE (tempi approssimativi)
 * ============================================================================
 *
 *  t = 0s   padre stampa il suo PID e crea rosa e viola.
 *           rosa stampa il messaggio e crea dalia.
 *           dalia stampa il messaggio e inizia sleep(3).
 *           viola stampa, genera il numero, lo scrive nella pipe,
 *           stampa "viola invia a padre ..." e inizia sleep(2).
 *           il padre (sbloccato dalla read) stampa "padre riceve ... da viola"
 *           e poi si blocca in waitpid(pRosa).
 *
 *  t = 2s   viola termina con exit(0)  (diventa zombie finche' il padre
 *           non chiama waitpid su di lei).
 *
 *  t = 3s   dalia termina con exit(12). Rosa viene sbloccata dal waitpid,
 *           stampa il codice 12 di dalia e inizia sleep(1).
 *
 *  t = 4s   rosa termina con exit(1). Il padre viene sbloccato, stampa il
 *           codice 1 di rosa, poi fa waitpid su viola (immediato, codice 0)
 *           e stampa "fine padre".
 *
 *  OUTPUT ATTESO (l'ordine delle prime righe puo' variare, perche' lo
 *  scheduler decide quale processo parte per primo):
 *
 *     padre 1000 by 900
 *         rosa 1001 by 1000, delay 1s
 *         viola 1003 by 1000, delay 2s
 *             dalia 1002 by 1001, delay 3s
 *         viola invia a padre 26
 *     padre riceve 26 da viola
 *             dalia terminato, return 12
 *         rosa (1001) ha aspettato dalia (1002) con ret 12
 *         rosa terminato, return 1
 *     padre (1000) ha aspettato rosa (1001) con ret 1
 *         viola terminato, return 0
 *     padre (1000) ha aspettato viola (1003) con ret 0
 *     fine padre
 *
 * ============================================================================
 *  OSSERVAZIONI E POSSIBILI MIGLIORAMENTI
 * ============================================================================
 *
 *  1. Il padre aspetta rosa PRIMA di viola, anche se viola termina prima:
 *     per questo "rosa terminato" compare prima di "viola terminato".
 *
 *  2. rand() senza srand(): il numero inviato e' sempre lo stesso.
 *     Soluzione: chiamare srand(time(NULL)) (da qui l'utilita' di <time.h>).
 *     Se lo si facesse prima della fork, i figli erediterebbero lo stesso seme.
 *
 *  3. Non viene controllato il valore di ritorno di fork() (-1 = errore),
 *     ne' quello di write().
 *
 *  4. Rosa e dalia ereditano i descrittori della pipe e non li chiudono.
 *     Qui non crea problemi (il padre legge un solo int, non attende l'EOF),
 *     ma in un codice piu' completo andrebbero chiusi anche nei figli che
 *     non usano la pipe.
 *
 *  5. Buffering di printf: sul terminale l'output e' a righe, quindi tutto
 *     funziona. Se l'output viene reindirizzato su file o pipe, stdout diventa
 *     completamente bufferizzato e la fork() puo' duplicare il contenuto del
 *     buffer, producendo righe stampate due volte. Si evita con fflush(stdout)
 *     prima di ogni fork().
 *
 *  6. I messaggi "delay Ns" non corrispondono sempre all'istante reale
 *     dello sleep (vedi rosa): sono solo indicativi.
 */
