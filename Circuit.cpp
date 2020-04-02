//
// Created by 2570p on 24.03.2020..
//

#include <algorithm>
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

//adding a component creates a new branch for it
//this method removes obsolete branches that are actually in series

void Circuit::removeObsoleteBranches() {
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

        bool firstBranchOrientationChanged;
        bool secondBranchOrientationChanged;

        if(firstBranch.getFirstNode() == nodeToGetRidOf) {
            firstNode = firstBranch.getSecondNode();
            firstBranchOrientationChanged = true;
        } else {
            firstNode = firstBranch.getFirstNode();
            firstBranchOrientationChanged = false;
        }

        if(secondBranch.getFirstNode() == nodeToGetRidOf) {
            secondNode = secondBranch.getSecondNode();
            secondBranchOrientationChanged = false;
        } else {
            secondNode = secondBranch.getFirstNode();
            secondBranchOrientationChanged = true;
        }

        if(!firstBranchOrientationChanged)
            newBranch += firstBranch;
        else {
            std::for_each(firstBranch.getVoltageSources().begin(), firstBranch.getVoltageSources().end(),
                    [](VoltageSource &c) -> void { c.toggleOrientation() ; });
            std::for_each(firstBranch.getCurrentSources().begin(), firstBranch.getCurrentSources().end(),
                    [](CurrentSource &c) -> void { c.toggleOrientation(); });
            newBranch += firstBranch;
        }

        if(!secondBranchOrientationChanged)
            newBranch += secondBranch;
        else {
            std::for_each(secondBranch.getVoltageSources().begin(), secondBranch.getVoltageSources().end(),
                    [](VoltageSource &c) -> void { c.toggleOrientation() ; });
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

vector<vector<int>> Circuit::firstKirchhoffRule() {
    int numberOfNodes = getNumberOfNodes();
    vector<vector<int>> matrixOfCurrents;
    vector<Branch> branchesContainingNode;
    vector<int> currentEquation;
    vector<Node> visitedNodes;
    std::set<Node> nodesInTheCircuit = getNodes();
    for (auto n : nodesInTheCircuit) {
        currentEquation.clear();
        branchesContainingNode = getBranchesContainingNode(n);
        for (auto b : branches)
            currentEquation.push_back(doesNodeContainBranch(b, branchesContainingNode) ? 1 : 0);
        matrixOfCurrents.push_back(currentEquation);
    }
    return matrixOfCurrents;
}

std::ostream &operator<<(std::ostream &os, const Circuit &C) {

    return os;
}

int main() {
       Circuit c;
       Node n1(1);
       Node n2(2);
       Branch b1(1, n1, n2);
       Node n3(3);
       Branch b2(2, n2, n3);
       Branch b3(3, n3, n1);

       c.addBranch(b1); c.addBranch(b2); c.addBranch(b3);
       c.removeObsoleteBranches();
       std::cout<<c.getNumberOfBranches() << " " << c.getBranches().at(0);
    }