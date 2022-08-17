/**
El programa 3 unido junto al 2 , simulan un despachador de procesos, con una comunicaion entre ellos con sockets
, memoria compartida y semaforos, este programa en especial se encarga de despachar los procesos en cuanto  detecte 
que ya hay en la memoria compartida, es decir, una vez que ya el programa 2 haya subido algunos procesos, el programa 3 
y el programa2 entraran en un proceso de sincronizaciòn, exclusiòn mutua para tener acceso a la memoria y esto se logra controlar 
con semaforos, en fin si el prgrama 3 se ejecuta si no detecta nada, sigue espoerando hasta que el 2 ya haya escrito algo en la memoria
ESTE MODELO DE DESPÀCHADOR SE PARECE A UN MODELO :::::::::::PRODUCTOR-CONSUMIDOR:::::::::   EN DONDE EL PROGRAMA 2 ES EL PRODUCTOR 
Y EL 3 ES EL CONSUMIDOR, de igual manera se puede concluir que el programa 2 funciona como despachador  a corto plazo y el programa 
3 como despachador a largo plazo 

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <sys/sem.h>

#define SEMPERM 0600
/*VARIABLES GLOBALES*/
int TEsp = 0; // tiempo de espera total que se lleva durante el despachador
int pFinish = 0;
int procesos = 0;
int BAN= 0;
int shm;
FILE* flujo;
 key_t nprocesos = 0; // se inicializa en 1 para que epueda entrara al ciclo antes de que la variable tome valores
int *keyCont = 0;
 int semid;
 
 /*STRUCTURA DE NODO COMPARTIDO*/
struct nodo{
  int prioridad;
  int dato;
  int tr;
  int te;
  char cadena[20];
  key_t next;
} nodo;

//prototipo de funciones
void limpiarShm();
void despachador_por_prioridades(key_t key);
int initsem(key_t semkey);
int p(int semid);
int v(int semid);
void limpiarShm2(key_t key);
void limpiarS();
void senalTrata();

void errorHandler(char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}


//CREA UN NODO COMPARTIDO CON LA LLAVE DE MEMORIA COMPARTIDA
struct nodo *getNode(key_t key){
  int shmid = shmget(key, sizeof(struct nodo), IPC_CREAT | 0660);
  if(shmid==-1){
    errorHandler("[-]SHMGET Fall");
  }
  struct nodo *req = shmat(shmid, NULL, 0);
  if(req==(void *)-1){
    errorHandler("[-]SHMAT Fallo");
  }
  return req;
}


 //////********/
int main(int argc, char *argv[]){
	//---------
	  key_t keyAux = 10000;//shm variable
	  //int shm;
	  if((shm=shmget(keyAux,sizeof(int),0600|IPC_CREAT))==-1){  // memoria compartida que se creop en prog. 2 (acceso con llave)
	    perror("shmget fallo");
	    exit(1);
	  }
	  if((keyCont=(int *)shmat(shm, 0,0))==(int *)-1) {  // adjunta el segmento de memoria compartida al espacio de direcciones de la llamada 
	    perror("shmat fallo");
	    exit(1);
	  }
	  //--------- // semaforos para acceder a memoria compartida
	  key_t semkey = 5000;//Llave del semaforo
	  int pid;
	  if((semid = initsem(semkey)) < 0){
	    exit(1);
	  }

    key_t key = 1; // llave de memoria compartida
    //_----------
  limpiarShm();
  /////////////////////////////
  //printf("\nAntes de la regiÃ³n critica\n", pid);
  key_t auxKey;
  key_t aux = key; 	
  flujo= fopen("datos_salida.txt", "w+"); // apertura de archivo donde se guardaran los cambios que despachador hara en los procesos
  //return 0;
  if(flujo == NULL){
	printf("\nError al tratar de guardar datos\n");}
  else{
  //para entrar al despachador entra en un ciclo infinito que solo se rompe con una señal, al lanzar un ***10*** desde terminal
      printf("\nIniciando despachador de procesos (calendarización por prioridades) ...\n");
	  sleep(4);
	  while(1){ // despacha poroceso por proceso que encuentre en la memoria
	      struct nodo *head = getNode(key);
	      auxKey = head->next;
	      if(auxKey != 0){
	      	   v(semid); // entra a la memoria compartida 
		       despachador_por_prioridades(aux); // a despachador se le pasa como parametro la llave de cada nodo
		       struct nodo *final = getNode(aux);
		       nprocesos = aux;
		       if(final->next == 0){ // cuando se llegue al final de la lista compartida, se vuelve a empezar para recorrer los nodos que hacen falta
		       	   //printf("TERMINO UNA VUELTA EN LA LISTA \n");
		       	   aux = key;
			   }else aux = final->next;
			   signal(SIGUSR1, senalTrata);
		       p(semid);  // sale de la memoria compartida
		  }
	  }
  }
  return 0;
}

// trata la señal 10 cuando esta es lanzada desde terminal
void senalTrata(){
	printf("entra a tratar senal  \n");
	printf("Procesos despachados en total :  %d \n", pFinish);
	fflush(flujo); // cierra fichero
	fclose(flujo);
	limpiarS(); //  elimina los recursos utilizados, tales como memoria y semaforos
	exit(0);
}


// limpia la estructura compartida /**********************PODRIAS CHECAR SI ENCERIO LO HACE***/
void limpiarShm(){
  int a;
  int shmid;
  key_t key = 1;
  
  while(key != 0){
  	struct nodo *ya = getNode(key);
    shmid = shmget(key, sizeof(struct nodo), IPC_CREAT | 0660);
    shmctl(shmid,IPC_RMID,(struct shmid_ds *)0); // borra lo que contenga la mem. compartida
    key = ya->next;
    //printf("Limpiar %d \n", key);
  }
  printf("Se han borrado %d nodos de la memoria compartida\n",key);
}

