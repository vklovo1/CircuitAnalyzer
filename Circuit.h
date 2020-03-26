//
// Created by 2570p on 24.03.2020..
//

#ifndef CIRCUITANALYZER_CIRCUIT_H
#define CIRCUITANALYZER_CIRCUIT_H


#include <vector>
#include "main.cpp"
#include <stack>

class Circuit {


    std::vector<Branch> branches;
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

    void addComponent(Component component, int coordinateX, int coordinateY);

    void addComponent(Component component, string nodeNameFirst, string nodeNameSecond);

    int getNumberOfBranchesFromNode(Node node);



    vector<Branch> getMinimumSpanningTree(Node startingNode);

    vector<Branch> getBranchesContainingNode(Node node);

    bool isVisited(Node nodeToCheck, vector<Node> visited_nodes);
};


#endif //CIRCUITANALYZER_CIRCUIT_H
