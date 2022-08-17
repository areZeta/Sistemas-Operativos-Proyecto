
/*EL PROGRAMA 1 solamente se encarga de leer procesos con su respectiva informaci�n y los va 
almacenando en una estructura(lista), desde donde dewspues los toma y los lanza al programa 2 mediante ujn socket */



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 4444

int hhp = 0;
int nodos = 0;
char *ip;
int op = 0;
/*ESTRUCTURA DE DONDE SE GUARDARAN NODOS DE LA LISTA que se obtine al introducirse procesos*/
typedef struct nodo{
	int prioridad;
	int dato;
	int tr; //tiempo restante
	int TEsp;
	char cadena[20];
	struct nodo* siguiente;
}nodo;

typedef struct lista{
	int datox;
	struct lista* siguiente;
}lista;

//APUNTADORES GLOBALES A LA LISTA DONDE SE ENCUENTRAN LOS PROCESOS 
nodo* primero = NULL;
nodo* ultimo = NULL;
lista* first = NULL;
lista* last = NULL;

//PROTOTIPO DE FUNCIONES
void insertarNodo(int, int , char const *cadena);
void insertarNodoN(int, int , char const *cadena);
void desplegarLista();
void submenu();
void datosNodo();
void eliminarNodoF();
void eliminarNodoI();
void menu();
void datosArchivo();
void limpiarListaCircular();
void limpiarListaCircularx();
void cargaProcesos();
void insertarNodoDatox(int);
int verificarDatox(int);
int contarProcesos();
void desplegarListax();
void desplegarListaServidor(int,int,int);

int main(int argc, char *argv[]){
  if(argc > 1){
    ip=argv[1]; // LEE LA IP AGREGADA DESDE TERMINAL AL COMPILAR
  	menu();
  }else{
    printf("[-]Es necesario colocar el ip del servidor...\n");
  }
	return 0;
}

void menu(){
	int key = 1;
	system("clear");
	printf("\n*****************************************************\n");
	printf("\n\t\tBienvenido");
	while(key != 0){
		printf("\n\tPrograma Lista circular simple\n");
		printf("\n1.Cargar procesos desde un archivo");
		printf("\n2.Ingresar datos desde teclado");
		printf("\n3.Salir");
		printf("\nIngrese una opcion: [ ]\b\b");
		scanf("%d",&op);
	printf("\n*****************************************************\n");
		if((op < 1) || (op > 3)){
			system("clear");
			printf("\n\nOpcion no valida, intente de nuevo");
		}else{
			switch(op){
				case 1:
					datosArchivo();
					submenu();
					break;
				case 2:
					limpiarListaCircular();
					submenu();
					break;
				case 3:
					system("clear");
					key = 0;
					break;
			}
		}
	}
}

void submenu(){
	int opc = 0;
	int key = 1;
	system("clear");
	while(key != 0){
		printf("\n1.Insertar un nodo.");
		printf("\n2.Eliminar nodo del inicio.");
		printf("\n3.Eliminar nodo del final.");
		printf("\n4.Mostrar lista ligada circular.");
		printf("\n5.Iniciar carga de procesos");
		printf("\n6.Regresar al menu.");
		printf("\nIngrese una opcion: [ ]\b\b");
		scanf("%d",&opc);
		if((opc < 1) || (opc > 6)){
			system("clear");
			printf("\n\nOpcion no valida, intente de nuevo\n");
		}else{
			switch(opc){
				case 1:
					datosNodo();
					break;
				case 2:
					eliminarNodoI();
					break;
				case 3:
					eliminarNodoF();
					break;
				case 4:
					desplegarLista();
					break;
				case 5:
					cargaProcesos();
					break;
				case 6:
          system("clear");
          key = 0;
          break;
			}
		}
	}
}
//

/*EN CASO DE QUE USUARIO DIGITE QUE QUIERE INGRESAR PROCESOS DESDE TECLADO, SE LE PIDE QUE A�ADA CIERTA INFORMACION DEL PROCESO*/
void datosNodo(){
	int dato;
	int prioridad;
	char cadena[20];
	system("clear");
	printf("----------------------------------- \n");
	printf("Ingrese nombre de proceso: ");
	scanf(" %[^\n]s",cadena);
	printf("Ingrese tiempo que necesita para ejecutarse: ");
	scanf("%d",&dato);
	printf("Ingrese la prioridad del proceso: ");
	scanf("%d",&prioridad);
	printf("----------------------------------- \n");
	insertarNodo(prioridad,dato,cadena);
}


