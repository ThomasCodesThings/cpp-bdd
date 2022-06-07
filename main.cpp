#include <iostream>
#include "BDDTree.h"

int random(){
    return ((int)rand() % 2);
}

string generateBooleanFunctionOutputString(unsigned long long int n){
    srand(time(NULL));
    string str;
    for(int i = 0; i < n; i++){
        ostringstream convert;
        convert << random();
        str += convert.str();
    }
    return str;
}

string decimalToBinary(unsigned long long int decimal, unsigned long long int stringSize){
    string str;
    int binary[stringSize];
    unsigned long long int i = 0;
    while(decimal >= 0 && i < stringSize){
        binary[i] = decimal % 2;
        decimal = decimal/2;
        i++;
    }
    for(int j = i-1; j >= 0; j--){
        ostringstream os;
        os << binary[j];
        str += os.str();
    }
    return str;
}


void inputTester(string inputString, int variables, int numberOfTrees){
    cout << "Starting test for " << variables << " variables and " << numberOfTrees << " trees." << endl;
    double BDDcreateTime = 0;
    double BDDreduceTime = 0;
    double BDDuseTime = 0;
    unsigned long long int numberOfNodes = 0;
    unsigned long long int numberOfDeletedNodes = 0;
    unsigned long long int totalNodesAfterReduction = 0;
    unsigned long long int exponent = exp2(variables);
    unsigned long long int numberOfTests = 0;
    unsigned long long int completedTrees = 0;
    string vectorString;
    if(inputString != ""){
        vectorString = inputString;
    }else {
        vectorString = generateBooleanFunctionOutputString(exponent);

    }
    for(numberOfTests = 0; numberOfTests < numberOfTrees; numberOfTests++) {
        BDDTree tree = BDDTree(vectorString, variables);
        clock_t start = clock();
        BDD *bdd;
        bdd = tree.BDD_create(new BFunction(vectorString));
        numberOfNodes += tree.getNoOfNodes();
        clock_t end = clock();
        BDDcreateTime += ((double) (end - start) / CLOCKS_PER_SEC);
        start = clock();
        numberOfDeletedNodes += tree.BDD_reduce(bdd);
        totalNodesAfterReduction += tree.getNoOfNodes();
        end = clock();
        BDDreduceTime += ((double) (end - start) / CLOCKS_PER_SEC);
        unsigned long long int successfulAttempts = 0;
        int k = 0;
        for (unsigned long long int j = 0; j < exponent; j++) {
            string * str = new string(decimalToBinary(j, variables));
            start = clock();
            if (tree.BDD_use(bdd, str->c_str()) == vectorString[j]) {
                successfulAttempts++;
            }
            end = clock();
            delete str;
            BDDuseTime += ((double) (end - start) / CLOCKS_PER_SEC);
        }
        //tree.printHelper(bdd, " ", true);
        double success = (double) (successfulAttempts / exponent) * 100;
        if (success == 100.0) {
            completedTrees++;
        }

    }

    double success = ((double) completedTrees / numberOfTests) * 100;
    cout <<"Found " << completedTrees <<  " input vectors(trees) out of " << numberOfTests  << " (" << setprecision(5) << success << "%)." << endl;
    double timePerOneTree = BDDcreateTime/ (double) numberOfTests;
    cout << "Operation BDD_create took " << fixed << BDDcreateTime << " seconds for " << numberOfTests << " operations, " << fixed << timePerOneTree << " seconds per 1 tree." << endl;
    cout << "Total number of nodes in BDD tree before BDD_reduce: " << numberOfNodes << " for " << numberOfTests << " trees." << endl;
    cout << "Average number of nodes in one BDD tree before BDD_reduce: " << numberOfNodes/numberOfTests << " per one tree." << endl;
    timePerOneTree = BDDreduceTime / (double) numberOfTests;
    cout << "Operation BDD_reduce took " << fixed << BDDreduceTime << " seconds for " << numberOfTests << " operations, " << fixed << timePerOneTree << " seconds per 1 tree." << endl;
    timePerOneTree = BDDuseTime / (double) numberOfTests;
    double totalReduction = ((double) (numberOfDeletedNodes) / numberOfNodes) * 100;
    cout << "Total umber of nodes in BDD tree after BDD_reduce: " <<  totalNodesAfterReduction << " (deleted " << numberOfDeletedNodes << ")" << " per " << numberOfTests << " tree." << endl;
    cout << "Average number of nodes in one BDD tree after BDD_reduce: " << totalNodesAfterReduction/numberOfTests << " (deleted " << (int) numberOfDeletedNodes/numberOfTests << ")" << " per one tree." << endl;
    cout << "Operation BDD_use took " << fixed << BDDuseTime << " seconds for " << numberOfTests << " operations, " << fixed << timePerOneTree << " seconds per 1 tree." << endl;
    cout << "Average reduction " << setprecision(5) << totalReduction << "%" << endl;
    cout << "***********************************************************" << endl << endl << endl;

}

