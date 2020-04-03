#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS              0
#define INVALID_TREE        -1
#define INVALID_NODE        -2
#define INVALID_REGISTER    -3
#define NODE_NOT_FOUND      -4
#define REGISTER_NOT_FOUND  -5

//Struct para data (auxilia na comparação)
typedef struct{
    int day, month, year;
} DATE;

//Struct para armazenamento das informações
typedef struct{
    unsigned long int cpf, rg;
    char *name, *city;
    DATE birth;
} REGISTER;

//Struct para referenciar os nós
typedef struct node{
    REGISTER *info;
    struct node *left, *right;
    int h, bf;
} NODE;

//Struct que define uma árvore e sua informações mais pertinentes
typedef struct{
    int qt;
    NODE *root;
    int (*cmp)(void*, void*);
} TREE;

TREE *avl_tree_create(int (*cmp)(void*, void*));
int avl_tree_read_file(TREE*, char*);
int avl_tree_menu();
int avl_tree_insert(TREE*, REGISTER*);
REGISTER *avl_tree_new_register();
int avl_tree_remove(TREE*, REGISTER*);
int avl_tree_search_by_cpf(TREE*);
int avl_tree_search_by_name(TREE*);
int avl_tree_search_by_date(TREE*);
int avl_tree_remove_by_cpf(TREE*, REGISTER*);
int avl_tree_print_register(REGISTER*);
int avl_tree_print_all_registers(TREE*);
int avl_tree_free(TREE*);

//Função auxiliar para criação de árvore ordenada por CPF
int cmp_by_cpf(void *a, void *b){
    if( ((REGISTER*)a)->cpf < ((REGISTER*)b)->cpf) return -1;
    if( ((REGISTER*)a)->cpf > ((REGISTER*)b)->cpf) return +1;
    else return 0;
}

//Função auxiliar para criação de árvore ordenada por DATA
int cmp_by_date(void *a, void *b){
    if( ((REGISTER*)a)->birth.year < ((REGISTER*)b)->birth.year) return -1;
    else if( ((REGISTER*)a)->birth.year > ((REGISTER*)b)->birth.year) return +1;
    else if( ((REGISTER*)a)->birth.month < ((REGISTER*)b)->birth.month) return -1;
    else if( ((REGISTER*)a)->birth.month > ((REGISTER*)b)->birth.month) return +1;
    else if( ((REGISTER*)a)->birth.day < ((REGISTER*)b)->birth.day) return -1;
    else if( ((REGISTER*)a)->birth.day > ((REGISTER*)b)->birth.day) return +1;
    else return 0;
}

//Função auxiliar para criação de árvore ordenada por NOME
int cmp_by_name(void *a, void *b){
    if( strcmp(((REGISTER*)a)->name, ((REGISTER*)b)->name) < 0 ) return -1;
    else if( strcmp(((REGISTER*)a)->name, ((REGISTER*)b)->name) > 0 ) return +1;
    else return 0;
}

int main(void) {
    REGISTER *aux;
    unsigned long int cpf;
    int op;
    char *file_name = "exemplo.txt";
    printf("Digite 1 se deseja fornecer nome do arquivo de entrada ao inves de usar o arquivo \"%s\" padrao: ", file_name);
    scanf("%d", &op);
    if(op == 1){
        printf("\nDigite o nome do novo arquivo de entrada: ");
        scanf("%ms", &file_name);
    }
    TREE *tree_by_cpf = avl_tree_create(cmp_by_cpf),
        *tree_by_date = avl_tree_create(cmp_by_date),
        *tree_by_name = avl_tree_create(cmp_by_name);
    avl_tree_read_file(tree_by_cpf, file_name);
    avl_tree_read_file(tree_by_date, file_name);
    avl_tree_read_file(tree_by_name, file_name);
    do{
        op = avl_tree_menu();
        switch(op){
            case 0:
                printf("\n\tAté mais! :)");
                break;
            case 1:
                avl_tree_search_by_cpf(tree_by_cpf);
                break;
            case 2:
                avl_tree_search_by_name(tree_by_name);
                break;
            case 3:
                avl_tree_search_by_date(tree_by_date);
                break;
            case 4:
                aux = avl_tree_new_register();
                avl_tree_insert(tree_by_cpf, aux);
                avl_tree_insert(tree_by_name, aux);
                avl_tree_insert(tree_by_date, aux);
                break;
            case 5:
                aux = avl_tree_new_register();
                avl_tree_remove(tree_by_cpf, aux);
                avl_tree_remove(tree_by_name, aux);
                avl_tree_remove(tree_by_date, aux);
                break;
            case 6:
                printf("\n\tESCOLHA O PARAMETRO DE ORDENACAO:\n\t\t1 - Por CPF;\n\t\t2 - Por NOME;\n\t\t3 - Por DATA.\n\n\t\tDigite a opcao escolhida : ");
                scanf("%d", &op);
                switch(op){
                    case 1:
                        avl_tree_print_all_registers(tree_by_cpf);
                        break;
                    case 2:
                        avl_tree_print_all_registers(tree_by_name);
                        break;
                    case 3:
                        avl_tree_print_all_registers(tree_by_date);
                        break;
                    default:
                        printf("\t\tOPCAO NAO RECONHECIDA\n\n");
                        break;
                }
                op = 6;
                break;
            default:
                printf("\tOPCAO NAO RECONHECIDA\n\n");
        }
    } while(op);
    avl_tree_free(tree_by_cpf);

    return 0;
}