/*SE MANDA A LLAMAR CADA VEZ QUE SE QUIERA GREGAR UN NODO A LA LISTA DE PROCESOS, LIGA NODOS Y FORMA LA LISTA*/
void insertarNodo(int prioridad,int dato,char const *cadena){
	nodos++;
	nodo* nuevo = (nodo*) malloc(sizeof(nodo));
	nuevo->dato = dato;
	nuevo->prioridad = prioridad; 
	nuevo->tr = dato;
	strcpy(nuevo->cadena,cadena);
	system("clear");
	if(primero==NULL){
		primero = nuevo;
		primero->siguiente = primero;
		ultimo = primero;
	}else{
		ultimo->siguiente = nuevo;
		nuevo->siguiente = primero;
		ultimo = nuevo;
	}
	printf("\nNodo agregado exitosamente\n");
}

void insertarNodoN(int prioridad,int dato,char const *cadena){
	nodo* nuevo = (nodo*) malloc(sizeof(nodo));
	nuevo->dato = dato;
	nuevo->tr = dato;
	nuevo->prioridad = prioridad;
	strcpy(nuevo->cadena,cadena);
	if(primero==NULL){
		primero = nuevo;
		primero->siguiente = primero;
		ultimo = primero;
	}else{
		ultimo->siguiente = nuevo;
		nuevo->siguiente = primero;
		ultimo = nuevo;
	}
}

void insertarNodoDatox(int datox){
	lista* nuevo = (lista*) malloc(sizeof(lista));
	nuevo->datox = datox;
	//system("clear");
	if(first==NULL){
		first = nuevo;
		first->siguiente = first;
		last = first;
	}else{
		last->siguiente = nuevo;
		nuevo->siguiente = first;
		last = nuevo;
	}
}

int verificarDatox(int datox){
	lista* actual = (lista*)malloc(sizeof(lista));
	actual = first;
	int auxiliar = 0;
	//system("clear");
	if(first != NULL){//Comprobando la existencia de la lista
		do{
			if (actual->datox == datox){
				auxiliar += 1;
			}
			actual = actual->siguiente;
		}while(actual != first);
		if (auxiliar >= 1) {
			return 0;
		}else {
			return 1;
		}
	}else{
		return 1;
	}
}


void desplegarListax(){
	lista* actual = (lista*)malloc(sizeof(lista));
	actual = first;
	if(first != NULL){//Comprobando la existencia de la lista
		do{
			printf("\n %d ",actual->datox);
			actual = actual->siguiente;
		}while(actual!=first);
		printf("\n");
	}else{
		printf("\nLa lista se encuentra vacia\n");
	}
}


/*MUESTRA LA LISTA DE PROCESOS QUE ESTAAN CARGADOS EN LA LISTA*/
void desplegarLista(){
	nodo* actual = (nodo*)malloc(sizeof(nodo));
	actual = primero;
	system("clear");
	if(primero != NULL){//Comprobando la existencia de la lista
		do{
			printf("\n %s %d %d",actual->cadena,actual->dato,actual->prioridad);
			actual = actual->siguiente;
		}while(actual!=primero);
		printf("\n");
	}else{
		printf("\nLa lista se encuentra vacia\n");
	}
}


//ELIMINA UN NODO DE LA LISTA  DE PROCESOS
void eliminarNodoF(){
	nodo* borrado = (nodo*)malloc(sizeof(nodo));
	nodo* penultimo = (nodo*)malloc(sizeof(nodo));
	borrado = NULL;
	penultimo = primero;
	system("clear");
	if(primero != NULL){
		if(primero == ultimo){
			borrado = ultimo;
			primero = NULL;
			ultimo = NULL;
		}else if(primero->siguiente == ultimo){
			borrado = ultimo;
			primero->siguiente = primero;
			ultimo = primero;
		}else{
			do{
				penultimo = penultimo->siguiente;
			}while(penultimo->siguiente != ultimo);
			borrado = ultimo;
			penultimo->siguiente = primero;
			ultimo = penultimo;
		}
		system("clear");
		printf("\nNodo eliminado exitosamente\n");
		free(borrado);
	}else{
		printf("\nLa lista se encuentra vacia\n");
	}
}

void eliminarNodoI(){
	nodo* borrado = (nodo*)malloc(sizeof(nodo));
	nodo* segundo = (nodo*)malloc(sizeof(nodo));
	segundo = NULL;
	borrado = NULL;
	system("clear");
	if(primero != NULL){
		if(primero == ultimo){
			borrado = primero;
			primero = NULL;
			ultimo = NULL;
		}else if(primero->siguiente == ultimo){
			borrado = primero;
			primero = ultimo;
			primero->siguiente = primero;
		}else{
			borrado = primero;
			primero = primero->siguiente;
			ultimo->siguiente = primero;
		}
		system("clear");
		printf("\nNodo eliminado exitosamente\n");
		free(borrado);
	}else{
		printf("\nLa lista se encuentra vacia\n");
	}
}

