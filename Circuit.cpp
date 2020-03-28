//
// Created by 2570p on 24.03.2020..
//

#include "Circuit.h"

#include <utility>

Circuit::Circuit(const vector<Branch> &branches)  {

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
        if (branches.at(i).getName() == branch.getName()) {
            branches.erase(branches.begin() + i);
            break;
        }
    }
}

vector<Branch> Circuit::getBranchesContainingNode(Node node) {
    vector<Branch> branchesContainingNode;
    for (int i = 0; i < branches.size(); i++) {
        if (branches[i].getNodes().first.getName() == node.getName() ||
            branches[i].getNodes().second.getName() == node.getName())
            branchesContainingNode.push_back(branches[i]);
    }
    return branchesContainingNode;
}

int Circuit::getNumberOfBranchesFromNode(Node node) {
    int numberOfBranches = 0;

    for (Branch b : branches)
        if (b.getFirstNode().getName() == node.getName() || b.getSecondNode().getName() == node.getName())
            numberOfBranches++;

    return numberOfBranches;
}

void Circuit::addComponent(std::shared_ptr<Component> component, string nodeNameFirst = "", string nodeNameSecond = "") {
    Branch newBranch("B" + std::to_string(getNumberOfBranches()), std::pair<Node, Node>(Node(nodeNameFirst), Node(nodeNameSecond)),
            vector<std::shared_ptr<Component>>{std::move(component)});
    addBranch(newBranch);
}

void Circuit::simplifyCircuit() {
    for(int i = 0; i < branches.size(); i++) {
        Node nodeToGetRidOf("");
        if(getNumberOfBranchesFromNode(branches[i].getFirstNode()) < 3)
            nodeToGetRidOf = branches[i].getFirstNode();
        else if(getNumberOfBranchesFromNode(branches[i].getSecondNode()) < 3)
            nodeToGetRidOf = branches[i].getSecondNode();
        else continue;

        auto branchesContainingNode = getBranchesContainingNode(nodeToGetRidOf);
        auto firstBranch = branchesContainingNode[0];
        auto secondBranch = branchesContainingNode[1];
        Branch newBranch();
        Node firstNode("");
        Node secondNode("");

        bool nodeToDeleteWasFirstInTheFirstBranch;
        bool nodeToDeleteWasFirstInTheSecondBranch;

        if(firstBranch.getFirstNode().getName() == nodeToGetRidOf.getName()) {
            firstNode.setName(firstBranch.getSecondNode().getName());
            nodeToDeleteWasFirstInTheFirstBranch = true;
        } else {
            firstNode.setName(firstBranch.getFirstNode().getName());
            nodeToDeleteWasFirstInTheFirstBranch = false;
        }

        if(secondBranch.getFirstNode().getName() == nodeToGetRidOf.getName()) {
            secondNode.setName(secondBranch.getSecondNode().getName());
            nodeToDeleteWasFirstInTheSecondBranch = true;
        } else {
            secondNode.setName(secondBranch.getFirstNode().getName());
            nodeToDeleteWasFirstInTheSecondBranch = false;
        }

        vector<std::shared_ptr<Component>> newBranchComponents(firstBranch.getComponents().size() + secondBranch.getComponents().size());

        if(!nodeToDeleteWasFirstInTheFirstBranch) {
            for (auto c : firstBranch.getComponents())
                newBranchComponents.push_back(c);
        } else {
            for(auto c : firstBranch.getComponents()) {
                if(!)
            }
        }
    }
}

void Circuit::drawWire(string nodeNameFirst, string nodeNameSecond) {
    int firstNodeIndex = std::stoi(nodeNameFirst.substr(1, string::npos));
    int secondNodeIndex = std::stoi(nodeNameSecond.substr(1, string::npos));

    string lesserIndexNodeName;
    if(firstNodeIndex < secondNodeIndex) lesserIndexNodeName = nodeNameFirst;
    else lesserIndexNodeName = nodeNameSecond;

    for(Branch b : branches) {
        if(b.getFirstNode().getName() == nodeNameSecond)
            b.setFirstNode(Node(lesserIndexNodeName));
        else if(b.getSecondNode().getName() == nodeNameSecond)
            b.setSecondNode(Node(lesserIndexNodeName));
    }
}

