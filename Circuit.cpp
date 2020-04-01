//
// Created by 2570p on 24.03.2020..
//

#include "Circuit.h"


Circuit::Circuit(const vector<Branch> &branches)  {
    this->branches = branches;
}

Circuit::Circuit() {
    numberOfNodes = 0;
    branches = std::vector<Branch>();
}

const vector<Branch> &Circuit::getBranches() const {
    return branches;
}

void Circuit::setBranches(const vector<Branch> &branches) {
    Circuit::branches = branches;
}

void Circuit::addBranch(const Branch &branch) {
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
    for (int i = 0; i < branches.size(); i++) {
        if (branches[i].getFirstNode() == node || branches[i].getSecondNode() == node)
            branchesContainingNode.push_back(branches[i]);
    }
    return branchesContainingNode;
}

int Circuit::getNumberOfBranchesFromNode(Node node) {
    int numberOfBranches = 0;

    for (Branch b : branches)
        if (b.getFirstNode() == node || b.getSecondNode() == node)
            numberOfBranches++;

    return numberOfBranches;
}

void Circuit::simplifyCircuit() {
    for(int i = 0; i < branches.size(); i++) {
        Node nodeToGetRidOf;
        if(getNumberOfBranchesFromNode(branches[i].getFirstNode()) < 3)
            nodeToGetRidOf = branches[i].getFirstNode();
        else if(getNumberOfBranchesFromNode(branches[i].getSecondNode()) < 3)
            nodeToGetRidOf = branches[i].getSecondNode();
        else continue;

        auto branchesContainingNode = getBranchesContainingNode(nodeToGetRidOf);
        auto firstBranch = branchesContainingNode[0];
        auto secondBranch = branchesContainingNode[1];
        Branch newBranch;
        Node firstNode;
        Node secondNode;

        bool nodeToDeleteWasFirstInTheFirstBranch;
        bool nodeToDeleteWasFirstInTheSecondBranch;

        if(firstBranch.getFirstNode() == nodeToGetRidOf) {
            firstNode = firstBranch.getSecondNode();
            nodeToDeleteWasFirstInTheFirstBranch = true;
        } else {
            firstNode = firstBranch.getFirstNode();
            nodeToDeleteWasFirstInTheFirstBranch = false;
        }

        if(secondBranch.getFirstNode() == nodeToGetRidOf) {
            secondNode = secondBranch.getSecondNode();
            nodeToDeleteWasFirstInTheSecondBranch = true;
        } else {
            secondNode = secondBranch.getFirstNode();
            nodeToDeleteWasFirstInTheSecondBranch = false;
        }

        /*vector<std::shared_ptr<Component>> newBranchComponents(firstBranch.getComponents().size() + secondBranch.getComponents().size());

        if(!nodeToDeleteWasFirstInTheFirstBranch) {
            for (auto c : firstBranch.getComponents())
                newBranchComponents.push_back(c);
        } else {
            for(auto c : firstBranch.getComponents()) {
                if(!)
            }
        }*/
    }
}

void Circuit::drawWire(int firstNodeID, int secondNodeID) {
    int lesserNodeIndex, higherNodeIndex;

    if(firstNodeID < secondNodeID) {
        lesserNodeIndex = firstNodeID;
        higherNodeIndex = secondNodeID;
    } else {
        lesserNodeIndex = secondNodeID;
        higherNodeIndex = firstNodeID;
    }

    for(Branch b : branches) {
        if(b.getFirstNode().getId() == higherNodeIndex)
            b.setFirstNode(Node(lesserNodeIndex));
        else if(b.getSecondNode().getId() == higherNodeIndex)
            b.setSecondNode(Node(lesserNodeIndex));
    }
}

int Circuit::getNumberOfBranches() {
    return branches.size();
}

int Circuit::getNumberOfNodes() {
    std::set<int> distinctNodes;
    for(Branch b : branches) {
        distinctNodes.insert(b.getFirstNode().getId());
        distinctNodes.insert(b.getSecondNode().getId());
    }
}

std::set<Node> Circuit::getNodes() {
    std::set<Node> distinctNodes;
    for(Branch b : branches) {
        distinctNodes.insert(b.getFirstNode().getId());
        distinctNodes.insert(b.getSecondNode().getId());
    }
    return distinctNodes;
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
    for (int i = 0; i < minimumSpanningTree.size(); i++) {
        if (branchToCheck == minimumSpanningTree[i])return true;
    }
    return false;
}

vector<Branch> Circuit::getCoTree() {
    vector<Branch> minimumSpanningTree = getMinimumSpanningTree();
    vector<Branch> freeBranches;
    bool branchInTheTree = false;
    for (int i = 0; i < getNumberOfBranches(); i++) {
        if (!isBranchInTheTree(branches[i]))freeBranches.push_back(branches[i]);
    }
    return freeBranches;
}