void limpiarListaCircular(){
	nodo* actual = (nodo*)malloc(sizeof(nodo));
	nodo* auxiliar = (nodo*)malloc(sizeof(nodo));
	system("clear");
	if(primero != NULL){
		actual = primero->siguiente;
		do{
			auxiliar = actual;
			actual = actual->siguiente;
			free(auxiliar);
		}while(primero!=auxiliar);
		primero = NULL;
	}
}

void limpiarListaCircularx(){
	lista* actual = (lista*)malloc(sizeof(lista));
	lista* auxiliar = (lista*)malloc(sizeof(lista));
	system("clear");
	if(first != NULL){
		actual = first->siguiente;
		do{
			auxiliar = actual;
			actual = actual->siguiente;
			free(auxiliar);
		}while(first!=auxiliar);
		first = NULL;
	}
}

/*ESTA FUNCION LEE PROCESOS DESDE UJH ARCHIVO Y LOS VA INGRESANDO A UNA LISTA DE PROCESOS PARA DESPUES LANZARLOS*/
void datosArchivo(){
	int contador = 0;
	int i = 0;
	char caracter;
	int dato;
	int prioridad;
	int bandera;
	char cadena[20];
	char cadena1[20];
	char cadena2[20];
	FILE * flujo = fopen("practica2_team7_datos.txt", "rb"); // APERTURA DE ARCHIVOI DESDE DONDE SE VAN A LEER LOS PROCESOS
	if(flujo == NULL){
		printf("\nError en la apertura del archivo\n");
	}else{
    printf("\n\n");
  	while(feof(flujo) == 0){ // SE ENTRA A UN CICLO A LEER LINEA POR LINEA
  		caracter = fgetc(flujo);
  		if(contador == 0){
  			if(caracter != ' '){
  				cadena[i] = caracter;
  				i++;
  			}else{
  				cadena[i] = '\0';
  				contador = 1;
  				i = 0;
  			}
  		}else if(contador == 1){
  			if(caracter != ' '){
  				cadena1[i] = caracter;
  				i++;
  			}else{
  				contador = 2;
  				i = 0;
  			}
  		}else if((contador == 2) && (caracter != ' ')){
  			if(caracter != '\n'){
  				cadena2[i] = caracter;
  				i++;
  			}else{
  				contador = 0;
  				i = 0;
  				bandera = 1;
  			}
  		}
  		if(bandera == 1){ 
  			dato = atoi(cadena1);//Funcion para pasar cadena->entero
  			prioridad = atoi(cadena2);
  			insertarNodo(prioridad,dato,cadena); // INSERTA UN NODO DETRAS DE OTRO Y CREA UNA LISTA
  			memset(cadena,0,20);//limpiar cadenas
  			memset(cadena1,0,20);
  			memset(cadena2,0,20);
  			bandera = 0;
  		}
  	}
  	fclose(flujo);
  	printf("\n\nSe ha leido el archivo correctamente");
  }
}

int contarProcesos(){
	int contador = 0;
	int i = 0;
	char caracter;
	int bandera;
	char cadena[20];
	char cadena1[20];
	char cadena2[20];
	int nProcesos = 0;
	FILE * flujo = fopen("practica2_team7_datos.txt", "rb");
	if(flujo == NULL){
		printf("\nError en la apertura del archivo\n");
	}
	printf("\n\n");
	while(feof(flujo) == 0){ //ENTRA EN CICLO DONDE LEE LINEA POR LINEA Y SEPARA ARGUMENTOS
		caracter = fgetc(flujo);
		if(contador == 0){
			if(caracter != ' '){
				cadena[i] = caracter;
				i++;
			}else{
				cadena[i] = '\0';
				contador = 1;
				i = 0;
			}
		}else if(contador == 1){
			if(caracter != ' '){
				cadena1[i] = caracter;
				i++;
			}else{
				contador = 2;
				i = 0;
			}
		}else if((contador == 2) && (caracter != ' ')){
			if(caracter != '\n'){
				cadena2[i] = caracter;
				i++;
			}else{
				contador = 0;
				i = 0;
				bandera = 1;
			}
		}
		if(bandera == 1){
			nProcesos += 1;
			memset(cadena,0,20);//limpiar cadenas
			memset(cadena1,0,20);
			memset(cadena2,0,20);
			bandera = 0;
		}
	}
	fclose(flujo);
	return nProcesos;
}


