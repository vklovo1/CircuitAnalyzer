//
// Created by 2570p on 24.03.2020..
//

#include <algorithm>
#include <QTextStream>
#include "Circuit.h"
#include "CMatrix.h"

Circuit::Circuit(const vector<Branch> &branches) {
    this->branches = branches;
}

Circuit::Circuit() {
    numberOfNodes = 0;
    branches = std::vector<Branch>();
}

const vector<Branch> &Circuit::getBranches() const {
    return branches;
}

vector<Branch> &Circuit::getBranches() {
    return branches;
}

void Circuit::setBranches(const vector<Branch> &branches) {
    Circuit::branches = branches;
}

void Circuit::addBranch(Branch &branch) {
    branches.push_back(branch);
}

void Circuit::removeBranch(const Branch &branch) {
    for (int i = 0; i < branches.size(); i++) {
        if (branches.at(i) == branch) {
            branches.erase(branches.begin() + i);
            break;
        }
    }
}

vector<Branch> Circuit::getBranchesContainingNode(Node node) {
    vector<Branch> branchesContainingNode;
    for (auto &b : branches)
        if (b.getFirstNode() == node || b.getSecondNode() == node)
            branchesContainingNode.push_back(b);
    return branchesContainingNode;
}

int Circuit::getNumberOfBranchesFromNode(Node node) {
    int numberOfBranches = 0;

    for (Branch b : branches)
        if (b.getFirstNode() == node || b.getSecondNode() == node)
            numberOfBranches++;

    return numberOfBranches;
}

//adding a component creates a new branch for it
//this method removes obsolete branches that are actually in series

void Circuit::removeObsoleteBranches() {

    for (auto i = 0; i < branches.size(); i++) {
        Branch b = branches.at(0);
        Node nodeToGetRidOf;
        if (getNumberOfBranchesFromNode(b.getFirstNode()) == 2)
            nodeToGetRidOf = b.getFirstNode();
        else if (getNumberOfBranchesFromNode(b.getSecondNode()) ==2)
            nodeToGetRidOf = b.getSecondNode();
        else {
            i++;
            continue;
        }

        auto branchesContainingNode = getBranchesContainingNode(nodeToGetRidOf);
        auto firstBranch = branchesContainingNode[0];
        auto secondBranch = branchesContainingNode[1];
        Branch newBranch;
        Node firstNode;
        Node secondNode;

        bool firstBranchOrientationChanged;
        bool secondBranchOrientationChanged;

        if (firstBranch.getFirstNode() == nodeToGetRidOf) {
            firstNode = firstBranch.getSecondNode();
            firstBranchOrientationChanged = true;
        } else {
            firstNode = firstBranch.getFirstNode();
            firstBranchOrientationChanged = false;
        }

        if (secondBranch.getFirstNode() == nodeToGetRidOf) {
            secondNode = secondBranch.getSecondNode();
            secondBranchOrientationChanged = false;
        } else {
            secondNode = secondBranch.getFirstNode();
            secondBranchOrientationChanged = true;
        }

        if (!firstBranchOrientationChanged)
            newBranch += firstBranch;
        else {
            std::for_each(firstBranch.getVoltageSources().begin(), firstBranch.getVoltageSources().end(),
                          [](VoltageSource &c) -> void { c.toggleOrientation(); });
            std::for_each(firstBranch.getCurrentSources().begin(), firstBranch.getCurrentSources().end(),
                          [](CurrentSource &c) -> void { c.toggleOrientation(); });
            newBranch += firstBranch;
        }

        if (!secondBranchOrientationChanged)
            newBranch += secondBranch;
        else {
            std::for_each(secondBranch.getVoltageSources().begin(), secondBranch.getVoltageSources().end(),
                          [](VoltageSource &c) -> void { c.toggleOrientation(); });
            std::for_each(secondBranch.getCurrentSources().begin(), secondBranch.getCurrentSources().end(),
                          [](CurrentSource &c) -> void { c.toggleOrientation(); });
            newBranch += secondBranch;
        }

        newBranch.setId(firstBranch.getId());
        newBranch.setNodes(firstNode, secondNode);
        removeBranch(firstBranch);
        removeBranch(secondBranch);
        addBranch(newBranch);

        i = -1;
    }
}

