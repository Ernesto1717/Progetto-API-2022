#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {false,true}bool;
typedef enum {MIN,EXA}tipo_valore;
typedef struct minmax{
    int N;
    tipo_valore val;
}MinMax;


// VINCOLO is the structure that contains the constraints
// certe is the word that is the constraint
// nothere is an array of boolean values that indicates if a letter is not allowed in a certain position
// numcar is an array of MinMax values that indicates a number, that can be the minimum or the maximum, of occurrences a certain letter in the word.
// visto is an array of boolean values that indicates if a letter has already checked during the game
// arrivisti is an array of integer values that indicates the number of occurrences of a certain letter in the word
// index_visti is the index of the last element of the array arrivisti
typedef struct v{
    char *certe;
    bool *nothere[64];
    MinMax numcar[64];
    bool visto[64];
    int arrivisti[64];
    int index_visti;
}VINCOLO;

typedef struct albero{
    char *parola;
    struct albero *left;
    struct albero *right;
}rbtree;

typedef struct EL{
    char *parola;
    struct EL* next;
}lista;


bool FINE=false

int iniziali(rbtree **ptree);
void insertRB(rbtree **ptree,char *temp);
rbtree* insertRBpartita(rbtree **ptree,char *temp);
void nuovapartita(rbtree **ptree);
int parolaexists(rbtree *ptree,char *word);
int parolaammissibile(char *parola,VINCOLO *vincolo);
void stampafiltrate(rbtree *tree);
void stampafiltratelista(lista *Tammessi);
void dealloc (rbtree *rbarr);
int listammissibili(lista **Tammessi,VINCOLO *vincolo);
void insertlista(rbtree *new,lista **linizio);
void mischialista(lista **Tammessi,lista **linizio);
void allocaM(char *parola,lista **Tammessi);
void dealloclista (lista **Tammessi);
void crealista(lista **Tammessi,rbtree *tree,VINCOLO *vincolo);
void costruiscihash(void);


int K=0;
int salvaindex=0;
int totaleglob=0;
int hash[256];
int main() {
    char command[20];
    int resultiniziali;   //serve a sapere se dopo parole iniziali c'è nuova partita o inserisci inizio
    rbtree **ptree,*tree=NULL;
    ptree=&tree;
    costruiscihash();


    if(scanf("%d",&K)){}

    resultiniziali=iniziali(ptree);

    if(resultiniziali==1)nuovapartita(ptree);
    else{
        if(scanf("%s",command)!=true){FINE=true;}
        while(FINE==false&&strcmp(command,"+inserisci_fine")!=0){
            insertRB(ptree,command);
            if(scanf("%s",command)!=true){FINE=true;break;}
        }
    }

    while(FINE==false){
        if(scanf("%s",command)!=true){break;}
        if (!(strcmp(command,"+inserisci_inizio"))){
            if(scanf("%s",command)!=true){break;}
            while(strcmp(command,"+inserisci_fine")!=0){
                insertRB(ptree,command);
                if(scanf("%s",command)!=true){FINE=true;break;}
            }
        }
        else if (command[1]=='n'){ //nuova_partita
            nuovapartita(ptree);
        }
    }
    dealloc(*ptree);
    return 0;
}

/**
 * This function is called when the test starts.
 * Its purpose is to create the first vocabulary three.
 * It creates a new game and it calls the function that creates the list of words that are compatible with the constraints.
 * @param ptree pointer to the vocabulary tree
 */
int iniziali(rbtree **ptree){
    char temp[20];
    while(scanf("%s",temp)==true&&temp[0]!='+'){
        insertRB(ptree,temp);
    }
    if(temp[1]=='n')return 1;
    else return 0;
}

/**
 *
 * @param ptree pointer to the vocabulary tree
 * @param temp word to be inserted
 */
void insertRB(rbtree **ptree,char *temp){
    rbtree* pre=NULL,
            *z=malloc(sizeof(rbtree)),
            *cur;
    z->parola=(char*)malloc(sizeof(char)*(K+1));
    strcpy(z->parola,temp);
    z->right=NULL;
    z->left=NULL;
    cur=*ptree;
    while(cur!=NULL){
        pre=cur;
        if(strcmp(temp,cur->parola)<0){
            cur=cur->left;
        }
        else cur=cur->right;
    }
    if(pre==NULL)*ptree=z;
    else if(strcmp(z->parola,pre->parola)<0)pre->left=z;
    else pre->right=z;
}
/**
 * This function is called just when the inserisci_inizio command is called
 * during a game and the word that is inserted is still compatible with the
 * momentary constraints.
 * @param ptree pointer to the vocabulary tree
 * @param temp word to be inserted
 * @return pointer to the inserted node
 */
