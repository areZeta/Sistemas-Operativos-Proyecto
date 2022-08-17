#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

extern int errno;

#define SEMPERM 0600
#define TRUE 1
#define FALSE 0
#define PORT 4444

pid_t childpid;
int newSocket;
//conexion mem compartida
int *keyCont = 0;
int *finish = 0;//encendido apagado
///memoria compartida////////
int shm = 0;
int shm2 = 0;
///semaforos//////////
int semid=0;
int semid1 = 0;
int semid2 = 0;
///////////


void senalTrata();
void limpiarS();

struct nodo{
  int prioridad;
  int dato;
  int tr;
  int te;
  char cadena[20];
  key_t next;
} nodo;

void errorHandler(char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

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

void addValue(key_t headKey, key_t newKey, int prioridad, int dato, char const *cadena){
  struct nodo *head = getNode(headKey);
  while(head->next!=0){
    headKey = head->next;
    head = getNode(headKey);
  }
  struct nodo *node = getNode(newKey);
  node->prioridad = prioridad;
  node->dato = dato;
  node->tr = dato;
  node->te = 0;
  strcpy(node->cadena, cadena);
  node->next = 0;
  head->next = newKey;
}


void printNodes(key_t key) {
  while (key != 0) {
    struct nodo *head = getNode(key);
    printf("%d\n", head->prioridad);
    printf("%d\n", head->dato);
    printf("%d\n", head->tr);
    printf("%d\n", head->te);
    printf("%s\n", head->cadena);
		printf("\n\n");
    key = head->next;

  }
}

int initsem(key_t semkey){
  int status = 0, semid;
  if((semid = semget(semkey, 1, SEMPERM|IPC_CREAT|IPC_EXCL)) == -1){
    if(errno == EEXIST)
    semid = semget(semkey, 1, 0);
  }
  else /*si se crea...*/
  status = semctl(semid, 0, SETVAL, 1); // inicializa semaforo en 0
  if( semid == -1 || status == -1){
    perror("initsem failed");
    return (-1);
  }
  else
  return semid;
}

/*pc Ã¢â‚¬â€œ operaciÃƒÂ³n de semÃƒÂ¡foro p (down), espera*/
int p(int semid){
  struct sembuf p_buf;
  p_buf.sem_num = 0;
  p_buf.sem_op = 1;
  p_buf.sem_flg = SEM_UNDO;
  if(semop(semid, &p_buf, 1) == -1){
    perror("p(semid) failed");
    exit(1);
  }else
  return(0);
}

/*v.c Ã¢â‚¬â€œ operaciÃƒÂ³n de semÃƒÂ¡foro v (up) entra*/
int v(int semid){
  struct sembuf v_buf;
  v_buf.sem_num = 0;
  v_buf.sem_op = -1;
  v_buf.sem_flg = SEM_UNDO;
  if(semop(semid, &v_buf, 1) == -1){
    perror("v(semid) failed");
    exit(1);
  }else
  return(0);
}

int main(){
	char cad[20];
	char cad1[20];
	char cad2[20];
  char fin[] = "finish";
	int dato;
	int prioridad;
  int count;
	int cp;
	//---------
	key_t key = 1;
	key_t newKey = 0;
	//---------
  key_t keyAux = 10000;//shm variable

  if((shm=shmget(keyAux,sizeof(int),0600|IPC_CREAT))==-1){// crear mem. compartida 1
    perror("shmget fallo");
    exit(1);
  }
  if((keyCont=(int *)shmat(shm, 0,0))==(int *)-1) { // atar memoria cmpartida 1
    perror("shmat fallo");
    exit(1);
  }
  (*keyCont) = 0;
  //----------------
  key_t keyAux2 = 11000;//shm variable finalizacion

  if((shm2=shmget(keyAux2,sizeof(int),0600|IPC_CREAT))==-1){ // crear mem. compartida 2
    perror("shmget fallo");
    exit(1);
  }
  if((finish=(int *)shmat(shm2, 0,0))==(int *)-1) {// atar memoria cmpartida 2
    perror("shmat fallo");
    exit(1);
  }
  (*finish) = 0;
  //SEMAFOROS---------
  key_t semkey2 = 7000;//Llave del semaforo finalizacion
 
  if((semid2 = initsem(semkey2)) < 0){
    exit(1);
  }
  //---------
  key_t semkey = 5000;//Llave del semaforo

  int pid;
  if((semid = initsem(semkey)) < 0){
    exit(1);
  }
  //---------
  key_t semkey1 = 6000;//Llave del semaforo auxiliar variable contador

  if((semid1 = initsem(semkey1)) < 0){
    exit(1);
  }
  //---------
	char buffer[1024];

	//----------
    int sockfd, ret;
	struct sockaddr_in serverAddr;//informacion respecto al servidor
	//----------

	struct sockaddr_in newAddr;//informacion respescto al cliente

	//---------
	socklen_t addr_size;
	//Definicion del socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error al crear el socket servidor.\n");
		exit(1);
	}
	printf("[+]El socket servidor ha sido creado.\n");
	//Configuracion del servidor
	memset(&serverAddr, '\0', sizeof(serverAddr));//coloca null al contenido del server
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	//Unir el socket al puerto
	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error en bind.\n");
		exit(1);
	}
	printf("[+]Union bind creada exitosamente %d\n", 4444);
	//colocr el socket en modo escucha
	if(listen(sockfd, 10) == 0){
		printf("[+]Escuchando....\n");
	}else{
		printf("[-]Error en escucha\n");
	}
	//------------
	while(1){
		//Dentro de un bucle seguira aceptando nuevas conexiciones
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Conexion aceptada de: %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		if((childpid = fork()) == 0){
			close(sockfd);


			while(1){
				recv(newSocket, buffer, 1024, 0);
				//incio del programa------
				int count = 0;
				int cp = 0;
				if((buffer[0] || buffer[1]) != '0'){
			   	for (int i=0; i <= strlen(buffer); i++){
				    if (count == 0){
				      if(buffer[i] != ' '){
				        cad[cp] = buffer[i];
				        cp ++;
				      }else{
				        cad[i] = '\0';
				        count = 1;//Siguiente dato
				        cp = 0;
				      }
				   }else if(count == 1){
							if(buffer[i] != ' ' && buffer[i] >= '0' && buffer[i] <= '9'){
								cad1[cp] = buffer[i];
								cp++;
							}else{
								count = 2;
								cp = 0;
							}
				    }else if((count == 2) && (count != ' ')){
							if(count != '\n'){
								cad2[cp] = buffer[i];
								cp++;
							}else{
								count = 0;
								cp = 0;
							}
					}
				  }
					dato = atoi(cad1);//Funcion para pasar cadena->entero
					prioridad = atoi(cad2);
					if(dato>0 && prioridad > 0 && prioridad < 6){
 //Almacenamiento de datos a la memoria compartida
	                       //!!! Necesita semaforos , acceso a memoria compartida !!!
				            //printf("\nProceso %d antes de la regiÃƒÂ³n critica contador:%d\n", pid, (*keyCont));
				        v(semid1);
				            v(semid); // entra
				            printf("---------------------------------------------------------------\n");
				            printf(" PROCESO :  %s  TIEMPO:  %d   PRIORIDAD:  %d\n",cad, dato, prioridad);
						 if((*keyCont) == 0){
						    struct nodo *req = getNode(key);//Retorna el apuntador que asocia el segmento de memoria
						    req->prioridad = prioridad;
						    req->dato = dato;
						    req->tr = dato;
						    strcpy(req->cadena,cad);
						    req->next = 0;
						    (*keyCont) ++;
						  }else{
						    newKey = 1+(*keyCont);
	                        printf("Llave de proceso:  %d \n",newKey);
						    addValue(key, newKey, prioridad, dato, cad);
						    (*keyCont) ++;
						  }
						  p(semid); //  semaforo sale
	                   p(semid1);
//!!! Salida de memoria compartida , cierre de semaforos!!!
						//printf(" Datos:  %s %d %d\n",cad, dato, prioridad);
					}
					memset(cad,0,20);//limpiar cadenas
					memset(cad1,0,20);
					memset(cad2,0,20);
				}

	        if(strcmp(buffer, "finish") == 0){
	          v(semid2);
			   printf("Fin del envio %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
	          (*finish) = 1;//lleno
	          p(semid2);
	          send(newSocket, fin, strlen(fin), 0);
						//break;
			}else{
	          send(newSocket, buffer, strlen(buffer), 0);
	        }
			bzero(buffer, sizeof(buffer));
		}
		fflush(stdout);
	    
	    exit(0);
	    
	  }
	  signal(SIGUSR1, senalTrata); // atrapa señal y la mete a una rutina d tratamiento
	}
	
	return 0;
}


