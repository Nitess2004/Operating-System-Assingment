 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <unistd.h>
 
 #define THREADS 3
 #define TURNS   2
 #define NUM_ACCOUNTS 2
 

 int balance[NUM_ACCOUNTS] = {1000, 1000};  
 pthread_mutex_t acct_lock[NUM_ACCOUNTS];
 

 int current = 0;
 pthread_mutex_t turn_lock;
 pthread_cond_t  turn_cv;
 
 
 void lock_accounts_in_order(int a, int b) {
     int first  = (a < b) ? a : b;
     int second = (a < b) ? b : a;
     pthread_mutex_lock(&acct_lock[first]);
     if (first != second)
         pthread_mutex_lock(&acct_lock[second]);
 }
 
 void unlock_accounts_in_order(int a, int b) {
     int first  = (a < b) ? a : b;
     int second = (a < b) ? b : a;
     if (first != second)
         pthread_mutex_unlock(&acct_lock[second]);
     pthread_mutex_unlock(&acct_lock[first]);
 }
 
 void *customer(void *arg) {
     int id = *(int *)arg;
     int from_acct = id % NUM_ACCOUNTS;
     int to_acct   = (id + 1) % NUM_ACCOUNTS;
 
     for (int turn = 0; turn < TURNS; turn++) {
 
    
         pthread_mutex_lock(&turn_lock);
         while (current != id)
             pthread_cond_wait(&turn_cv, &turn_lock);   
         pthread_mutex_unlock(&turn_lock);
 
         printf("[Turn %d] Customer %d is scheduled (quantum start)\n", turn, id + 1);
 
        
         lock_accounts_in_order(from_acct, to_acct);
 
         if (balance[from_acct] >= 50) {
             balance[from_acct] -= 50;
             balance[to_acct]   += 50;
             printf("Customer %d transferred $50: Acct%d=%d, Acct%d=%d\n",
                    id + 1, from_acct, balance[from_acct], to_acct, balance[to_acct]);
         } else {
             printf("Customer %d: insufficient funds in Acct%d\n", id + 1, from_acct);
         }
 
         usleep(200000); 
 
         unlock_accounts_in_order(from_acct, to_acct);
 

         pthread_mutex_lock(&turn_lock);
         current = (current + 1) % THREADS;
         pthread_cond_broadcast(&turn_cv);
         pthread_mutex_unlock(&turn_lock);
     }
     return NULL;
 }
 
 int main(void) {
     pthread_t t[THREADS];
     int id[THREADS];
 
     pthread_mutex_init(&turn_lock, NULL);
     pthread_cond_init(&turn_cv, NULL);
     for (int i = 0; i < NUM_ACCOUNTS; i++)
         pthread_mutex_init(&acct_lock[i], NULL);
 
     printf("=== Bank ATM Round Robin Simulation ===\n");
     printf("Starting balances: Acct0=%d, Acct1=%d\n\n", balance[0], balance[1]);
 
     for (int i = 0; i < THREADS; i++) {
         id[i] = i;
         pthread_create(&t[i], NULL, customer, &id[i]);
     }
     for (int i = 0; i < THREADS; i++)
         pthread_join(t[i], NULL);
 
     printf("\nFinal balances: Acct0=%d, Acct1=%d\n", balance[0], balance[1]);
 
     pthread_mutex_destroy(&turn_lock);
     pthread_cond_destroy(&turn_cv);
     for (int i = 0; i < NUM_ACCOUNTS; i++)
         pthread_mutex_destroy(&acct_lock[i]);
 
     return 0;
 }