void Circuit::removeBranchesWithInfiniteResistance() {
    for (auto b : branches) {
        if (fabs(b.getResistance() + 1) < EPSILON)
            removeBranch(b);
    }
}

void Circuit::shortConnectBranchesWithZeroResistance() {
    for (auto b : branches) {
        if (fabs(b.getResistance()) < EPSILON) {
            Node firstNode = b.getFirstNode();
            Node secondNode = b.getSecondNode();
            auto branchesContainingSecondNode = getBranchesContainingNode(secondNode);

            for (auto b: getBranchesContainingNode(secondNode)) {
                if (b.getFirstNode() == secondNode)
                    b.setFirstNode(firstNode);
                if (b.getSecondNode() == secondNode)
                    b.setSecondNode(firstNode);
            }
            removeBranch(b);
        }
    }
}

void Circuit::drawWire(int firstNodeID, int secondNodeID) {
    int lesserNodeIndex, higherNodeIndex;

    if (firstNodeID < secondNodeID) {
        lesserNodeIndex = firstNodeID;
        higherNodeIndex = secondNodeID;
    } else {
        lesserNodeIndex = secondNodeID;
        higherNodeIndex = firstNodeID;
    }

    for (Branch b : branches) {
        if (b.getFirstNode().getId() == higherNodeIndex)
            b.setFirstNode(Node(lesserNodeIndex));
        else if (b.getSecondNode().getId() == higherNodeIndex)
            b.setSecondNode(Node(lesserNodeIndex));
    }
}

int Circuit::getNumberOfBranches() {
    return branches.size();
}

int Circuit::getNumberOfNodes() {
    std::set<int> distinctNodes;
    for (Branch b : branches) {
        distinctNodes.insert(b.getFirstNode().getId());
        distinctNodes.insert(b.getSecondNode().getId());
    }
    return distinctNodes.size();
}

std::set<Node> Circuit::getNodes() {
    std::set<Node> distinctNodes;
    for (Branch b : branches) {
        distinctNodes.insert(b.getFirstNode().getId());
        distinctNodes.insert(b.getSecondNode().getId());
    }
    return distinctNodes;
}

void Circuit::addResistorToCircuit(const Resistor &r, int firstNodeID, int secondNodeID) {
    Branch newBranch(getAvailableBranchId(), Node(firstNodeID), Node(secondNodeID));
    newBranch.addResistor(r);
    addBranch(newBranch);
}

void Circuit::addVoltageSourceToCircuit(const VoltageSource &v, int firstNodeID, int secondNodeID) {
    Branch newBranch(getAvailableBranchId(), Node(firstNodeID), Node(secondNodeID));
    newBranch.addVoltageSource(v);
    addBranch(newBranch);
}

void Circuit::addCurrentSourceToCircuit(CurrentSource c, int firstNodeID, int secondNodeID) {
    Branch newBranch(getAvailableBranchId(), Node(firstNodeID), Node(secondNodeID));
    addResistorToCircuit(Resistor(c.getInternalResistance()), firstNodeID, secondNodeID);
    c.setIdeal();
    newBranch.addCurrentSource(c);
    addBranch(newBranch);
}

void Circuit::addVoltmeterToCircuit(Voltmeter v, int firstNodeID, int secondNodeID) {
    Branch newBranch(getAvailableBranchId(), Node(firstNodeID), Node(secondNodeID));
    newBranch.addResistor(Resistor(v.getInternalResistance()));
    addBranch(newBranch);

    VoltmeterWrapper voltmeterWrapper(v, Node(firstNodeID), Node(secondNodeID), &newBranch.getResistors().front());
    voltmeters.push_back(voltmeterWrapper);
}