void ownInputTest(){
    cout << "How many times you want to try?" << endl;
    int times = 0;
    cin >> times;
    BDDTree BDDTree("00",10);
    cout << "Insert vector" << endl;
    string vector;
    cin >> vector;

    BDD *root = NULL;
    root = BDDTree.BDD_create(new BFunction(vector));
    if(root != NULL) {
        BDDTree.setRoot(root);
        for (int i = 0; i < times; i++) {
            string data;
            cout << "Enter combination" << endl;
            cin >> data;
            if(data.length() > log2(BDDTree.getRoot()->bfunction->vector.length())){
                cout << "Try again with smaller length" << endl;
            }else {
                char c = BDDTree.BDD_use(root, data);
                if (c == '0' || c == '1') {
                    cout << "Result for \"" << data << "\" is '" << c << "'." << endl;
                } else {
                    cout << "error code: " << c << endl;
                }
            }
        }
    }else{
        cout << "null tree" << endl;
        return;
    }
}

void tester(){
    start:
    cout << "Press 0 for automatic test of BDD up to " << MAX_BINARY << " different variables, press 1 to do own test, press 2 to do try own vector input, press 3 to do own test" << endl;
    cout << "Press 4 to end tester." << endl;
    int choice = 0;
    int noOfVariables = 0;
    string inputVector;
    int numberOfTrees = 0;
    cin >> choice;
    switch (choice) {
        case 0:
            cout << "Starting automatic test!(up to " << MAX_BINARY << " different variables)." << endl;
            for(int i = 1; i <= MAX_BINARY; i++){
                inputTester("", i, 2000);
            }
            break;
        case 1:
            cout << "Insert amount of variables in boolean function (max " << MAX_BINARY << ")." << endl;
            cin >> noOfVariables;
            cout << "How many trees do you want to create?" << endl;
            cin >> numberOfTrees;
            if(numberOfTrees > 0 && noOfVariables > 0 && noOfVariables <= MAX_BINARY) {
                inputTester("", noOfVariables, numberOfTrees);
            }else{
                cout << "Invalid input!" << endl;
            }
            break;
        case 2:
            cout << "Insert vector:" << endl;
            cin >> inputVector;
            cout << "How many trees do you want to create?" << endl;
            cin >> numberOfTrees;
            if(inputVector.length() % 2 == 0){
                inputTester(inputVector, log2(inputVector.length()), numberOfTrees);
            }else{
                cout << "Invalid size of vector!" << endl;
            }
            break;
        case 3:
            ownInputTest();
            break;
        case 4:
            return;
        default:
            cout << "Invalid choice! Try again." << endl;
            goto start;
    }
    cout << endl << "Want to trest again?? [y/n]" << endl;
    char c;
    cin >> c;
    if(c == 'y'){
        goto  start;
    }
    return;
}
int main() {
    tester();
    return 0;
}
