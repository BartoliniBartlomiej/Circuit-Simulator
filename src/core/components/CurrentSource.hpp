#pragma once
#include "../Component.hpp"

// Ideal Current Source
// current flows from nodeB to nodeA (through the external circuit: A → B).
//
// nodeA ──→── circuit ──→── nodeB
// I (forced)
//
// stamp:
// I[a] -= I_src (current "flows out" of node A into the matrix)
// I[b] += I_src (current "flows into" node B).

class CurrentSource : public Component {
public:
    CurrentSource(const std::string& name, Node* nodeA, Node* nodeB, double current)
        : Component(name, nodeA, nodeB), current_(current)
    {}

    void stamp(Matrix& /*G*/, Vector& I) const override {
        if (!nodeA->isGround()) I[nodeA->id - 1] += current_;
        if (!nodeB->isGround()) I[nodeB->id - 1] -= current_;
    }

    double getCurrent() const override { return current_; }

    double current() const { return current_; }
    void setCurrent(double i) { current_ = i; }

private:
    double current_; // [A]
};