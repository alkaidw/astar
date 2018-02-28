#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>

//#include<files.h>

// Converts degrees to radians.
#define toRadians(degrees) (degrees * M_PI / 180.0)

// Converts radians to degrees.
#define toDegrees(radians) (radians * 180.0 / M_PI)

// define a string de via de mão única
#define oneway "oneway"

//boolean
#define TRUE 1
#define FALSE 0

typedef unsigned long unsiglong;
typedef unsiglong way;      //estrutura temporaria dos caminhos



int ExitError(char *str, int n);


#include"node.c"


unsiglong count_(char *str, FILE *fin);

Node *read_map(char *filename, Node *nodes);
int charcount(char *str, char c);
long double bisearch(Node *nodes, unsiglong id, unsiglong n);
//unsiglong *insert(unsiglong *vet, int count, unsiglong a);


int compare_ids(const void *arg1, const void *arg2);
void pause(void);

void astar(Node *nodes, unsiglong nnodes, unsiglong start, unsiglong goal);


#include"queue2.c"



int main(int argc, char *argv[]){

	FILE *fin;

	Node *nodes;
	way *ways;

	if ((fin = fopen (argv[1], "r")) == NULL) 
		ExitError("the data file does not exist orcannot be opened", 11); 

	//contadores
	unsiglong nnodes = count_("nodes", fin);
	unsiglong nways = count_("ways", fin);;

	//nós de origem e destino
	//unsiglong node_start = 240949599;
	//unsiglong node_goal =  195977239;


//	cataluna
	unsiglong node_start = 8670491;
	unsiglong node_goal = 8670523;


//	teste 1
//	unsiglong node_start = 1;
//	unsiglong node_goal = 6;



	size_t buffer_size = 79858;
	char *buffer = malloc(buffer_size * sizeof(char));
	char *p1 = 0, *p2;

	unsiglong i;
	unsiglong id;
	
	// aloca memoria para os nós
	if((nodes = (Node*)malloc(nnodes*sizeof(Node))) == NULL)
		ExitError("when allocatingmemory for the nodes vector",1);
		

	// percorre o arquivo alimentando o vetor de nós
	i = 0;
	while(getline(&buffer, &buffer_size, fin) != EOF){

		//se nao for nó, pula
		if(buffer[0] != 'n')
			continue;

		//aponta para o id da linha corrente, a sexta posição
		p1 = buffer+5;

		//conversão da string para float
		id = atof(p1);

		nodes[i].id = id;
	
		/*
		// resgata o campo nome, rotina problematica
		if(*(p2+1) != '|'){
			p1 = p2;
			p2 = strchr(p1, '|');
			if(p2 != NULL)
				*p2 = '\0';
			strcpy(nodes[i].name, p1);
		}
		*/

		// percorre o ponteiro p2 até chegar nos campos de lat e long
		int count=0;
		while(count<8){
			if(*p1 == '|')
				++count;
			++p1;
		}

		// resgata os valores de latitude e longitude do buffer e armazena no nó
		nodes[i].lat = atof(p1);
		p1 = strchr(p1, '|')+1;
		nodes[i].lon = atof(p1);
		nodes[i].nsucc = 0;
		++i;		
	}


//	printf("%lu", i);
//	printf("%lu", nnodes);

	// "rebobina" o arquivo
	rewind(fin);
	
	int n;
	unsiglong *succtmp;
	char isOneWay;

	if((ways = (way*)malloc(nways*sizeof(way))) == NULL){
		printf("when allocatingmemory for the nodes vector");
		exit(1);
	}



	while(getline(&buffer, &buffer_size, fin) != EOF){

		if(buffer[0] != 'w')
			continue;

		isOneWay = (strstr(buffer, oneway) != NULL);
			
		
				
		//move o cursor p1 até o inicio da lista de nós, são 9 campos
		int count=0;
		p1=buffer;
		while(count<9){
			if(*p1 == '|')
				++count;
			++p1;
		}

		//conta os nós armazenados no caminho
		n = charcount(p1, '|')+1;




		//reserva memoria pra lista de nós do caminho
		if((succtmp = (unsiglong *) malloc(n*sizeof(unsiglong))) == NULL)
			ExitError("when allocatingmemory for the ways vector",1);

		//conversão da string de id nós para longs armazenados no vetor temporario
		for(i=0; i<n; ++i){
			succtmp[i] = atof(p1);
			p1 = strchr(p1, '|')+1;
		}


		unsiglong idx1, idx2;
		unsigned short nsucc;


		//o trecho que segue verifica a existencia do primeiro nó do caminho		
		idx1 = bisearch(nodes, succtmp[0], nnodes);
		
		//printf("1o. no %lu\nidx1: %lu\n",succtmp[0],idx1);

		if(idx1 != -1){		
			//percorre o caminho do elemento segundo até o último
			for(i=1; i<n; ++i){
			
				//procurando por cada nó
				idx2 = bisearch(nodes, succtmp[i], nnodes);
				//que caso seja encontrado será armazenado na lista de adjacencias

				if(idx2 != -1){

					nsucc = nodes[idx1].nsucc;

					//realocacao de memoria
					nodes[idx1].successors = realloc(nodes[idx1].successors, (nsucc+1)*sizeof(unsiglong));
					//deve-se rever aqui a questão da repetição 
					//dos nós na linha de adjacencias					
					
					//nodes[idx1].successors[nsucc] = succtmp[i];
					if(insert(nodes[idx1].successors, nsucc, succtmp[i]) != NULL)
						nodes[idx1].nsucc++; //atualizacao do grau do nó




					//EH NECESSARIO TESTAR ESSE PONTEIRO
					if(!isOneWay){
						//printf("mao dupla\n");
						nodes[idx2].successors = realloc(nodes[idx2].successors, 
										(nodes[idx2].nsucc+1)*
											sizeof(unsiglong));

						if(insert(nodes[idx2].successors, nodes[idx2].nsucc, succtmp[i-1]) != NULL)
							nodes[idx2].nsucc++; //atualizacao do grau do nó
					}
					//passo			
					idx1 = idx2;

				}
				else
					break;
				//caso contrário \relax

			}
		}
		else
			//libera memoria
			free(succtmp);
	}


/*
	int j;	
	// impressão dos dados lidos
	for(i=0;i<nnodes;++i){
		
		printf("node.id: %lu\n", (nodes[i].id));		
		printf(" lat: %lf\n", (nodes[i].lat));		
		printf(" lon: %lf\n", (nodes[i].lon));
		printf("   sucessores: ");
		for(j =0; j<nodes[i].nsucc; ++j)
			printf("%lu ", nodes[i].successors[j]);
		puts("\n");
	}
	

	printf("nos %lu\n", nnodes);
	printf("caminhos %lu\n", nways);
	
	//printf("%f", distance_Nodes(nodes, nodes+1));
*/




	free(buffer);
	free(ways);



	//printf("\n%f\n", haversine(23,15,42,8));
//		open

	
	puts("Astar");
//	unsiglong a = 8670498;

	Node* no;
	(bsearch(&node_start, nodes, nnodes, sizeof(Node), compare_ids) != NULL)? puts("no inicial ok"):puts("no inicial não encontrado");
	(bsearch(&node_goal, nodes, nnodes, sizeof(Node), compare_ids) != NULL)? puts("no inicial ok"):puts("no destino não encontrado");

//	free(nodes);
	//pause();


	astar(nodes, nnodes, node_start, node_goal);
	return 0;
}