void Circuit::addAmpermeterToCircuit(Ampermeter a, int firstNodeID, int secondNodeID) {
    Branch newBranch(getAvailableBranchId(), Node(firstNodeID), Node(secondNodeID));
    newBranch.addResistor(Resistor(a.getInternalResistance()));
    addBranch(newBranch);

    AmpermeterWrapper ampermeterWrapper(a, newBranch, &newBranch.getResistors().front());
    ampermeters.push_back(ampermeterWrapper);
}

//Returns all branches in the circuit that do not have Current Source
vector<Branch> Circuit::getBranchesWithoutCurrentSource() {
    vector<Branch> branchesWithoutCurrentSource;
    for (auto b: branches)
        if (!b.hasCurrentSources()) branchesWithoutCurrentSource.push_back(b);
    return branchesWithoutCurrentSource;
}

//Check whether the given Node is in the Vector
bool Circuit::isNodeInNodeVector(const Node &nodeToCheck, const vector<Node> &visitedNodes) {
    for (int i = 0; i < visitedNodes.size(); i++) {
        if (nodeToCheck == visitedNodes[i])
            return true;
    }
    return false;
}

//It is important to avoid branches that have Current Sources getBranchesWithoutCurrentSourceFromVector returns all branches that don't contain CS
vector<Branch> Circuit::getBranchesWithoutCurrentSourceFromVector(vector<Branch> vectorToCheck) {
    vector<Branch> vectorToReturn;
    for (auto b : vectorToCheck) {
        if (!b.hasCurrentSources()) vectorToReturn.push_back(b);
    }
    return vectorToReturn;
}

//Minimum Spanning Tree returns vector of Branches that make the MST
vector<Branch> Circuit::getMinimumSpanningTree() {
    Node startingNode = branches[0].getFirstNode();
    Node currentNode = startingNode;
    vector<Node> visitedNodes;
    vector<Branch> treeBranches;
    std::stack<Node> orderOfVisitedNodes;
    while (true) {
        vector<Branch> branchesContainingNode = this->getBranchesContainingNode(currentNode);
        branchesContainingNode = getBranchesWithoutCurrentSourceFromVector(branchesContainingNode);
        if (currentNode == startingNode && isNodeInNodeVector(startingNode, visitedNodes))
            break;
        visitedNodes.push_back(currentNode);
        for (int i = 0; i < branchesContainingNode.size(); i++) {
            if (branchesContainingNode.at(i).getFirstNode() == currentNode &&
                !isNodeInNodeVector(branchesContainingNode.at(i).getSecondNode(), visitedNodes)) {
                treeBranches.push_back(branchesContainingNode.at(i));
                orderOfVisitedNodes.push(currentNode);
                currentNode = branchesContainingNode.at(i).getSecondNode();
                break;
            } else if (branchesContainingNode.at(i).getSecondNode() == currentNode &&
                       !isNodeInNodeVector(branchesContainingNode.at(i).getFirstNode(), visitedNodes)) {
                treeBranches.push_back(branchesContainingNode.at(i));
                orderOfVisitedNodes.push(currentNode);
                currentNode = branchesContainingNode.at(i).getFirstNode();
                break;
            } else {
                if (i == branchesContainingNode.size() - 1) {
                    currentNode = orderOfVisitedNodes.top();
                    orderOfVisitedNodes.pop();
                }
            }
        }
    }

    return treeBranches;
}

bool Circuit::isBranchInTheTree(Branch branchToCheck) {
    vector<Branch> minimumSpanningTree = getMinimumSpanningTree();
    for (const auto &b : minimumSpanningTree) {
        if (branchToCheck == b)
            return true;
    }
    return false;
}

