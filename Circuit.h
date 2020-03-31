//
// Created by 2570p on 24.03.2020..
//

#ifndef CIRCUITANALYZER_CIRCUIT_H
#define CIRCUITANALYZER_CIRCUIT_H


#include <vector>
#include "main.cpp"
#include <stack>
#include <memory>

class Circuit {
    vector<Branch> branches;
    int numberOfNodes;

public:
    Circuit();

    Circuit(const vector<Branch> &branches);

    const vector<Branch> &getBranches() const;

    void setBranches(const vector<Branch> &branches);

    void addBranch(const Branch &branch);

    void removeBranch(const Branch &branch);

    int getNumberOfBranches();

    int getNumberOfNodes();


    int getNumberOfBranchesFromNode(Node node);

    void addComponent(std::shared_ptr<Component> component, string nodeNameFirst, string nodeNameSecond);

    vector<Branch> getMinimumSpanningTree(Node startingNode);

    vector<Branch> getBranchesContainingNode(Node node);

    bool isVisited(Node nodeToCheck, vector<Node> visited_nodes);

    void simplifyCircuit();

    void drawWire(string nodeNameFirst, string nodeNameSecond);
};


#endif //CIRCUITANALYZER_CIRCUIT_H
