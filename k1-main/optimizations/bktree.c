typedef struct BKTree{
    BKNode* root;
} BKTree;
typedef  struct BKNode{
    char* word;
    BKChild* leaf;
}BKNode;
typedef struct BKChild {
    int distance;
    BKNode* child;
    BKChild* next;
}BKChild;

BKNode* create_node(char* word){
    BKNode* no = malloc(sizeof(BKNode));
    no->word = word;
    no->leaf = NULL;
    return no;
}
BKNode* find_child(BKNode* node, int d ){
    BKChild* actual = node->leaf;
    while (actual != NULL){
        if(actual-> distance == d){
            return actual->child;
        }
        actual = actual->next;
    }
    return NULL;
}

void add_child(BKNode* branch, int d, BKNode* enfant){
    BKChild* aj=  malloc(sizeof(BKChild));
    aj->distance = d;
    aj ->child = enfant;
    
    BKChild* actual = branch->leaf;
    if(actual == NULL){
        aj->next =NULL;
        branch->leaf = aj;
        return ;
    }
    aj->next = actual;
    branch->leaf = aj;
    return;
}

void init_tree(BKTree* arbre){
    arbre ->root = 0;
}

float distance(char* word1, char* word2);

void add_word(BKTree* arbre, char* mot){
    if(arbre->root == NULL){
        arbre->root =create_node(mot)
        return;
    }


}