rbtree* insertRBpartita(rbtree **ptree,char *temp){
    rbtree* pre=NULL,
            *z=malloc(sizeof(rbtree)),
            *cur;
    z->parola=(char*)malloc(sizeof(char)*(K+1));
    strcpy(z->parola,temp);
    z->right=NULL;
    z->left=NULL;
    cur=*ptree;
    while(cur!=NULL){
        pre=cur;
        if(strcmp(temp,cur->parola)<0){
            cur=cur->left;
        }
        else cur=cur->right;
    }
    if(pre==NULL)*ptree=z;
    else if(strcmp(z->parola,pre->parola)<0)pre->left=z;
    else pre->right=z;
    return z;
}

/**
 * This is the game function. It is called when the command nuova_partita is
 * achieved. It initializes the constraints and the list of the words that
 * are compatible with the constraints.
 * @param ptree is the vocabulary tree
 */
void nuovapartita(rbtree **ptree){
    rbtree *new;
    lista *ammessi=NULL,**Tammessi=&ammessi;
    lista *inizio=NULL,**linizio=&inizio;
    bool boolista;
    boolista=false;
    char rife[K+1],
            temp[20],
            res[K+1];
    res[K]='\0';

    int     n,
            num,
            alfabeto[2][64],   //alfabeto di 0 N
    H;              //alfabeto 1 conta C

    VINCOLO vincolo;                                                    //INIZIALIZZO VINCOLO
    vincolo.certe=(char*)malloc(sizeof(char)*(K+1));
    for(int i=0;i<K;i++) {
        vincolo.certe[i]='$';
    }
    for(int i=0;i<64;i++){
        vincolo.nothere[i]=(bool*)malloc(sizeof(bool)*K);
        for(int g=0;g<K;g++)vincolo.nothere[i][g]=false;
        vincolo.visto[i]=false;
    }
    vincolo.certe[K]='\0';                                              //_______
    vincolo.index_visti=0;

    if(scanf("%s",rife)!=true){FINE=true;return;}

    if(scanf("%d",&n)!=true){FINE=true;return;} //n is the maximum number of tries that the player can do to guess the word
    for(int i=0;i<64;i++){alfabeto[0][i]=0;}
    for(int i=0;i<K;i++){alfabeto[0][hash[(int)rife[i]]]+=1;}

    while(n>0){
        if(scanf("%s",temp)!=true){FINE=true;return;}
        if(temp[0]=='+') {
            if (temp[1] == 's') {       //STAMPAFILTRATE
                if (boolista == false) {
                    stampafiltrate(*ptree);
                }
                else {
                    stampafiltratelista(*Tammessi);
                }
            }else if (temp[12] == 'n'){               //INSERISCI INIZIO

                if (scanf("%s", temp) != true){FINE = true; return;}

                if (boolista == false) {
                    while (FINE == false && strcmp(temp,"+inserisci_fine")!=0) {
                        insertRB(ptree, temp);

                        if (scanf("%s", temp) != true){FINE = true; return;}

                    }
                    if (FINE == true)return;
                } else {
                    while (FINE == false && strcmp(temp,"+inserisci_fine")!=0) {
                        if (parolaammissibile(temp, &vincolo)) {
                            new = insertRBpartita(ptree, temp);

                            insertlista(new, linizio);
                        } else {
                            insertRB(ptree, temp);

                        }
                        if (scanf("%s", temp) != true){FINE = true; return;}
                    }
                    if (FINE == true)return;

                    mischialista(Tammessi, linizio);
                    *linizio = NULL;
                }
            }
        }
        else if(strcmp(rife,temp)==0){
            printf("ok\n");
            dealloclista(Tammessi);
            free(vincolo.certe);
            for(int i=0;i<64;i++) {
                free(vincolo.nothere[i]);
            }
            return;
        }
        else if(parolaexists(*ptree,temp)==0) printf("not_exists\n");  //se non esiste non decremento n

        // Start of checking how similar is the word to the reference word and updating the constraints.
        else{
            n--;
            for(int i=0;i<64;i++){alfabeto[1][i]=0;}    //            inizializzo alfabeto
            for(int i=0;i<K;i++){    //     [+-/-|]
                H=hash[(int)temp[i]];  //calcolo hash del carattere corrente in temp
                if(temp[i]==rife[i]){
                    res[i]='+';
                    alfabeto[1][H]++;
                    vincolo.certe[i]=temp[i];
                }
                else{
                    if(alfabeto[0][H]==0){
                        res[i]='/';       //se temp[i] non compare in rife
                        for(int g=0;g<K;g++)vincolo.nothere[H][g]=true;   //lp
                    }
                    else{
                        res[i]='|';
                        //carattere compare in rife ma non in questa posizione -> vincolo 3
                        vincolo.nothere[H][i]=true;
                    }
                }
            }

            //check third constraint
            for(int i=0;i<K;i++){
                if(res[i]=='|'){
                    num=0;
                    for(int j=0;j<i;j++){
                        if(temp[j]==temp[i]&&temp[j]!=rife[j])num++;
                    }
                    int y=hash[(int)temp[i]];
                    if (num>=(alfabeto[0][y]-alfabeto[1][y])){
                        res[i]='/';

                        vincolo.numcar[y].N=num+alfabeto[1][y];
                        if(vincolo.visto[y]==false){
                            vincolo.arrivisti[vincolo.index_visti]=y;
                            vincolo.index_visti++;
                            vincolo.visto[y]=true;
                        }
                        vincolo.numcar[y].val=EXA;

                        for(y=i+1;y<K;y++){
                            // se ho trovato una lettera in pos scorretta e sufficienti in pos scorretta prima sono sicuro che anche dopo saranno sufficienti
                            if((temp[y]==temp[i])&&temp[y]!=rife[y])res[y]='/';
                        }
                    }
                        // se non sono entrato nell'if vuol dire che | rimane tale-> N è maggiore di num +c (+1 poichè è strett magg)
                    else {
                        if(vincolo.visto[y]==false){
                            vincolo.numcar[y].N = num + alfabeto[1][y] + 1;
                            vincolo.arrivisti[vincolo.index_visti]=y;
                            vincolo.index_visti++;
                            vincolo.visto[y]=true;
                            vincolo.numcar[y].val=MIN;
                        }
                        else if(vincolo.visto[y]==true&&vincolo.numcar[y].val==MIN&&vincolo.numcar[y].N<num + alfabeto[1][y] + 1){
                            vincolo.numcar[y].N = num + alfabeto[1][y] + 1;
                        }
                    }
                }
            }

            printf("%s\n",res);
            if(boolista==false){
                boolista=true;
                totaleglob=0;
                crealista(Tammessi,*ptree,&vincolo);
                printf("%d\n",totaleglob);
            }
            else{
                printf("%d\n",listammissibili(Tammessi,&vincolo));
            }
        }
    }
    printf("ko\n");
    dealloclista(Tammessi);
    free(vincolo.certe);
    for(int i=0;i<64;i++){
        free(vincolo.nothere[i]);
    }
}

