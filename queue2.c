#include<stdlib.h>

typedef unsigned long unsiglong;


//estrutura das filas
typedef struct {
	//h = custo até o destino, g = custo até o anterior, f = soma dos custos
	double g, h, f;
	Node *node, *node_par;
	void *par;
} Qel;


Qel* new_Qel(Node *cnode, Node *pnode){



	Qel* qel = (Qel*) malloc(sizeof(Qel));


	if(qel == NULL)
		ExitError("when allocating memory for Qel",1);


	qel->node = cnode;
	if(pnode != NULL)
		qel->node_par = pnode;
	
	return qel;
}

void destroy_Qel(Qel *qel){
	free(qel);
	return;
}

int comp_node_dist(const void* a, const void* b){
	return ((Qel*)a)->f - ((Qel*)b)->f;
}


/******************************************************************************/
typedef struct {
	//Node *node_goal;
	Qel **elems;
	unsiglong rpos, spos, size;
} Queue;


Queue* set_goal(Queue* q, Node* goal_node);

Queue* new_Queue(unsiglong size);
void destroy_Queue(Queue *q);
Qel* q_store(Queue *q, Qel* el);
Qel* q_retrieve(Queue *q);
int is_empty_Queue(Queue *q);
int comp_node_lat(const void* a, const void* b);
Queue* sort(Queue *q);
unsiglong q_length(Queue* q);



Queue* new_Queue(unsiglong size){
	Queue* q = (Queue*) malloc(sizeof(Queue));

	if(q == NULL)
		ExitError("when allocatingmemory for the Queue", 1);
	
	q->elems = (Qel**) malloc(size*sizeof(Qel**));

	if(q->elems == NULL)
		ExitError("when allocatingmemory for the Queue", 1);

	q->rpos = 0;
	q->spos = 0;
	q->size = size;

	return q;
}


/*Queue* set_goal(Queue* q, Node* g_goal){
	q->node_goal = g_goal;
	return q;
}


void destroy_Queue(Queue *q){
	free(q->elems);
	free(q);
}*/

int is_empty_Queue(Queue *q){
	return q->rpos == q->spos;
}

void q_print(Queue* q){
	unsiglong i;
	printf("fila, capacidade: %lu\n", q->size);
	printf("quantidade: %lu\n", q->spos - q->rpos);
	for(i=q->rpos; i<q->spos; ++i){

		printf("\nid %lu\n", q->elems[i]->node->id);
	}
	return;
}

void q_resume(Queue* q, char str[]){
	unsiglong i;
	printf("  fila %s: ", str);
	for(i=q->rpos; i<q->spos; ++i)
		printf(" %lu", q->elems[i]->node->id);
	putchar('\n');
}


Qel* q_store(Queue* q, Qel* qel){

	if(q->spos < q->size){
		q->elems[q->spos] = qel;
		q->spos++;

		return qel;
	}
	puts("fila cheia");
	return NULL;
}

Qel* q_retrieve(Queue *q){
	if(q->rpos < q->spos)
		return q->elems[q->rpos++];
	puts("fila vazia");
	return NULL;
}

Queue* q_sort(Queue *q){

	qsort(q->elems, q->size, sizeof(Qel*), comp_node_dist);

}

int q_inqueue(Queue* q, unsiglong node_id){
	long i;
	for(i=q->rpos; i<q->spos; ++i)
		if(q->elems[i]->node->id == node_id)
			return TRUE;
	return FALSE;
}

void q_backtrack(Queue* q, Qel* qel){
	unsiglong i;
	Qel* qel_par = qel->par;

	printf("caminho de volta %lu ", qel->node->id);
	while(qel_par != NULL){
		printf("%lu ",qel_par->node->id);
		qel_par = qel_par->par;
	}
	
	return;
}

unsiglong q_length(Queue* q){
	return q->spos - q->rpos;
}

int comp_node_lat(const void* a, const void* b){
	return ((Qel*)a)->node->lat - ((Qel*)b)->node->lat;
}
