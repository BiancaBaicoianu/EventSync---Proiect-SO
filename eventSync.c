#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>

sem_t event;
//declaram o variabila de timp semafor 


//functia eventopen initializeaza variabila semafor cu 0 si este apelata o data la inceputul programului
void eventopen() {
    if (sem_init(&event, 0, 0) != 0) {
    	//tratam eroarea de initializare esuata
        perror("Error: sem_init() failed\n");
        return errno;
    }
}


//functia eventclose distruge variabila semafor la finalul programului
void eventclose() {
    if (sem_destroy(&event) != 0) {
    	//tratam eroarea
        perror("Error: sem_destroy() failed\n");
        return errno;
    }
}

//functia eventwait decrementeaza variabila semafor. Daca valoarea este mai mica decat 0, blocheaza thread-ul apelat
void eventwait() {
	if (sem_wait(&event) != 0) {
	    //tratam eroarea pentru sem_wait esuat
	    perror("Error: sem_wait() failed\n");
	    return errno;
	}
        
}


//functia eventsignal incrementeaza valoarea variabilei semafor. Daca exita un thread blocat in eventwait, il deblocheaza si ii permite sa isi continuie executia
void eventsignal() {
    
    if (sem_post(&event) != 0) {
        //tratam eroarea
        perror("Error: sem_post() failed\n");
        return errno;
    }
}


//functia thread_function este functia executata de fiecare thread. Apeleaza eventwait care v-a bloca thread-ul pana cand evenimentul este semnalat
void *thread_function(void *arg) {
    eventwait();
    printf("Event was signaled!\n");
    return NULL;
}

int main() {
    //initializam semaforul
    eventopen();
    
    //creeam thread-urile si le atribuim executarea functiei thread_function
    pthread_t thread1, thread2;
    int ret1 = pthread_create(&thread1, NULL, thread_function, NULL);
    int ret2 = pthread_create(&thread2, NULL, thread_function, NULL);

    //tratam erori de creare thread
    if (ret1 != 0 || ret2 != 0) {
        perror("Error: pthread_create() failed\n");
        return errno;
    }
    printf("Waiting for event...\n");
    //asteapta 5 secunde
    sleep(5);
    
    //apelam eventsignal de 2 ori ca sa deblocam ambele thread-uri blocate de eventwait 
    eventsignal();
    eventsignal();
    
    //asteptam ca ambele theread-uri sa se finalizeze
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    
    //inchidem semaforul 
    eventclose();
    
    return 0;
}