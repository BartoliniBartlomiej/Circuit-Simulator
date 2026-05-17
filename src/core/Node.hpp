#pragma once

#include <string>

class Node {
public:
    int id; 
    std::string name;
    double voltage;

    Node(int _id, const std::string& _name = "")
        : id(_id), 
        name(_name.empty() ? "net_" + std::to_string(id) : _name),
        voltage(0.0)
    {}  

    bool isGround() const { return id == 0; }
};