void astar(Node *nodes, unsiglong nnodes, unsiglong start, unsiglong goal){

	Queue *open, *close;
	open = new_Queue(nnodes);
	close = new_Queue(nnodes);



	//pause();

	Node* idx_start = (Node*) bsearch(&start, nodes, nnodes, sizeof(Node), compare_ids);


//	printf("%Lf", bisearch(nodes, start, nnodes));

//	pause();

	Node* idx_goal = (Node*) bsearch(&goal, nodes, nnodes, sizeof(Node), compare_ids);

//	pause();

	if(idx_start == NULL || idx_goal == NULL)
		return;
	
	

	unsiglong idx_tmp;
	unsiglong i;
	Node *cnode, *pnode, *node_tmp, *st_node, *gl_node;	

	

	gl_node = idx_goal;
	st_node = idx_start;

	Qel *qel, *qel_tmp;

	qel = new_Qel(st_node, NULL);
	qel->h = distance_Nodes(st_node, gl_node);	
	qel->g = 0;
	qel->f = qel->h + qel->g;
	qel->node_par = NULL;
	qel->par = NULL;

	q_store(open, qel);
	qel->wq=1;
	unsiglong count = 0;


/*
	printf("nos %lu\n", nnodes);
	printf("qelH %Lf\n", qel->h);
	printf("latlon %Lf, %Lf\n", st_node->lat, st_node->lon);
	printf("latlon %Lf, %Lf\n", gl_node->lat, gl_node->lon);

	printf("latlon %Lf\n", haversine(gl_node->lat, gl_node->lon, st_node->lat, st_node->lon));




	printf("latlon real %Lf\n", haversine(41.231982, 1.690731, 41.232461, 1.694275));

	printf("latlon %Lf\n", haversine(42.134561, 1.134581, 41.187341, 1.145321));
*/


	pause();

	while(!is_empty_Queue(open)){
	
		//printf("%lu\n", count++);

		qel = q_retrieve(open); //não é necessário o teste do retrieve
		q_store(close, qel);
		qel->wq=2;


//		q_sort(close);		
//		q_sort(open);

		cnode = qel->node;		


//		print_Node(cnode);
//		q_resume(open, "aberta");
//		q_resume(close, "fechada");

		if(cnode->id == gl_node->id){
			printf("\ncnode.id == glnode.id\ncaminho encontrado\n");

			//pause();

			q_backtrack(close, qel);
			break;
			
		}




		for(i=0; i < cnode->nsucc; ++i){


			// gargalo
			unsiglong succ_id = qel->node->successors[i];
			if(/*q_inqueue(open, succ_id) || */ q_inqueue(close, succ_id))
				continue;


			
			idx_tmp = bisearch(nodes, qel->node->successors[i], nnodes);
			node_tmp = nodes+idx_tmp;
			




			qel_tmp = new_Qel(node_tmp, cnode);

			qel_tmp->h = distance_Nodes(node_tmp, gl_node);
			qel_tmp->g = qel->g + distance_Nodes(node_tmp, st_node);
			qel_tmp->f = qel_tmp->g + qel_tmp->h;

			qel_tmp->par = (Qel*)qel;
			qel_tmp->node_par = cnode;



			long double tentativeG = qel->g + distance_Nodes(qel->node, node_tmp);

/*

			printf("%Lf, %Lf\n", qel->g, qel_tmp->g);
			printf("distancia %Lf\n", distance_Nodes(qel->node, node_tmp));

			if(tentativeG >= qel_tmp->g)
				continue;
*/


			
			q_store(open, qel_tmp);
			qel_tmp->wq= 1;
		}
		
//		q_sort(close);		
		q_sort(open);

	}

}