// trata la seÃ±al 10 cuando esta es lanzada desde terminal
void senalTrata(){
	printf("entra a tratar senal  \n");
	close(newSocket);
	limpiarS(); //  elimina los recursos utilizados, tales como memoria y semaforos
	kill(childpid, SIGKILL);
	wait(NULL);  // wait espera a cada proceso hijo que se crea
	exit(0);
}

void limpiarS(){ // borra todos los recursos utilizados 
   //memoria c
   if(shmdt(keyCont) == -1){ // se des- ata memoria de donde fue atada con shmat
    printf(" error des atar\n");
  }
  if(shmdt(finish) == -1){ // se des- ata memoria de donde fue atada con shmat
    printf(" error des atar2 \n");
  }
  if(shmctl(shm,IPC_RMID,(struct shmid_ds *)0) < 0)
    printf(" error eliminar mem shm \n");

  if( shmctl(shm2,IPC_RMID,(struct shmid_ds *)0) < 0)
    printf(" error eliminar mem shm2 \n");
    //borrado de semaforos
  
  if(semctl(semid1, 0, IPC_RMID,0) < 0)
    printf(" error eliminar  sem2 \n");
  if(semctl(semid2, 0, IPC_RMID,0) < 0)
    printf(" error eliminar  sem3 \n");
}