//Função para criação de uma árvore que se organiza segundo função de comparação fornecida
TREE *avl_tree_create(int (*cmp)(void*, void*)){
    TREE *tree = (TREE*) calloc(1, sizeof(TREE));
    tree->cmp = cmp;
    return tree;    
}

//Le arquivo para preenchimento inicial da arvore com base no nome do arquivo representado pela string recebida no parâmetro
int avl_tree_read_file(TREE *tree, char *file_name){
    if(!tree) return INVALID_TREE;

    FILE *arq = fopen(file_name, "r");
    do{
        REGISTER *aux = (REGISTER*) calloc(1, sizeof(REGISTER));
        fscanf(arq, "\n%ld", &aux->cpf);
        fscanf(arq, ";%ld", &aux->rg);
        fscanf(arq, ";%m[^;]s", &aux->name);
        fscanf(arq, ";%d/%d/%d", &aux->birth.day, &aux->birth.month, &aux->birth.year);
        fscanf(arq, ";%m[^\n]s\n", &aux->city);
        if(aux->cpf) avl_tree_insert(tree, aux);
        //Opcao para codigo que le os registros do arquivo e os imprime na tela
        // if(aux->cpf != 0){
        //     avl_tree_insert(tree, aux);
        //     avl_tree_print_register(aux);
        // }
    } while(!feof(arq));
    fclose(arq);

    return SUCCESS;
}

//Exibe menu principal de opções
int avl_tree_menu(){
    int op;
    printf("\nMENU DE OPCOES:\n\t1 - Consultar registro por CPF;\n\t2 - Consultar registro por PRIMEIRO NOME;\n\t3 - Consultar registro por INTERVALO ENTRE DATAS;\n\t4 - Inserir novo registro;\n\t5 - Remover registro nas 3 árvores;\n\t6 - Imprimir registros;\n\t0 - SAIR;\n\n\tDigite a opcao escolhida: ");
    scanf("%d", &op);
    // printf("\n");
    return op;
}

//Função para atualização do fator de balanceamento de um nó
int avl_tree_balancing_factor(NODE *node){
    if(!node) return INVALID_NODE;

    node->bf = (!node->right ? 0:node->right->h) - (!node->left ? 0:node->left->h);
    return SUCCESS;
}

//Identifica maior altura possível para o nó
int avl_tree_new_height(NODE *node){
    if(!node) return INVALID_NODE;

    int hl = (!node->left) ? 0:node->left->h, hr = (!node->right) ? 0:node->right->h;
    if(hl > hr) node->h = hl + 1;
    else node->h = hr + 1;

    return SUCCESS;
}

int avl_tree_rotation_rr(NODE **node){
    if(!node) return INVALID_NODE;

    NODE *aux = (*node)->right;
    (*node)->right = aux->left;
    aux->left = *node;
    *node = aux;
    //Recalcula alturas
    avl_tree_new_height((*node)->left);
    avl_tree_new_height(*node);

    return SUCCESS;
}

int avl_tree_rotation_ll(NODE **node){
    if(!node) return INVALID_NODE;

    NODE *aux = (*node)->left;
    (*node)->left = aux->right;
    aux->right = (*node);
    *node = aux;
    //Recalcula alturas
    avl_tree_new_height((*node)->right);
    avl_tree_new_height(*node);

    return SUCCESS;
}

//Percorre a árvore para inserir e retorna para a raiz balanceando-a
NODE *avl_tree_travels_balancing(NODE **node, NODE *new_node, int (*cmp)(void*, void*)){
    if(!*node) *node = new_node;
    else {
        int res = cmp(new_node->info, (*node)->info);
        if(res < 0) avl_tree_travels_balancing(&(*node)->left, new_node, cmp);
        else if(res > 0) avl_tree_travels_balancing(&(*node)->right, new_node, cmp);
        else {
            //Alterar sem adicionar
            free((*node)->info);
            (*node)->info = new_node->info;
            return new_node;
        }
    }

    //Altera altura relativa
    avl_tree_new_height(*node);

    //Calcula fator de balanceamento
    avl_tree_balancing_factor(*node);

    //O fator de balanceamento vem negativo para desbalanceamento à esquerda e positivo para à direita
    if((*node)->bf <= -2) {
        if((*node)->left->bf >= 0) avl_tree_rotation_rr(&(*node)->left);
        avl_tree_rotation_ll(node);
    } else if((*node)->bf >= +2) {
        if((*node)->right->bf < 0) avl_tree_rotation_ll(&(*node)->right);
        avl_tree_rotation_rr(node);
    }

    return new_node;
}

