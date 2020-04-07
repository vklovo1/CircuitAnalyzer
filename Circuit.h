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
#include <list>

using std::vector;
using std::list;

class Circuit {
    vector<Branch> branches;
    list<VoltmeterWrapper> voltmeters;
    list<AmpermeterWrapper> ampermeters;

    int numberOfNodes;

public:
    Circuit();

    Circuit(const vector<Branch> &branches);

    const vector<Branch> &getBranches() const;

    void setBranches(const vector<Branch> &branches);

    void addBranch(Branch &branch);

    void removeBranch(const Branch &branch);

    int getNumberOfBranches();

    int getNumberOfNodes();

    int getNumberOfBranchesFromNode(Node node);

    vector<Branch> getBranchesContainingNode(Node node);

    static bool isNodeInNodeVector(const Node &nodeToCheck, const vector<Node> &visitedNodes);

    vector<Branch> getMinimumSpanningTree();

    vector<vector<Branch>> getLoops();

    vector<Branch> getCoTree();

    bool isBranchInTheTree(Branch branchToCheck);

    vector<vector<int>> firstKirchhoffsLaw();

    std::set<Node> getNodes();

    void removeObsoleteBranches();

    void drawWire(int firstNodeID, int secondNodeID);

    friend std::ostream& operator<<(std::ostream& os, const Circuit &C);

    void addResistorToCircuit(const Resistor &r, int firstNodeID, int secondNodeID);

    void addVoltageSourceToCircuit(const VoltageSource &v, int firstNodeID, int secondNodeID);

    void addCurrentSourceToCircuit(CurrentSource V, int firstNodeID, int secondNodeID);

    vector<Branch> &getBranches();

    friend std::ostream &operator<<(std::ostream &os, const Circuit &C);

    static bool isBranchInTheVector(const Branch &branchToCheck, const vector<Branch> &branchesContainingNode);

    void addVoltmeterToCircuit(Voltmeter v, int firstNodeID, int secondID);

    void addAmpermeterToCircuit(Ampermeter a, int firstNodeID, int secondNodeID);

    void removeBranchesWithInfiniteResistance();

    void shortConnectBranchesWithZeroResistance();

    vector<Branch> getBranchesWithoutCurrentSource();

    vector<Branch> getBranchesWithoutCurrentSourceFromVector(vector<Branch> vectorToCheck);

    vector<std::vector<double>> secondKirchoffsLaw();

    bool isBranchInTheLoop(Branch branchToCheck, int IndexOfALoop);

    int getnumberOfLoops();

    Node commonNode(Branch firstBranch, Branch secondBranch);

    int getIndexOfABranchInBranches(Branch branchToCheck);
};


#endif //CIRCUITANALYZER_CIRCUIT_H
