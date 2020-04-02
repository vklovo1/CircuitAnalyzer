//
// Created by 2570p on 24.03.2020..
//

#ifndef CIRCUITANALYZER_CIRCUIT_H
#define CIRCUITANALYZER_CIRCUIT_H


#include <vector>
#include "main.cpp"
#include <stack>
#include <memory>
#include <utility>
#include <set>

using std::vector;

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

    vector<Branch> getBranchesContainingNode(Node node);

    bool isNodeInNodeVector(const Node &nodeToCheck, const vector<Node> &visitedNodes);

    vector<Branch> getMinimumSpanningTree();

    vector<vector<Branch>> getLoops();

    vector<Branch> getCoTree();

    bool isBranchInTheTree(Branch branchToCheck);

    vector<vector<int>> firstKirchhoffRule();

    static bool doesNodeContainBranch(Branch branchToCheck, vector<Branch> branchesContainingNode);

    std::set<Node> getNodes();

    void removeObsoleteBranches();

    void drawWire(int firstNodeID, int secondNodeID);

    friend std::ostream& operator<<(std::ostream& os, const Circuit &C);
};


#endif //CIRCUITANALYZER_CIRCUIT_H
