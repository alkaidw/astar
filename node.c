
typedef unsigned long unsiglong;

typedef struct {
	unsiglong id;	 	// Node identification
	char *name;
	double lat, lon;	// Node position
	short nsucc;	// Node successors: weighted edges
	unsiglong *successors;
	
} Node;

typedef struct {
	Node *v;
	unsiglong len;
} NodeVector;



short add_successor(Node* node, unsiglong id);
short weighted_Node(Node* node);
double distance_Nodes(Node* a, Node* b);

NodeVector* new_NodeVector(unsiglong len);
unsiglong add_Node(NodeVector *nv, Node node);
unsiglong len_NodeVector(NodeVector* nv);
void destroy_NodeVector(NodeVector* nv);
unsiglong *insert(unsiglong *vet, int count, unsiglong a);
double haversine(double lat1, double lon1, double lat2, double lon2);


void print_Node(Node* no);



short add_successor(Node* node, unsiglong id){
	
	if(node->nsucc < 16){
		node->successors[++node->nsucc] = id;
		return(node->nsucc);
	}
	return -1;
}

short weighted_Node(Node* node){
	
	return node->nsucc;

}

double distance_Nodes(Node* a, Node* b){

	return haversine(a->lat, a->lon, b->lat, b->lon);
	
}

NodeVector* new_NodeVector(unsiglong len){

	NodeVector *nv;
	nv = (NodeVector*) malloc(sizeof(NodeVector));

	if(nv == NULL)
		return NULL;

	nv->v = (Node*) malloc(len*sizeof(unsiglong));
	
	if(nv->v == NULL)
		return NULL;

	nv->len = 0;

	return nv;
}


unsiglong add_Node(NodeVector *nv, Node node){
	
	nv->v[nv->len++] = node;
	return nv->len;

}

unsiglong len_NodeVector(NodeVector* nv){
	
	return nv->len;

}

void destroy_NodeVector(NodeVector* nv){

	free(nv->v);
	free(nv);
	return;

}



unsiglong *insert(unsiglong *vet, int count, unsiglong a){

	int i;
	unsiglong *buffer_tmp;

	i=0;
	while(i < count && vet[i] < a)
		++i;

	if(i == count){
		if((vet = realloc(vet, (count+1)*sizeof(unsiglong))) == NULL)
			ExitError("falha na alocacao na função insert", 1);
		vet[i] = a;

		return vet;
	}
			
	else if(vet[i] > a){ 
		if((buffer_tmp = malloc((count-i)*sizeof(unsiglong))) == NULL)
			ExitError("falha na alocacao na função insert", 1);

		memcpy(buffer_tmp, vet+i, (count-i)*sizeof(unsiglong));

		if((vet = realloc(vet, (count+1)*sizeof(unsiglong))) == NULL)
			ExitError("falha na alocacao na função insert", 1);

		vet[i] = a;
		memcpy(vet+i+1, buffer_tmp, (count-i)*sizeof(unsiglong));

		return vet;

	}
	return NULL;

}




double haversine(double lat1, double lon1, double lat2, double lon2){

	double R = 6371e3;
	double phi_1 = toRadians(lat1);
	double phi_2 = toRadians(lat2);
	double delta_phi = toRadians(abs(lat2-lat1));
	double delta_lambda = toRadians(abs(lon2-lon1));
	double a = sin(delta_phi/2.0) * sin(delta_phi/2.0) +
			cos(phi_1) * cos(phi_2) *
			sin(delta_lambda/2.0) * sin(delta_lambda/2.0);
	double c = 2 * atan2(sqrt(a), sqrt((1-a)));
	double d = R * c;

	return d;
}


void print_Node(Node* no){
	puts("--------------------");
	printf("node_id: %lu\n", no->id);
	printf("lat/lon: [%.3f,%.3f]\n", no->lat, no->lon);
	printf("nsucc: %d\n", no->nsucc);
	printf("sucessores: ");
	for(int i=0; i<no->nsucc; ++i)
		printf("%lu ", no->successors[i]);

	putchar('\n');
	puts("--------------------");	
}