//ESTA FUNCION SE ENCARGA DE MANDA MEDIANTE UN SOCKET, PROCESOS SIMULTANEAMENTE AL PROGRAMA 2
void desplegarListaServidor(int auxt,int n,int nProcesos ){
  int clientSocket, ret;
	char dato[20];
	char prioridad[20];
	struct sockaddr_in serverAddr; // PARA HACER REFERENCIA AL SOCKET
	char buffer[1024];
	char finish[] = "finish"; 
  nodo* actual = (nodo*)malloc(sizeof(nodo));
	actual = primero;
  int auxCont = 0;
	if(primero != NULL){//Comprobando la existencia de la lista
		struct hostent *he;
		if((he = gethostbyname(ip)) == NULL){
			printf("gethostbyname error\n");
			exit(-1);
		}
		//definicion del socket
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(clientSocket < 0){
			printf("[-]Error en la conexion.\n");
			exit(1);
		}
		printf("[+]Socket cliente ha sido creado.\n");
		memset(&serverAddr, '\0', sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(PORT);
		serverAddr.sin_addr = *((struct in_addr *)he->h_addr);//inet_addr("127.0.0.1");
		//he->h_addr pasa la informacion de he a h_addr
		//conexion al servidor
		ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
		if(ret < 0){
			printf("[-]Error en la conexion.\n");
			exit(1);
		}
		printf("[+]Conectado al servidor.\n");
		do{
      if((auxCont >= auxt) && (auxCont < n)){
				memset(buffer,'\0',sizeof(buffer));
				memset(dato,'\0',sizeof(dato));
				memset(prioridad,'\0',sizeof(prioridad));
				sprintf(dato, "%d", actual->dato);
				sprintf(prioridad, "%d", actual->prioridad);
				strcpy (buffer, actual->cadena);
				size_t length = strlen(buffer);
				buffer[length] = ' ';
				strcat(buffer, dato);
				length = strlen(buffer);
				buffer[length] = ' ';
				length = strlen(prioridad);
				prioridad[length] = '\0';
				//prioridad[length+1] = '\0';
     		strcat (buffer, prioridad);
				//printf("\n %s ",buffer);
				send(clientSocket, buffer, strlen(buffer), 0);
				sleep(1);
				if(nProcesos == (hhp)){
					send(clientSocket, finish, strlen(finish), 0);
					sleep(1);
				}
				hhp ++;
      }
      actual = actual->siguiente;
      auxCont += 1;
		}while(actual!=primero);
		close(clientSocket);
		printf("[-]Desconectado del servidor.\n");
	}else{
		printf("\nLa lista se encuentra vacia\n");
	}
}

void cargaProcesos(){
	nodo* actual = (nodo*)malloc(sizeof(nodo));
	limpiarListaCircularx(); //limpiar lista auxiliar
	srand (time(NULL));
	actual = primero;
  int auxt;//Auxiliar en el envio de datos al servidor
  int auxn;//auxiliar en el tamaño del lote
	int procesosArchivo = 0;
	int nProcesos = 0;
	int aux2 = 1;
	int n = 0; //tamaño del lote
	int contador1 = 0;
	int hp = 0;
	int t = 0;//Tiempo en que se carga el lote
	system("clear");
	hhp = 1;
	do {
		nProcesos = rand() % 501;
	} while((nProcesos < 2) || (nProcesos > nodos));
  auxt=0;
  auxn=0;
	do {
		n = rand() % 51;//Tamaño del lote
		t = rand() % 18;//tiempo de carga
		if (t == 0){
			t += 1;
		}
		if(n > nProcesos){
			n = nProcesos;
		}
		if(n == 0){
			n++;
		}
		if ((contador1 + n)  > nProcesos) {
			n = nProcesos - contador1;
		}
		contador1 = contador1 + n;
		printf("\nnProcesos:%d  Lote de:%d \n",nProcesos,n);
		//recorridoProcesos(procesosArchivo,nProcesos,n,t);
		printf("Los procesos seran cargados en: %d segundos\n",t);
	    sleep(t);
		if(contador1 >= nProcesos){
			aux2 = 2;
		}
    auxn += n;
    //Lista de procesos a enviar al servidor
		desplegarListaServidor(auxt,auxn,nProcesos);
		hp ++;
    auxt += n;
	} while(aux2 != 2);
}