/**
 * @param ptree the vocabulary tree
 * @param word the word to check
 * @return 1 if the word exists, 0 otherwise
 */
int parolaexists(rbtree *ptree,char *word){
    while(ptree!=NULL){
        if(strcmp(word,ptree->parola)<0){
            ptree=ptree->left;
        }
        else if(strcmp(word,ptree->parola)==0)return 1;
        else ptree=ptree->right;
    }
    return 0;
}

/**
 * this function checks if a word is admissible according to the constraints
 * inferred from the previous guesses.
 * @param parola is the word to check
 * @param vincolo is the constraint structure
 * @return 1 if the word is admissible, 0 otherwise
 */
int parolaammissibile(char *parola,VINCOLO *vincolo){
    int i;
    for(i=0;i<K;i++){
        if((*vincolo).certe[i]!='$'&&(*vincolo).certe[i]!=parola[i])return 0;
        
    }
    
    for(i=0;i<K;i++){
        
        if((*vincolo).nothere[hash[(int)parola[i]]][i]==true)return 0;
    }

    int occorrenze[64]={0};
    MinMax save;
    for(i=0;i<K;i++)occorrenze[hash[(int)parola[i]]]++;   //occorrenze lettere nella parola

    for(i=0;i<(*vincolo).index_visti;i++){
        save=(*vincolo).numcar[(*vincolo).arrivisti[i]];
        if(save.val==MIN&&occorrenze[(*vincolo).arrivisti[i]]<save.N)return 0;
        else if (save.val==EXA&&occorrenze[(*vincolo).arrivisti[i]]!=save.N)return 0;
    }
    return 1;
}


