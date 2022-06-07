#include <iostream>
#include <stdio.h>
#include "string.h"
#include <queue>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <list>
#include <math.h>
#include <set>

#define MAX_BINARY 24 //zvladne to aj viac(podla RAM systemu)

using namespace std;

struct BFunction{
    string vector; //vektor funkcie
    BFunction(string vec): //konstruktor
    vector(vec){
    }
};

struct BDD{

    BDD(BFunction *bfunc, BDD *parent): //konstruktor
            parent(parent),
            rchild(nullptr),
            lchild(nullptr)
    {
        bfunction = bfunc;
        if(parent != NULL){
            depth = parent->depth+1;
        }else{
            depth = 0;
        }
    }
    BFunction *bfunction; //booleovsky vektor
    BDD *parent, *rchild, *lchild; //pointre na rodica a potomkov
    unsigned long long int depth; //udaj o hlbke
};

typedef set<BDD*> bddList;
typedef vector<bddList> doubleList; //dvojty spajany zoznam

class BDDTree {
private:
    BFunction booleanVector;
    BDD *root;
    int noOfVariables;
    unsigned long long int noOfNodes;
    doubleList uniqueNodeList; //dvojty spajany zoznam unikatnych prvkov podla hlbky
    doubleList BDDNodeList; //spajany zoznam uzlov podla hlbky
public:

    BDDTree(string vector, int variables): //konstrutor triedy
            uniqueNodeList(variables+1),
            BDDNodeList(variables+1),
            root(nullptr),
            booleanVector(vector),
            noOfVariables(variables),
            noOfNodes(0){
    }

    //gettery a setery
    void setVector(string booleanVector){
        BDDTree::booleanVector = booleanVector;
    }

    string getVector(){
        return booleanVector.vector;
    }

    void setRoot(BDD *root) {
        BDDTree::root = root;
    }

    int getNoOfVariables(){
        return noOfVariables;
    }

    doubleList getuniqueNodeList(){
        return uniqueNodeList;
    }
    void setNoOfVariables(int noOfVariables) {
        BDDTree::noOfVariables = noOfVariables;
    }

    int getNoOfNodes(){
        return noOfNodes;
    }

    void setNoOfNodes(int noOfNodes) {
        BDDTree::noOfNodes = noOfNodes;
    }

    BDD *getRoot() {
        return root;
    }

    BDD *newBDD(BFunction *bFunction, BDD *parent) { //funkcia na vytvorenie noveho uzla
        BDD *newNode = new BDD(bFunction, parent);
        if(newNode != NULL) {
            return newNode;
        }
        return NULL;
    }

    string getSubstring(string str, int start, int end) { //funkcia na ziskanie polovice z retazca
        string stemp;
        for (unsigned long long int i = start; i < end; i++) {
            stemp += str[i];
        }
        if (stemp.c_str() != NULL) {
            return stemp;
        }
        return NULL;
    }