//busca na ordem de log(n)
long double bisearch(Node *nodes, unsiglong id, unsiglong n){
	
	unsiglong flr = 0;
	unsiglong top = n-1;
	unsiglong i;
	
//	pause();

	while(flr<=top){

		i = (flr+top)/2;

		if (id == nodes[i].id)
			return i;

		if (id < nodes[i].id)
			top = i-1;

		else
			flr = i+1;

	}	
	return -1;   // não encontrado
}

int compare_ids(const void *arg1, const void *arg2){
	return ((Node*)arg1)->id - ((Node*)arg2)->id;
}





int ExitError(char *str, int n){
	printf("%s",str);
	exit(n);
}

//contador de caracteres
int charcount(char *str, char c){
	int count = 0;
	while(*str != 0){
		if(*str == c)
			++count;
		++str;
	}
	return count;
}

//conta linhas com determinado caracter inicial, 
//usado para contar nós 'n' e caminhos 'w'
unsiglong count_(char *str, FILE *fin){

	unsiglong n = 0;
	size_t buffer_size = 79858;
	char *buffer = malloc(buffer_size * sizeof(char));
	char c = str[0];

	while(getline(&buffer, &buffer_size, fin) != EOF)
		if(buffer[0] == c)
			++n;
	rewind(fin);
	return n;	
}



void pause(void){
	puts("pausa");
	putchar('\a');
	getchar();
}