//Amplia contador da árvore e realiza inserção de um novo registro
int avl_tree_insert(TREE *tree, REGISTER *info){
    if(!tree) return INVALID_TREE;

    tree->qt++;
    //Cria novo nó com informações pasadas
    NODE *new_node = (NODE*) calloc(1, sizeof(NODE));
    new_node->info = info;

    avl_tree_travels_balancing(&tree->root, new_node, tree->cmp);

    return SUCCESS;
}

//Recebe dados para criação de um registro
REGISTER *avl_tree_new_register(){

    REGISTER *aux = (REGISTER*) malloc(sizeof(REGISTER));
    printf("\n\tDIGITE ABAIXO AS INFORMACOES DO REGISTRO\n");
    printf("\tCPF : ");
    scanf("%ld", &aux->cpf);
    printf("\tRG : ");
    scanf("%ld", &aux->rg);
    printf("\tNome : ");
    scanf(" %m[^\n]s", &aux->name);
    printf("\tData de nascimento na formatacao DD/MM/AAAA : ");
    scanf(" %d/%d/%d", &aux->birth.day, &aux->birth.month, &aux->birth.year);
    printf("\tCidade Natal : ");
    scanf(" %m[^\n]s", &aux->city);
    printf("\n");

    return aux;
}

//Procura por nó específico na árvore para deleta-lo
int avl_tree_find_to_delete(NODE **node, REGISTER *aux, NODE *dest, int (*cmp)(void*, void*)){
    int res;
    if(dest) {
        if((*node)->right) res = avl_tree_find_to_delete(&(*node)->right, aux, dest, cmp);
        else {
            free(dest->info->name);
            free(dest->info->city);
            free(dest->info);
            dest->info = (*node)->info;
            NODE *to_free = *node;
            *node = (*node)->left;
            free(to_free);
        }
    } else if(!*node) return NODE_NOT_FOUND;
    else {
        if(cmp(aux, (*node)->info) < 0) res = avl_tree_find_to_delete(&(*node)->left, aux, NULL, cmp);
        else if(cmp(aux, (*node)->info) > 0) res = avl_tree_find_to_delete(&(*node)->right, aux, NULL, cmp);
        else {
            res = SUCCESS;
            if((*node)->left) avl_tree_find_to_delete(&(*node)->left, aux, *node, cmp);
            else avl_tree_find_to_delete(node, aux, *node, cmp);
        }
    }

    //Altera altura relativa
    avl_tree_new_height(*node);

    //Calcula fator de balanceamento
    avl_tree_balancing_factor(*node);

    //O fator de balanceamento vem negativo para desbalanceamento à esquerda e positivo para à direita
    if(*node && (*node)->bf <= -2) {
        if((*node)->left->bf >= 0) avl_tree_rotation_rr(&(*node)->left);
        avl_tree_rotation_ll(node);
    } else if(*node && (*node)->bf >= +2) {
        if((*node)->right->bf < 0) avl_tree_rotation_ll(&(*node)->right);
        avl_tree_rotation_rr(node);
    }

    return res;
}

//Função para comportar outra função auxiliar recursiva que trabalha para remover um registro da árvore
int avl_tree_remove(TREE *tree, REGISTER *aux){
    if(!tree) return INVALID_TREE;

    if(avl_tree_find_to_delete(&tree->root, aux, NULL, tree->cmp) == SUCCESS) tree->qt--;
    else printf("\n\tREGISTRO NAO ENCONTRADO\n");

    return SUCCESS;
}

//Procura entre os nós o que possúi o registro que contenha determinado CPF
REGISTER *avl_tree_search_cpf_in_node(NODE *node, unsigned long int cpf, int (*cmp)(void*, void*)){
    if(!node) return NULL;

    REGISTER *to_cmp = (REGISTER*) malloc(sizeof(REGISTER));
    to_cmp->cpf = cpf;
    int res = cmp(to_cmp, node->info);
    free(to_cmp);

    if(res > 0) return avl_tree_search_cpf_in_node(node->right, cpf, cmp);
    else if(res < 0) return avl_tree_search_cpf_in_node(node->left, cpf, cmp);
    else return node->info;
}