int Circuit::getNumberOfBranches() {
    return branches.size();
}

int Circuit::getNumberOfNodes() {
    return 0;
}

bool Circuit::isVisited(Node nodeToCheck, vector<Node> visited_nodes) {
    for (int i = 0; i < visited_nodes.size(); i++) {
        if (nodeToCheck.getName() == visited_nodes[i].getName())
            return true;
    }
    return false;
}

vector<Branch> Circuit::getMinimumSpanningTree(Node startingNode) {
    Node currentNode = startingNode;
    vector<Node> visitedNodes;
    vector<Branch> treeBranches;
    std::stack<Node> orderOfVisitedNodes; // redom ubacujem cvorove koje uzimam LIFO
    while (true) {
        vector<Branch> branchesContainingNode = this->getBranchesContainingNode(currentNode);
        if (currentNode.getName() == startingNode.getName() && isVisited(startingNode, visitedNodes))
            break; //Ako se vrati nazad do pocetnog cvora, kraj (MOZDA OVO NE VALJA)
        visitedNodes.push_back(currentNode);
        for (int i = 0; i < branchesContainingNode.size(); i++) {
            if (branchesContainingNode.at(i).getFirstNode().getName() == currentNode.getName() &&
                !isVisited(branchesContainingNode.at(i).getSecondNode(), visitedNodes)) {
                treeBranches.push_back(branchesContainingNode.at(i));
                orderOfVisitedNodes.push(currentNode);
                currentNode = branchesContainingNode.at(i).getSecondNode();
                break;
            } else if (branchesContainingNode.at(i).getSecondNode().getName() == currentNode.getName() &&
                       !isVisited(branchesContainingNode.at(i).getFirstNode(), visitedNodes)) {
                treeBranches.push_back(branchesContainingNode.at(i)); //MOZDA CE BIT DUPLIH GRANA
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

int main() {
    Node a("A");
    Node b("B");
    Node c("C");
    Node d("D");
    Node e("E");
    Node f("F");
    Node g("G");
    Branch B1 = Branch("1", std::pair<Node, Node>(a, f), std::vector<std::shared_ptr<Component>>{});
    Branch B2 = Branch("2", std::pair<Node, Node>(f, c), std::vector<std::shared_ptr<Component>>{});
    Branch B3 = Branch("3", std::pair<Node, Node>(c, e), std::vector<std::shared_ptr<Component>>{});
    Branch B4 = Branch("4", std::pair<Node, Node>(e, g), std::vector<std::shared_ptr<Component>>{});
    Branch B5 = Branch("5", std::pair<Node, Node>(g, d), std::vector<std::shared_ptr<Component>>{});
    Branch B6 = Branch("6", std::pair<Node, Node>(d, a), std::vector<std::shared_ptr<Component>>{});
    Branch B7 = Branch("7", std::pair<Node, Node>(a, b), std::vector<std::shared_ptr<Component>>{});
    Branch B8 = Branch("8", std::pair<Node, Node>(b, d), std::vector<std::shared_ptr<Component>>{});
    Branch B9 = Branch("9", std::pair<Node, Node>(d, e), std::vector<std::shared_ptr<Component>>{});
    Branch B10 = Branch("10", std::pair<Node, Node>(e, b), std::vector<std::shared_ptr<Component>>{});
    Branch B11 = Branch("11", std::pair<Node, Node>(b, c), std::vector<std::shared_ptr<Component>>{});
    Branch B12 = Branch("12", std::pair<Node, Node>(b, f), std::vector<std::shared_ptr<Component>>{});
    const vector<Branch> grane = {B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12};
    Circuit krug1;
    krug1.setBranches(grane);
    vector<Branch> stablo = krug1.getMinimumSpanningTree(a);
    std::cout << "Stablo je";
    for (int i = 0; i < stablo.size(); i++) {
        std::cout << stablo[i].getName() << ", ";
    }
    std::cout << " broj " << stablo.size();

}



