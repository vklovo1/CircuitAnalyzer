//
// Created by 2570p on 24.03.2020..
//

#include <algorithm>
#include "Circuit.h"


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
    for (auto &b : branches) {
        Node nodeToGetRidOf;
        if (getNumberOfBranchesFromNode(b.getFirstNode()) < 3)
            nodeToGetRidOf = b.getFirstNode();
        else if (getNumberOfBranchesFromNode(b.getSecondNode()) < 3)
            nodeToGetRidOf = b.getSecondNode();
        else continue;

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
    Branch newBranch(getNumberOfBranches() + 1, Node(firstNodeID), Node(secondNodeID));
    newBranch.addResistor(r);
    addBranch(newBranch);
}

void Circuit::addVoltageSourceToCircuit(const VoltageSource &v, int firstNodeID, int secondNodeID) {
    Branch newBranch(getNumberOfBranches() + 1, Node(firstNodeID), Node(secondNodeID));
    newBranch.addVoltageSource(v);
    addBranch(newBranch);
}

void Circuit::addCurrentSourceToCircuit(CurrentSource c, int firstNodeID, int secondNodeID) {
    Branch newBranch(getNumberOfBranches() + 1, Node(firstNodeID), Node(secondNodeID));
    addResistorToCircuit(Resistor(c.getInternalResistance()), firstNodeID, secondNodeID);
    c.setIdeal();
    newBranch.addCurrentSource(c);
    addBranch(newBranch);
}

void Circuit::addVoltmeterToCircuit(Voltmeter v, int firstNodeID, int secondNodeID) {
    Branch newBranch(getNumberOfBranches() + 1, Node(firstNodeID), Node(secondNodeID));
    newBranch.addResistor(Resistor(v.getInternalResistance()));
    addBranch(newBranch);

    VoltmeterWrapper voltmeterWrapper(v, Node(firstNodeID), Node(secondNodeID), &newBranch.getResistors().front());
    voltmeters.push_back(voltmeterWrapper);
}

void Circuit::addAmpermeterToCircuit(Ampermeter a, int firstNodeID, int secondNodeID) {
    Branch newBranch(getNumberOfBranches() + 1, Node(firstNodeID), Node(secondNodeID));
    newBranch.addResistor(Resistor(a.getInternalResistance()));
    addBranch(newBranch);

    AmpermeterWrapper ampermeterWrapper(a, newBranch, &newBranch.getResistors().front());
    ampermeters.push_back(ampermeterWrapper);
}

bool Circuit::isNodeInNodeVector(const Node &nodeToCheck, const vector<Node> &visitedNodes) {
    for (int i = 0; i < visitedNodes.size(); i++) {
        if (nodeToCheck == visitedNodes[i])
            return true;
    }
    return false;
}

vector<Branch> Circuit::getMinimumSpanningTree() {
    Node startingNode = branches[0].getFirstNode();
    Node currentNode = startingNode;
    vector<Node> visitedNodes;
    vector<Branch> treeBranches;
    std::stack<Node> orderOfVisitedNodes;
    while (true) {
        vector<Branch> branchesContainingNode = this->getBranchesContainingNode(currentNode);
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

vector<Branch> Circuit::getCoTree() {
    vector<Branch> minimumSpanningTree = getMinimumSpanningTree();
    vector<Branch> freeBranches;
    for (int i = 0; i < getNumberOfBranches(); i++) {
        if (!isBranchInTheTree(branches[i]))
            freeBranches.push_back(branches[i]);
    }
    return freeBranches;
}


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

bool Circuit::nodeContainsBranch(const Branch &branchToCheck, const vector<Branch> &branchesContainingNode) {
    for (auto b : branchesContainingNode) {
        if (b == branchToCheck) return true;
    }
    return false;
}

vector<vector<int>> Circuit::firstKirchhoffRule() {
    int numberOfNodes = getNumberOfNodes();
    vector<vector<int>> matrixOfCurrents;
    vector<Branch> branchesContainingNode;
    vector<int> currentEquation;
    vector<Node> visitedNodes;
    std::set<Node> nodesInTheCircuit = getNodes();
    for (const auto &n : nodesInTheCircuit) {
        currentEquation.clear();
        branchesContainingNode = getBranchesContainingNode(n);
        for (const auto &b : branches)
            currentEquation.push_back(nodeContainsBranch(b, branchesContainingNode) ? 1 : 0);
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

int main() {
    Node a(1);
    Node b(2);
    Node c(3);
    Node d(4);
    Node e(5);
    Node f(6);
    Node g(7);
    Branch B1 = Branch(1, a, f);
    Branch B2 = Branch(2, f, c);
    Branch B3 = Branch(3, c, e);
    Branch B4 = Branch(4, e, g);
    Branch B5 = Branch(5, g, d);
    Branch B6 = Branch(6, d, a);
    Branch B7 = Branch(7, a, b);
    Branch B8 = Branch(8, b, d);
    Branch B9 = Branch(9, d, e);
    Branch B10 = Branch(10, e, b);
    Branch B11 = Branch(11, b, c);
    Branch B12 = Branch(12, b, f);
    Branch B13 = Branch(13, a, b);
    Branch B14 = Branch(14, b, a);
    Branch B15 = Branch(15, a, b);
    const vector<Branch> grane = {B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12};
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
    vector<vector<int>> matricaStruja = krug1.firstKirchhoffRule();
    int x = 0;
    for (auto i:matricaStruja) {
        for (auto j:i)
            std::cout << j << ", ";
        std::cout << std::endl;
    }
}

/*  int main() {
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