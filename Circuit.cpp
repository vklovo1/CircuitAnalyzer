//
// Created by 2570p on 24.03.2020..
//

#include "Circuit.h"

Circuit::Circuit(const vector<Branch> &branches) : branches(branches) {}

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
    for(int i = 0; i < branches.size(); i++) {
        if(branches.at(i).getName() == branch.getName()) {
            branches.erase(branches.begin() + i);
            break;
        }
    }
}

vector<Branch> Circuit::getBranchesContainingNode(Node node) {
    vector<Branch> branches_containing_node;
    for(int i = 0; i < branches.size(); i++) {
        if(branches[i].getNodes().first.getName() == node.getName() ||
            branches[i].getNodes().second.getName() == node.getName())
            branches_containing_node.push_back(branches[i]);
    }
    return branches_containing_node;
}

int Circuit::getNumberOfBranchesFromNode(Node node) {
    int numberOfBranches = 0;

    for(Branch b : branches)
        if(b.getFirstNode().getName() == node.getName() || b.getSecondNode().getName() == node.getName())
            numberOfBranches++;

    return numberOfBranches;
}

void Circuit::addComponent(Component component, string nodeNameFirst = "", string nodeNameSecond = "") {
    if(branches.size() == 0) {
        Node node1 = Node("N1");
        Node node2 = Node("N2");
        Branch branch1 = Branch("B1", std::pair<Node, Node>(node1, node2), std::vector<Component>{component});
        addBranch(branch1);
    } else {
        int branchContainingNodeIndex = getIndexOfBranchOfNode(Node(nodeNameFirst));
        if(branchContainingNodeIndex != -1) { //case1
        } else {
            branchContainingNodeIndex = getIndexOfBranchOfNode(Node(nodeNameSecond));
            if(branchContainingNodeIndex != -1) { //case2

            } else { //case3

            }
        }
    }
}

int Circuit::getNumberOfBranches() {
    return branches.size();
}

int Circuit::getNumberOfNodes() {
    return 0;
}

vector<Branch>  Circuit::MinimumSpanningTree(Node starting_node){
    Node current_node=starting_node;
    vector<Branch> tree_branches;
    std::stack<Node> order_of_visited_nodes; // redom ubacujem cvorove koje uzimam LIFO
    while(true){
        vector<Branch> branches_containing_node = getBranchesContainingNode(current_node);
        if(current_node.getName()==starting_node.getName() && starting_node.isVisited()) break; //Ako se vrati nazad do pocetnog cvora, kraj
        current_node.setVisited(true);
        for(int i = 0; i < branches_containing_node.size(); i++){
            if(branches_containing_node.at(i).getFirstNode().getName()==current_node.getName() && !branches_containing_node.at(i).getSecondNode().isVisited()){
                tree_branches.push_back(branches_containing_node.at(i));
                order_of_visited_nodes.push(current_node);
                current_node=branches_containing_node.at(i).getSecondNode();
                break;
            }
            else if(branches_containing_node.at(i).getSecondNode().getName()==current_node.getName() && !branches_containing_node.at(i).getFirstNode().isVisited()){
                    tree_branches.push_back(branches_containing_node.at(i)); //MOZDA CE BIT DUPLIH GRANA
                    order_of_visited_nodes.push(current_node);
                    current_node=branches_containing_node.at(i).getFirstNode();
                    break;
            }
            else{
                if(i==branches_containing_node.size()-1) {
                    current_node=order_of_visited_nodes.top();
                    order_of_visited_nodes.pop();
                }
            }
        }


    }



};