//CoTree represents vector of branches that are not in the Minimum Spanning tree -- These branches are used in Loops
vector<Branch> Circuit::getCoTree() {
    vector<Branch> minimumSpanningTree = getMinimumSpanningTree();
    vector<Branch> freeBranches;
    for (int i = 0; i < getNumberOfBranches(); i++) {
        if (!isBranchInTheTree(branches[i]))
            freeBranches.push_back(branches[i]);
    }
    freeBranches = getBranchesWithoutCurrentSourceFromVector(freeBranches);
    return freeBranches;
}

//Loops are used to make equations in the Second Kirchoffs Law -- Function getLoops() return matrix of elements
//Each row represents one loop, number of rows = number of loops
//Each element represents branch entering the given loop
vector<vector<Branch>> Circuit::getLoops() {
    vector<Branch> currentLoop;
    vector<Branch> freeBranches = getCoTree();
    vector<Branch> branchesContainingNode;
    vector<Node> visitedNodes;
    bool goBack = false;
    Node endingNode;
    Node currentNode;
    std::stack<Node> orderOfVisitedNodes;
    vector<vector<Branch>> loops; //Matrix of branches with each row containing branches that enter one loop
    for (int i = 0; i < freeBranches.size(); i++) {
        currentNode = freeBranches[i].getFirstNode();
        endingNode = freeBranches[i].getSecondNode();
        currentLoop.push_back(freeBranches[i]);
        visitedNodes.clear();
        while (true) {
            if (goBack) {
                currentNode = orderOfVisitedNodes.top();
                orderOfVisitedNodes.pop();
                if (currentLoop.size() == 1 || currentLoop.size() == 0) currentLoop.clear();
                else currentLoop.erase(currentLoop.begin() + currentLoop.size() - 1);
            }
            visitedNodes.push_back(currentNode);
            if (currentNode == endingNode)break;
            branchesContainingNode = this->getBranchesContainingNode(currentNode);
            goBack = false;
            for (int j = 0; j < branchesContainingNode.size(); j++) {
                if (isBranchInTheTree(branchesContainingNode[j])) {
                    if (!isNodeInNodeVector(branchesContainingNode[j].getFirstNode(), visitedNodes)) {
                        orderOfVisitedNodes.push(currentNode);
                        currentNode = branchesContainingNode[j].getFirstNode();
                        currentLoop.push_back(branchesContainingNode[j]);
                        break;
                    } else if (!isNodeInNodeVector(branchesContainingNode[j].getSecondNode(), visitedNodes)) {
                        orderOfVisitedNodes.push(currentNode);
                        currentNode = branchesContainingNode[j].getSecondNode();
                        currentLoop.push_back(branchesContainingNode[j]);
                        break;
                    }
                }
                if (j == branchesContainingNode.size() - 1)
                    goBack = true; //dead end - no tree branches to connect the loop, go back
            }
        }
        loops.push_back(currentLoop);
        currentLoop.clear();
    }
    return loops;
}

int Circuit::getnumberOfLoops() {
    return getLoops().size();
}

//Function isBranchInTheVector returns true if the given branch is in the Vector
bool Circuit::isBranchInTheVector(const Branch &branchToCheck, const vector<Branch> &vectorBranches) {
    for (auto b : vectorBranches) {
        if (b == branchToCheck) return true;
    }
    return false;
}

//First Kirchoffs Law returning the int matrix of elements
//Rows representing Nodes for Kirchoffs Law, the number of Rows is numberOfNodes - 1
//Coloumns represent currents which are sorted same as the branches in branches vector of a circuit
vector<vector<int>> Circuit::firstKirchhoffsLaw() {
    int numberOfNodes = getNumberOfNodes();
    vector<vector<int>> matrixOfCurrents;
    vector<Branch> branchesContainingNode;
    vector<int> currentEquation;
    vector<Node> visitedNodes;
    std::set<Node> nodesInTheCircuit = getNodes();
    for (const auto &n : nodesInTheCircuit) {
        currentEquation.clear();
        branchesContainingNode = getBranchesContainingNode(n);
        for (const auto &b : branches) {
            if (isBranchInTheVector(b, branchesContainingNode)) {
                if (n == b.getFirstNode())
                    currentEquation.push_back(-1);
                else if (n == b.getSecondNode()) currentEquation.push_back(1);
            } else currentEquation.push_back(0);
        }

        matrixOfCurrents.push_back(currentEquation);
    }
    return matrixOfCurrents;
}