    int BDD_reduce(BDD *bdd){
        if(bdd == NULL){ //ak by bol ukazovatel bdd neplatny
            return -1;
        }
        int noOfNodesBeforeReduce = this->noOfNodes; //tu si docasne ulozim plny pocet uzlov
        for(int i = BDDNodeList.size()-1; i > 0; i--){ //prejdem postupne od spodu nahor po urovniach stromu
            for(auto node: BDDNodeList[i]){ //kontrolujem kazdy uzol z uzlami z unikatneho vektora(pola)
                for(auto compareNode : uniqueNodeList[i]){
                    if(node->bfunction->vector == compareNode->bfunction->vector && compareNode != node){ //a ak najdem taky ktory mi vyhovuje)
                        BDD *oldParent = node->parent; //tak ho len nahradim uzlom z unikatneho zoznamu (len prave raz sa tam taky nachadza)
                        if(oldParent->lchild == node){ //tu len musim skontrolovat na akej strane pracujem vzhladom na rodica
                            oldParent->lchild = compareNode;
                            this->noOfNodes--;
                            //node = NULL;
                            delete node; // a vymazem stary uzol
                            node = compareNode;
                            node->parent = oldParent;
                        }
                        else if(oldParent->rchild == node){
                            oldParent->rchild = compareNode;
                            this->noOfNodes--;
                            //delete node;
                            //node = NULL;
                            delete node;
                            node = compareNode;
                            node->parent = oldParent;
                        }
                        break;
                    }
                }
            }
        }
        return noOfNodesBeforeReduce - this->noOfNodes; //a vratim pocet zmanazanych uzlov
    }
    //najprv vytvorim lavy uzol a potom pravy rekurzivne
    void createHelper(BDD *node, string data) { //pomocna funkcia na vytvorenie stromu
        if (data.length() <= 1) {
            return;
        }
        if (node->lchild == NULL) {
            this->noOfNodes++;
            unsigned long long int len = data.length();
            node->lchild = newBDD(new BFunction(getSubstring(node->bfunction->vector, 0, len/2)), node);
            bool exists = false;
            for(auto it = uniqueNodeList[node->lchild->depth].begin(); it != uniqueNodeList[node->lchild->depth].end(); it++){ //tu si len ukladam unikatne uzly do pola unikatnych
                if((*it)->bfunction->vector == (node->lchild->bfunction->vector)){ //uzlov, musim prest zakazdym celym polom lebo funkcia set
                    exists = true; //nevie detekovar rovnost struktur
                    break;
                }
            }
            if(!exists) {
                uniqueNodeList[node->lchild->depth].insert(node->lchild); //ulozim si novy uzol do prvka unikatnych uzlov
            }
            BDDNodeList[node->lchild->depth].insert(node->lchild); //aj do pola uzlov
        }
        createHelper(node->lchild, node->lchild->bfunction->vector); //rekurzia
        if (node->rchild == NULL) { //to iste urobim aj tu len na pravej strane
            this->noOfNodes++;
            unsigned long long int len = data.length();
            node->rchild = newBDD(new BFunction(getSubstring(node->bfunction->vector, len/2,len)), node);
            bool exists = false;
            for(auto it = uniqueNodeList[node->rchild->depth].begin(); it != uniqueNodeList[node->rchild->depth].end(); it++){
                if((*it)->bfunction->vector == (node->rchild->bfunction->vector)){
                    exists = true;
                    break;
                }
            }
            if(!exists) {
                uniqueNodeList[node->rchild->depth].insert(node->rchild);
            }
            BDDNodeList[node->rchild->depth].insert(node->rchild);
        }
        createHelper(node->rchild, node->rchild->bfunction->vector);
    }

    BDD *BDD_create(BFunction *bFunction) {
        if (bFunction->vector.length() % 2 != 0) { //ak by mal vektor nespravnu dlzku tak nic neurobim
            return NULL;
        }
        root = newBDD(bFunction, NULL); //vytvorim koren
        this->noOfNodes++;
        uniqueNodeList[root->depth].insert(root); //samozrejme ho musim pridat do pomocnych poli
        BDDNodeList[root->depth].insert(root);
        createHelper(root, bFunction->vector); //zavolam pomocnu funkciu
        return root; //a vratim koren
    }

    char BDD_use(BDD *bdd, string input) {
        char c[1];
        if(bdd != NULL && input.length() != 0 && input.c_str() != NULL) {
            BDD *temp = bdd;
            for (int i = 0; i < input.length(); i++) {
                if (temp != NULL) {
                    if(temp->bfunction->vector.length() == 1){ //ak skorej narazim na koniec(pri zredukovanom tvare)
                        strcpy(c, temp->bfunction->vector.c_str());
                        return *c; //returns 0 or 1
                    }
                    if (input[i] == '0') {
                        temp = temp->lchild;
                    } else if (input[i] == '1') {
                        temp = temp->rchild;
                    }else {
                        //strcpy(c, "a");
                        return 'a'; //returns a, end invalid character spotted
                    }
                }
            }
            if (temp != NULL && temp->bfunction->vector.length() == 1) {
                strcpy(c, temp->bfunction->vector.c_str());
                return *c; //returns 0 or 1
            }else{
                //strcpy(c, "b");
                return 'b'; //returns b, end of list reached
            }
        }
        //strcpy(c, "c");
        return 'c'; //returns c, invalid input
    }

    void printHelper(BDD *root, string indent, bool last) {
        if (root != NULL) {
            cout << indent;
            if (root == getRoot()) {
                cout << "ROOT->";
                indent += "   ";
            } else {
                if (last) {
                    cout << "R----";
                    indent += "   ";
                } else {
                    cout << "L----";
                    indent += "|  ";
                }
            }
            cout << root->bfunction->vector << "(" << root << ")" << endl;
            printHelper(root->lchild, indent, false);
            printHelper(root->rchild, indent, true);
        }
    }

    void print(BDD *root) {
        if (root != NULL) {
            printHelper(root, "", true);
        }

    }

    void deleteBDD(BDD* node){ //vymazanie stromu po ukoncni prace s nim
        if (node == NULL){
            return;
        }
        deleteBDD(node->lchild);
        deleteBDD(node->rchild);
        delete node;
        memset(node, 0, sizeof(node));
        //node = NULL;
    }

    ~BDDTree(){
        deleteBDD(this->root); 
        this->root = NULL; 
    }
};