//Chama busca por CPF na árvore
int avl_tree_search_by_cpf(TREE *tree){
    if(!tree) return INVALID_TREE;

    unsigned long int cpf;
    printf("\n\tDigite o CPF desejado: ");
    scanf("%ld", &cpf);
    printf("\n");

    REGISTER *answer = avl_tree_search_cpf_in_node(tree->root, cpf, tree->cmp);

    if(answer) {
        avl_tree_print_register(answer);
        return SUCCESS;
    } else {
        printf("\n\tREGISTRO NAO ENCONTRADO\n");
        return REGISTER_NOT_FOUND;
    }
}

//Procura entre os nós os que possuam o registro cujo nome completo se inicia com determinado nome
void *avl_tree_search_name_in_node(NODE *node, char *name){
    if(!node) return NULL;

    if(node->left) avl_tree_search_name_in_node(node->left, name);
    
    if(strncmp(name, node->info->name, strlen(name)) == 0) avl_tree_print_register(node->info);

    if(node->right) avl_tree_search_name_in_node(node->right, name);

    return SUCCESS;
}

//Chama busca por PRIMEIRO NOME na árvore
int avl_tree_search_by_name(TREE *tree){
    if(!tree) return INVALID_TREE;

    char *name;
    printf("\n\tDigite o NOME desejado: ");
    scanf("%ms", &name);
    printf("\n");

    avl_tree_search_name_in_node(tree->root, name);

    return SUCCESS;
}

//Procura entre os nós os que possuam o registro cuja data de nascimento pertença a um intervalo
void *avl_tree_search_date_in_node(NODE *node, DATE *start_date, DATE *end_date, int (*cmp)(void*, void*)){
    if(!node) return NULL;

    int r1, r2;
    REGISTER *to_cmp = (REGISTER*) malloc(sizeof(REGISTER));
    to_cmp->birth = *start_date;
    r1 = cmp(node->info, to_cmp);
    to_cmp->birth = *end_date;
    r2 = cmp(node->info, to_cmp);
    free(to_cmp);

    if(r1 >= 0 && node->left) avl_tree_search_date_in_node(node->left, start_date, end_date, cmp);
    if(r1 >= 0 && r2 <= 0) avl_tree_print_register(node->info);
    if(r2 <= 0 && node->right) avl_tree_search_date_in_node(node->right, start_date, end_date, cmp); 

    return SUCCESS;
}

//Chama busca por INTERVALO ENTRE DATAS na árvore
int avl_tree_search_by_date(TREE *tree){
    if(!tree) return INVALID_TREE;
    
    DATE *start_date = (DATE*) malloc(sizeof(DATE)), *end_date = (DATE*) malloc(sizeof(DATE));
    printf("\n\tDigite a DATA INICIAL seguindo formatacao DD/MM/AAAA: ");
    scanf("%d/%d/%d", &start_date->day, &start_date->month, &start_date->year);
    printf("\n");
    printf("\tDigite a DATA FINAL seguindo formatacao DD/MM/AAAA: ");
    scanf("%d/%d/%d", &end_date->day, &end_date->month, &end_date->year);

    avl_tree_search_date_in_node(tree->root, start_date, end_date, tree->cmp);

    return SUCCESS;
}

//Exibe informação de um registro
int avl_tree_print_register(REGISTER *info){
    if(!info) return INVALID_REGISTER;

    printf("\n-----REGISTRO-----\n");
    printf("CPF: %011ld\nRG: %ld\nNome: %s\nData de Nascimento: %02d/%02d/%04d\nCidade Natal: %s\n\n", info->cpf, info->rg, info->name, info->birth. day, info->birth.month, info->birth.year, info->city);

    return SUCCESS;
}

//Exibe os nós anteriores (menores), o nó atual e os próximos(maioes)
int avl_tree_print_node(NODE *node){
    if(node->left) avl_tree_print_node(node->left);
    avl_tree_print_register(node->info);
    if(node->right) avl_tree_print_node(node->right);

    return SUCCESS;
}

//Função que comporta outra função auxiliar que trabalha para exibir todos os registros contidos em uma árvore
int avl_tree_print_all_registers(TREE *tree){
    if(!tree) return INVALID_TREE;

    printf("\n");
    avl_tree_print_node(tree->root);
    printf("\nTOTAL DE REGISTROS : %d\n\n", tree->qt);

    return SUCCESS;
}

//Libera um nó e seus filhos quando esses existirem
int avl_tree_free_node(NODE *node){
    if(node->left) avl_tree_free_node(node->left);
    if(node->right) avl_tree_free_node(node->right);
    free(node->info->name);
    free(node->info->city);
    free(node);

    return SUCCESS;
}

//Libera os nós da árvore e seu descritor
int avl_tree_free(TREE *tree){
    if(!tree) return INVALID_TREE;

    if(tree->qt != 0) avl_tree_free_node(tree->root);
    free(tree);

    return SUCCESS;
}