std::ostream &operator<<(std::ostream &os, const Circuit &c) {
    for (auto b : c.branches) {
        os << b << std::endl;
    }
    return os;
}

bool Circuit::isBranchInTheLoop(Branch branchToCheck, int indexOfALoop) {
    vector<Branch> loopToCheck = getLoops().at(indexOfALoop);
    for (int i = 0; i < loopToCheck.size(); i++) {
        if (branchToCheck == loopToCheck.at(i))return true;
    }
    return false;
}

Node Circuit::commonNode(Branch firstBranch, Branch secondBranch) {
    if (firstBranch.getFirstNode() == secondBranch.getFirstNode() ||
        firstBranch.getFirstNode() == secondBranch.getSecondNode())
        return firstBranch.getFirstNode();
    return firstBranch.getSecondNode();
}

//getIndexOfABranchInBranches returns the index value of a branch in branches vector of a circuit
int Circuit::getIndexOfABranchInBranches(Branch branchToCheck) {
    for (int i = 0; i < getNumberOfBranches(); i++) {
        if (branchToCheck == branches.at(i)) return i;
    }
    return -1;
}

//Second Kirchoffs Law returning matrix of double elements
//Rows represent Loops -- Number of loops = number of rows
//Coloumns represent voltage drops on each Branch, Branches are sorted same as vector Branches of the circuit
//Last Coloumn represents sum of all Voltage Sources in the loop
std::vector<std::vector<double>> Circuit::secondKirchoffsLaw() {
    vector<vector<Branch>> loopsInCircuit = getLoops();
    vector<vector<double>> matrixSecondKirchoffRule;
    vector<double> currentEquation;
    vector<Branch> currentLoop;
    double resistanceOfABranch;
    double voltageOfABranch;
    double sumOfVoltageSourcesInLoop = 0;
    Node startingNode;
        for (int i = 0;
             i <
             getnumberOfLoops(); i++) { // i represents index number of a loop in a loop matrix, rows represent loops
            sumOfVoltageSourcesInLoop = 0;
            currentEquation.clear();
            currentEquation.resize(getNumberOfBranches(), 0); //make placeholders in currentEquation
            currentLoop = loopsInCircuit.at(i);
            startingNode = currentLoop.at(0).getFirstNode();
            //Go through the Loop

            for (int j = 0; j < currentLoop.size(); j++) {
                int indexOfABranchInBranchesVector = getIndexOfABranchInBranches(currentLoop.at(j));
                Node commonNodeOfBranches;
                //check the direction of the loop (checking which node is the common node for this branch and the next one)
                if (j !=
                    currentLoop.size() -
                    1) //If this is the last branch in the loop, check it with the first (starting one)
                    commonNodeOfBranches = commonNode(currentLoop.at(j), currentLoop.at(j + 1));
                else
                    commonNodeOfBranches = commonNode(currentLoop.at(j),
                                                      currentLoop.at(0)); //else check it with the next one
                if (currentLoop.size() ==
                    2) { //SPECIAL CASE -- Loop has only two branches -- they have both nodes common
                    if ((commonNodeOfBranches == currentLoop.at(0).getFirstNode() &&
                         //If they have same node orientation
                         commonNodeOfBranches == currentLoop.at(1).getFirstNode()) ||
                        // FirstNode of first branch is same as First Node second branch
                        (commonNodeOfBranches == currentLoop.at(0).getSecondNode() &&
                         //SecondNude of first branch same as second node second branch
                         commonNodeOfBranches == currentLoop.at(1).getSecondNode())) {
                        //first branch
                        resistanceOfABranch = currentLoop.at(0).getResistance();
                        voltageOfABranch = currentLoop.at(0).getVoltageFromVoltageSources();
                        currentEquation.at(indexOfABranchInBranchesVector) = resistanceOfABranch;
                        sumOfVoltageSourcesInLoop -= voltageOfABranch;
                        //second branch
                        resistanceOfABranch = currentLoop.at(1).getResistance();
                        voltageOfABranch = currentLoop.at(1).getVoltageFromVoltageSources();
                        indexOfABranchInBranchesVector = getIndexOfABranchInBranches(currentLoop.at(1));
                        currentEquation.at(indexOfABranchInBranchesVector) = resistanceOfABranch * (-1);
                        sumOfVoltageSourcesInLoop += voltageOfABranch;
                    } else {
                        //first branch
                        resistanceOfABranch = currentLoop.at(0).getResistance();
                        voltageOfABranch = currentLoop.at(0).getVoltageFromVoltageSources();
                        currentEquation.at(indexOfABranchInBranchesVector) = resistanceOfABranch;
                        sumOfVoltageSourcesInLoop -= voltageOfABranch;
                        //second branch
                        resistanceOfABranch = currentLoop.at(1).getResistance();
                        voltageOfABranch = currentLoop.at(1).getVoltageFromVoltageSources();
                        indexOfABranchInBranchesVector = getIndexOfABranchInBranches(currentLoop.at(1));
                        currentEquation.at(indexOfABranchInBranchesVector) = resistanceOfABranch;
                        sumOfVoltageSourcesInLoop -= voltageOfABranch;
                    }
                    break;
                }
                resistanceOfABranch = currentLoop.at(j).getResistance();
                voltageOfABranch = currentLoop.at(j).getVoltageFromVoltageSources();
                if (commonNodeOfBranches == currentLoop.at(j).getFirstNode()) {
                    currentEquation.at(indexOfABranchInBranchesVector) = resistanceOfABranch;
                    sumOfVoltageSourcesInLoop -= voltageOfABranch;
                } else {
                    currentEquation.at(indexOfABranchInBranchesVector) = resistanceOfABranch * (-1);
                    sumOfVoltageSourcesInLoop += voltageOfABranch;
                }
            }
            currentEquation.push_back(sumOfVoltageSourcesInLoop);
            matrixSecondKirchoffRule.push_back(currentEquation);
        }

    vector<double> equationForCurrentSources;
    bool isThereCurrentSource = false;
    for(auto b:branches){
        equationForCurrentSources = vector<double> (getNumberOfBranches(),0.0);
        isThereCurrentSource=false;
        if(b.hasCurrentSources()) {
            isThereCurrentSource = true;
            equationForCurrentSources.at(5)=1;

            double current=0;
            for(auto c : b.getCurrentSources()) {
                current=c.getCurrent()*(-1);
                break;
            }

            equationForCurrentSources.push_back(current);
        }
        if(isThereCurrentSource) matrixSecondKirchoffRule.push_back(equationForCurrentSources);
    }

    return matrixSecondKirchoffRule;
}