vector<vector<Branch>> Circuit::getLoops() {
    vector<Branch> currentLoop;
    vector<Branch> freeBranches = getCoTree();
    vector<Branch> branchesContainingNode;
    vector<Node> visitedNodes;
    bool goBack = false;
    Node endingNode = freeBranches[0].getSecondNode();
    Node currentNode = freeBranches[0].getFirstNode();;
    std::stack<Node> orderOfVisitedNodes; // redom ubacujem cvorove koje uzimam LIFO
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
                currentLoop.erase(currentLoop.begin() + currentLoop.size() - 1);
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
            loops.push_back(currentLoop);
            currentLoop.clear();
        }
    }
    return loops;
}

bool Circuit::doesNodeContainBranch(Branch branchToCheck, vector<Branch> branchesContainingNode) {
    for (auto b:branchesContainingNode) {
        if (b == branchToCheck) return true;
    }
    return false;
}

/*vector<Node> Circuit::getNodes() {
    vector<Node> nodesInTheCircuit;
    vector<Node> visitedNodes;
    Node currentNode = branches[0].getFirstNode();
    nodesInTheCircuit.push_back(currentNode);
    visitedNodes.push_back(currentNode);
    for (auto b:branches) {
        if (!isNodeInNodeVector(b.getFirstNode(), visitedNodes)) {
            currentNode = b.getFirstNode();
            nodesInTheCircuit.push_back(currentNode);
            visitedNodes.push_back(currentNode);
        } else if (!isNodeInNodeVector(b.getSecondNode(), visitedNodes)) {
            currentNode = b.getSecondNode();
            nodesInTheCircuit.push_back(currentNode);
            visitedNodes.push_back(currentNode);
        }
    }
    return nodesInTheCircuit;
}*/

vector<vector<int>> Circuit::firstKirchhoffRule() {
    int numberOfNodes = getNumberOfNodes();
    vector<vector<int>> matrixOfCurrents;
    vector<Branch> branchesContainingNode;
    vector<int> currentEquation;
    vector<Node> visitedNodes;
    vector<Node> nodesInTheCircuit = getNodes();
    for (auto n:nodesInTheCircuit) {
        currentEquation.clear();
        branchesContainingNode = getBranchesContainingNode(n);
        for (auto b:branches)
            currentEquation.push_back(doesNodeContainBranch(b, branchesContainingNode) ? 1 : 0);
        matrixOfCurrents.push_back(currentEquation);
    }
    return matrixOfCurrents;
}

    int main() {
       /* Node a("A");
        Node b("B");
        Node c("C");
        Node d("D");
        Node e("E");
        Node f("F");
        Node g("G");
        Branch B1 = Branch("1", std::pair<Node, Node>(a, f), std::vector<Component>{});
        Branch B2 = Branch("2", std::pair<Node, Node>(f, c), std::vector<Component>{});
        Branch B3 = Branch("3", std::pair<Node, Node>(c, e), std::vector<Component>{});
        Branch B4 = Branch("4", std::pair<Node, Node>(e, g), std::vector<Component>{});
        Branch B5 = Branch("5", std::pair<Node, Node>(g, d), std::vector<Component>{});
        Branch B6 = Branch("6", std::pair<Node, Node>(d, a), std::vector<Component>{});
        Branch B7 = Branch("7", std::pair<Node, Node>(a, b), std::vector<Component>{});
        Branch B8 = Branch("8", std::pair<Node, Node>(b, d), std::vector<Component>{});
        Branch B9 = Branch("9", std::pair<Node, Node>(d, e), std::vector<Component>{});
        Branch B10 = Branch("10", std::pair<Node, Node>(e, b), std::vector<Component>{});
        Branch B11 = Branch("11", std::pair<Node, Node>(b, c), std::vector<Component>{});
        Branch B12 = Branch("12", std::pair<Node, Node>(b, f), std::vector<Component>{});
        Branch B13 = Branch("13", std::pair<Node, Node>(a, b), std::vector<Component>{});
        Branch B14 = Branch("14", std::pair<Node, Node>(b, a), std::vector<Component>{});
        Branch B15 = Branch("15", std::pair<Node, Node>(a, b), std::vector<Component>{});
        const vector<Branch> grane = {B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12};
        Circuit krug1;
        krug1.setBranches(grane);
        //TEST ZA KONTURU
        vector<Branch> stablo = krug1.getMinimumSpanningTree();
        std::cout << "Stablo je";
        for (int i = 0; i < stablo.size(); i++) {
            std::cout << stablo[i].getName() << ", ";
        }
        //TEST SLOBODNIH GRANA
        vector<Branch> slobodnaGrana = krug1.getCoTree();
        std::cout << std::endl << "Slobodne grane: ";
        for (int i = 0; i < slobodnaGrana.size(); i++) {
            std::cout << slobodnaGrana[i].getName() << ", ";
        }
        //TEST KONTURA
        vector<vector<Branch>> konture = krug1.getLoops();
        std::cout << std::endl << "Konture :";
        for (int i = 0; i < konture.size(); i++) {
            std::cout << std::endl;
            for (int j = 0; j < konture[i].size(); j++) {
                std::cout << konture[i][j].getName() << " ";
            }
        }
        //TEST CVOROVA
        vector<Node> cvorovi = krug1.getNodes();
        std::cout << std::endl;
        for (auto c:cvorovi) {
            std::cout << c.getName() << ", ";
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
        }*/
    }