// elimina recursos utilizados
void limpiarS(){ // borra todos los recursos utilizados 
   //shmdt(estado)
   if(shmdt(keyCont) == -1){ // se des- ata memoria de donde fue atada con shmat
    printf(" error des atar\n");
  }
  if(shmctl (shm, IPC_RMID , 0) < 0)
    printf(" error eliminar mem shm \n");
  if(semctl(semid, 0, IPC_RMID,0) < 0)
    printf(" error eliminar mem sem \n");
}


// inicia semaforos 
int initsem(key_t semkey){
  int status = 0, semid;
  if((semid = semget(semkey, 1, SEMPERM|IPC_CREAT|IPC_EXCL)) == -1){
    if(errno == EEXIST)
    semid = semget(semkey, 1, 0);
  }else status = semctl(semid, 0, SETVAL, 1); // si se crea

  if( semid == -1 || status == -1){
    perror("initsem failed");
    return (-1);
  }
  else
  return semid;
}

/*pc â€“ operaciÃ³n de semÃ¡foro p (down)*/
int p(int semid){
  struct sembuf p_buf;
  p_buf.sem_num = 0;
  p_buf.sem_op = -1;
  p_buf.sem_flg = SEM_UNDO;
  if(semop(semid, &p_buf, 1) == -1){
    perror("p(semid) failed");
    exit(1);
  }else
  return(0);
}

/*v.c â€“ operaciÃ³n de semÃ¡foro v (up)*/
int v(int semid){
  struct sembuf v_buf;
  v_buf.sem_num = 0;
  v_buf.sem_op = 1;
  v_buf.sem_flg = SEM_UNDO;
  if(semop(semid, &v_buf, 1) == -1){
    perror("v(semid) failed");
    exit(1);
  }else
  return(0);
}


//QUANTUM asiganado de acuerdo a  prioridad considerando: alta = 5, baja = 1; Cambio de contexto cte. de 2s
void despachador_por_prioridades(key_t key){
	//printf("entro \n");
  struct nodo *desp = getNode(key);
  if( desp->tr != 0 && desp->prioridad < 6){	
	        int quan = 0;  // quan = Quantum , TEsp = contador que llevara el tiempo que tarda en ejecutarse el algoritmo
		     desp->te = TEsp;
		     //la asignaciÃ³n del quantum de acuerdo a la prioridad
         	 if(desp->prioridad == 1){ quan = 8; }
         	 if(desp->prioridad  == 2){ quan = 9; }
         	 if(desp->prioridad  == 3){ quan = 10; }
         	 if(desp->prioridad  == 4){ quan = 11; }
         	 if(desp->prioridad  == 5){ quan = 12; }
         	 //system("clear");
 	 		 if( desp->tr < quan){  // en caso de que quantum sea mayor al tiempo que necesita el proceso para ejecutarse
				 // Aqui la prioridad ya no aumenta porque el proceso ya va a terminar
				    desp->tr = 0; 
		            printf("\n---------------------------------------------------------------\n Proceso: %s", desp->cadena);
				    printf("\tPrioridad  %d ", desp->prioridad);
					printf("\tTiempo que necesita: %d ", desp->dato );
				    printf("\tTiempo restante %d ", desp->tr );
				    printf("\tTiempo de espera %d ", desp->te);
				    printf("\n----------------------------------------------------------------\n");
				    TEsp = TEsp + desp->tr; // a la variable que lleva el control del tiempo que lleva el despachador
				  sleep( desp->tr); //Ocupa solo el tiempo que le falta para terminar, ya no lo que se le asigno de quantum completos
			  }else{
                TEsp = TEsp + quan;
                if(desp->prioridad < 5){desp->prioridad = desp->prioridad + 1;}
			    desp->tr = desp->tr - quan; // Se le resta el tiempo que ya ejecutÃ³ para obtener el tiempo restante
	            printf("\n----------------------------------------------------------------------\n Proceso: %s", desp->cadena);
			    printf("\tPrioridad  %d ", desp->prioridad);
				printf("\tTiempo que necesita: %d ",desp->dato );
			    printf("\tTiempo restante %d ", desp->tr );
			    printf("\tTiempo de espera %d ", desp->te);
			    printf("\n-----------------------------------------------------------------------\n");
				sleep(quan); //Se ejecuta
              }
              fflush(stdout); // se escriben resultados de recorrido en un archivo de salida
		      fprintf(flujo, "Proceso: %s \tPrioridad: %d \tTiempo que necesita: %d \tTiempo restante: %d \tTiempo de espera: %d\n",desp->cadena,desp->prioridad,desp->dato,desp->tr,desp->te);
			  fflush(stdout);
			  if(desp->tr == 0)	{ // si ya no necesitan mas tiempo para ejecutarse, estos ya no se toman en cuenta
			  	  printf("\n Proceso terminado:  %s", desp->cadena);
			  	  pFinish += 1;
			  	  BAN = 1;
			  }	  
			  printf("\n-----------------------------\n");
			  printf("Cambio de Contexto...");
			  printf("\n-----------------------------\n");
          	  sleep(2); // cambio de contexto = 2 segundos
		      TEsp =TEsp + 2; // Tiempo de espera que es un contador se le suma esos segundos perdidod por el cambio de contexto
	}
}