vector<double> Circuit::measureCurrentsOfACircuit(){
    vector<double> currentsInTheCircuit = {};
    if(getNumberOfBranches()==1){
        if(getBranches().at(0).hasCurrentSources()){
            currentsInTheCircuit.push_back(getBranches().at(0).getCurrentFromCurrentSources());
            return currentsInTheCircuit;
        }
        else{
            Branch b = getBranches().at(0);
            double current;
            current = b.getVoltageFromVoltageSources()/b.getResistance();
            currentsInTheCircuit.push_back(current);
            return currentsInTheCircuit;
        }
    }
    vector<vector<int>> firstKirchoffsLawMatrix = firstKirchhoffsLaw();
    vector<vector<double>> secondKirchoffsLawMatrix = secondKirchoffsLaw();
    firstKirchoffsLawMatrix.erase(firstKirchoffsLawMatrix.begin()+firstKirchoffsLawMatrix.size()-1);
    int numberOfEquations = firstKirchoffsLawMatrix.size() + secondKirchoffsLawMatrix.size();






    vector<vector<double>> equationMatrix;

    for(auto i : secondKirchoffsLawMatrix){
        vector<double> vectorRow;
        vectorRow.clear();
        for(auto j : i){
            if(j == i.at(i.size()-1))break;
            else vectorRow.push_back(j);
        }
        equationMatrix.push_back(vectorRow);
    }

   for(auto i : firstKirchoffsLawMatrix){
       vector<double> vectorRow;
       vectorRow.clear();
       for(auto j : i){
           vectorRow.push_back((double) j);
       }
       equationMatrix.push_back(vectorRow);
   }


   CMatrix finalMatrix("finalMatrixOfEquations", equationMatrix.size(), getNumberOfBranches());

   for(int i = 0; i < equationMatrix.size(); i++){
       for(int j = 0; j < equationMatrix.at(i).size(); j++){
           finalMatrix.m_pData[i][j] = equationMatrix.at(i).at(j);
       }
   }

   CMatrix sourcesMatrix("sourcesMatrix", getNumberOfBranches(), 1);
    for(int i = 0; i < secondKirchoffsLawMatrix.size(); i++){
            sourcesMatrix.m_pData[i][0] = secondKirchoffsLawMatrix.at(i).at(secondKirchoffsLawMatrix.at(i).size()-1) * (-1); // EQUATIONS * CURRENT + SOURCES = 0, *(-1) to shift SOURCES to right side
    }


   CMatrix currentMatrix("currentMatrix",getNumberOfBranches(),1);

   finalMatrix = finalMatrix.Inverse();
   currentMatrix = finalMatrix * sourcesMatrix;



   for(int i = 0; i < getNumberOfBranches(); i++){
       currentsInTheCircuit.push_back(currentMatrix.m_pData[i][0]);
   }

   //set currents
   for(int i = 0; i < getNumberOfBranches(); i++){
       branches.at(i).setCurrent(currentsInTheCircuit.at(i));
   }

    return currentsInTheCircuit;
    
}

