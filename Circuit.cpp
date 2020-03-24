//
// Created by 2570p on 24.03.2020..
//

#include "Circuit.h"

Circuit::Circuit(const vector<Branch> &branches) : branches(branches) {}

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



