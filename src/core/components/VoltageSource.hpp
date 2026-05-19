#pragma once
#include "../Component.hpp"

// Ideal voltage source between nodeA (+) and nodeB (-).
//
// Requires extended MNA: the Circuit must reserve an additional
// row/column for each VoltageSource to represent the branch current
// (branchCurrentIndex_).
// Circuit::solve() calls assignBranchIndex() before matrix assembly.
//
// Extended matrix (n nodes + k voltage sources):
//
//   [ G   A ] [ V ]   [ I  ]
//   [ Aᵀ  0 ] [ J ] = [ Vs ]
//
//   A[a][j] =  1,  A[b][j] = -1  (j = the current index of this branch)
//   Aᵀ[j][a] = 1, Aᵀ[j][b] = -1
//   rhs[n+j] = voltage_

class VoltageSource : public Component {
public:
    VoltageSource(const std::string& name, Node* nodeA, Node* nodeB, double voltage)
        : Component(name, nodeA, nodeB), voltage_(voltage), branchIndex_(-1)
    {}

    void assignBranchIndex(int idx) { branchIndex_ = idx; }
    int  branchIndex() const        { return branchIndex_; }

    void stamp(Matrix& G, Vector& rhs) const override {
        int j = branchIndex_;

        auto idx = [](const Node* n) { return n->id - 1; };

        if (!nodeA->isGround()) {
            G[idx(nodeA)][j] += 1.0;
            G[j][idx(nodeA)] += 1.0;
        }
        if (!nodeB->isGround()) {
            G[idx(nodeB)][j] -= 1.0;
            G[j][idx(nodeB)] -= 1.0;
        }

        // V(a) - V(b) = voltage_
        rhs[j] = voltage_;
    }

    double getCurrent() const override { return branchCurrent_; }
    void   setBranchCurrent(double i)  { branchCurrent_ = i; }

    double voltage() const  { return voltage_; }
    void setVoltage(double v) { voltage_ = v; }

private:
    double voltage_;             // [V]
    int branchIndex_;
    double branchCurrent_ = 0.0; // [A]
    
};