int Circuit::getAvailableBranchId() {
    std::set<int> ids;
    for (auto b : branches)
        ids.insert(b.getId());
    for(int i = 1; ; i++) {
        if(ids.find(i) == ids.end())
            return i;
    }
}


/*int main() {
    Node a(1);
    Node b(2);
    Node c(3);
    Node d(4);
    Node e(5);
    Node f(6);
    Node g(7);


    VoltageSource v1(1, 5);
    VoltageSource v2(2, 10);
    VoltageSource v3(3, 10, 0, false);

    Branch B1 = Branch(1, a, b);
    B1.addResistor(2000);
    B1.addVoltageSource(v1);


    Branch B2 = Branch(2, b, c);
    B2.addResistor(1000);
    B2.addVoltageSource(v1);

    Branch B3 = Branch(3, b, d);
    B3.addVoltageSource(v2);

    Branch B4 = Branch(4, d, c);
    B4.addResistor(1000);

    Branch B5 = Branch(5, c, a);

    Branch B6 = Branch(6, d, a);
    //B6.addVoltageSource(v3);
   // B6.addResistor(1000);
    CurrentSource C1 = CurrentSource(1,0.5);
    B6.addCurrentSource(C1);

    vector<Branch> grane = {B1};
    Circuit krug1;
    krug1.setBranches(grane);

    //TEST ZA KONTURU
    vector<Branch> stablo = krug1.getMinimumSpanningTree();
    std::cout << "Stablo je";
    for (int i = 0; i < stablo.size(); i++) {
        std::cout << stablo[i] << ", ";
    }
    //TEST SLOBODNIH GRANA
    vector<Branch> slobodnaGrana = krug1.getCoTree();
    std::cout << std::endl << "Slobodne grane: ";
    for (int i = 0; i < slobodnaGrana.size(); i++) {
        std::cout << slobodnaGrana[i] << ", ";
    }
    //TEST KONTURA
    vector<vector<Branch>> konture = krug1.getLoops();
    std::cout << std::endl << "Konture :";
    for (int i = 0; i < konture.size(); i++) {
        std::cout << std::endl;
        for (int j = 0; j < konture[i].size(); j++) {
            std::cout << konture[i][j] << " ";
        }
    }
    //TEST CVOROVA
    std::set<Node> cvorovi = krug1.getNodes();
    std::cout << std::endl;
    for (auto c:cvorovi) {
        std::cout << c << ", ";
    }
    std::cout << std::endl;
    //TEST 1 KZ
    std::cout << "Matrica za prvi KZ:" << std::endl;
    vector<vector<int>> matricaStruja = krug1.firstKirchhoffsLaw();
    int x = 0;
    for (auto i:matricaStruja) {
        for (auto j:i)
            std::cout << j << " ";
        std::cout << std::endl;
    }
    //Test 2 KZ

    vector<vector<double>> matrixSecondKirchoffsLaw = krug1.secondKirchoffsLaw();
    std::cout << std::endl;
    for (auto i:matrixSecondKirchoffsLaw) {
        for (auto j:i)
            std::cout << j << " ";
        std::cout << std::endl;
    }
}

  int main() {
     Circuit c;
     char choice;
     bool breakLoop = false;
     int id = 0;
     int node1, node2;
     double resistance;
     double voltage;
     double current;
     while(true) {
         std::cout<< "Enter R for resistor, C for current source, E for voltage source, A for ampermeter, or V for voltmeter (X for exit):\n";
         std::cin >> choice;
         choice = toupper(choice);
         switch (choice) {
             case 'R':
                 std::cout << "Enter resistance:\n";
                 std::cin >> resistance;
                 std::cout << "Enter nodes (two integers):\n";
                 std::cin >> node1 >> node2;
                 c.addResistorToCircuit(Resistor(resistance, id++), node1, node2);
                 break;
             case 'C':
                 std::cout << "Enter current:\n";
                 std::cin >> current;
                 std::cout << "Enter nodes (two integers):\n";
                 std::cin >> node1 >> node2;
                 std::cout << "Enter internal resistance (-1 for ideal):\n";
                 std::cin >> resistance;
                 c.addCurrentSourceToCircuit(CurrentSource(id++, current, resistance), node1, node2);
                 break;
             case 'E':
                 std::cout << "Enter voltage:\n";
                 std::cin >> voltage;
                 std::cout << "Enter nodes (two integers):\n";
                 std::cin >> node1 >> node2;
                 std::cout << "Enter internal resistance (0 for ideal):\n";
                 std::cin >> resistance;
                 c.addVoltageSourceToCircuit(VoltageSource(id++, voltage, resistance), node1, node2);
                 break;
             case 'A':
                 std::cout << "Enter internal resistance (0 for ideal):\n";
                 std::cin >> resistance;
                 std::cout << "Enter nodes (two integers):\n";
                 std::cin >> node1 >> node2;
                 c.addAmpermeterToCircuit(Ampermeter(id++, resistance), node1, node2);
                 break;
             case 'V':
                 std::cout << "Enter internal resistance (-1 for ideal):\n";
                 std::cin >> resistance;
                 std::cout << "Enter nodes (two integers):\n";
                 std::cin >> node1 >> node2;
                 c.addVoltmeterToCircuit(Voltmeter(id++, resistance), node1, node2);
                 break;
             case 'X':
                 breakLoop = true;
                 break;
             default :
                 std::cout << "Wrong letter!\n";
         }
         if (breakLoop) break;
     }
     c.removeObsoleteBranches();
     std::cout<<c;
  }*/