void dealloc (rbtree *rbarr){
    if(rbarr==NULL)return;
    if(rbarr->left!=NULL)dealloc(rbarr->left);
    if(rbarr->right!=NULL)dealloc(rbarr->right);
    free(rbarr->parola);
    free(rbarr);
}

/**
 * this function is called when the command stampafiltrate is given in input
 * during a game before the first guess. It prints all the words in the vocabulary.
 * @param tree is the pointer to the tree
 */
void stampafiltrate(rbtree *tree){
    if(tree==NULL)return;
    if(tree->left!=NULL)stampafiltrate(tree->left);
    printf("%s\n",tree->parola);
    if(tree->right!=NULL)stampafiltrate(tree->right);
}

/**
 * this function is called when the command stampafiltratelista is given in input
 * during a game after the first guess. It prints all the words in the vocabulary.
 * @param Tammessi is the pointer to the list where the admissible words are stored
 */
void stampafiltratelista(lista *Tammessi){
    while(Tammessi!=NULL){
        printf("%s\n",Tammessi->parola);
        Tammessi=Tammessi->next;
    }
}

void crealista(lista **Tammessi,rbtree *tree,VINCOLO *vincolo){
    if(tree==NULL)return;
    crealista(Tammessi,tree->right,vincolo);
    if(parolaammissibile(tree->parola,vincolo)){
        allocaM(tree->parola,Tammessi);
        totaleglob++;
    }
    crealista(Tammessi,tree->left,vincolo);
    return;
}


int listammissibili(lista **Tammessi,VINCOLO *vincolo){
    lista *punt=*Tammessi,*prec=NULL;
    int totale=0;
    while(punt!=NULL){
        if(parolaammissibile(punt->parola,vincolo)){
            totale++;
            if(prec==NULL)prec=*Tammessi;
            else prec=prec->next;
            punt=punt->next;
        }
        else{
            if(punt==*Tammessi){
                punt=punt->next;
                free(*Tammessi);
                *Tammessi=punt;
            }
            else{
                prec->next=punt->next;
                free(punt);
                punt=prec->next;
            }
        }
    }
    return totale;
}

void insertlista(rbtree *new,lista **linizio){
    lista *punt=*linizio,*prec=NULL,*temp;
    while(punt!=NULL&&strcmp(punt->parola,new->parola)<0){
        if(prec==NULL)prec=punt;
        else prec=prec->next;
        punt=punt->next;
    }

    if(prec==NULL){
        temp=malloc(sizeof(lista));
        temp->parola=new->parola;
        temp->next=*linizio;
        *linizio=temp;
    }
    else{
        temp=malloc(sizeof(lista));
        temp->parola=new->parola;
        temp->next=prec->next;
        prec->next=temp;
    }
}

/**
 * This function has the purpose of merging the two lists of words that are still admissible.
 * The first list is the one that contains all the words in the vocabulary, the second one is the one
 * that contains tha words that hasve just been inserted in the tree.
 * @param Tammessi
 * @param linizio
 */
void mischialista(lista **Tammessi,lista **linizio){
    lista *puntl=*linizio,*puntt=*Tammessi,*prec=NULL;
    while(puntl!=NULL&&puntt!=NULL){
        if(strcmp(puntl->parola,puntt->parola)>0){
            if(prec==NULL)prec=puntt;
            else prec=prec->next;
            puntt=puntt->next;
        }
        else{
            if(puntt==*Tammessi){
                *Tammessi=puntl;
                puntl=puntl->next;
                (*Tammessi)->next=puntt;
                if(prec==NULL)prec=*Tammessi;
            }
            else{
                prec->next=puntl;
                puntl=puntl->next;
                prec->next->next=puntt;
                prec=prec->next;
            }
        }
    }
    if(puntt==NULL&&prec!=NULL)prec->next=puntl;
}


void dealloclista (lista **Tammessi){
    lista *punt=*Tammessi, *temp=*Tammessi;
    while(punt!=NULL){
        punt=punt->next;
        free(temp);
        temp=punt;
    }
}

void allocaM (char *parola,lista **Tammessi){
    lista *punt;
    punt=malloc(sizeof(lista));
    punt->parola=parola;
    punt->next=*Tammessi;
    *Tammessi=punt;
}

void costruiscihash(void){
    hash[45]=0;
    hash[95]=37;
    int Y=1;
    for(int i=48;i<58;i++){
        hash[i]=Y;
        Y++;
    }
    for(int i=65;i<91;i++){
        hash[i]=Y;
        Y++;
    }
    Y++;
    for(int i=97;i<123;i++){
        hash[i]=Y;
        Y++;
    }
}

