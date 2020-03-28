#include <iostream>
#include <cmath>
#include <vector>


using std::string;
using std::pair;
using std::vector;

const double EPSILON = 10e-7;

class Component {
    string name;
public:
    Component(const string &name) : name(name) {}

    void setName(const string &name) {
        Component::name = name;
    }

    const string &getName() const {
        return name;
    }
};

class Source: public Component {
    double internalResistance;
    bool orientedTowardsSecondNode;
    double value;
public:
    explicit Source(const string &name, double value = 5, bool orientedTowardsSecondNode = true) :
        Component(name), value(value), orientedTowardsSecondNode(orientedTowardsSecondNode) {}

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        Source::internalResistance = internalResistance;
    }

    bool isOrientedTowardsSecondNode() const {
        return orientedTowardsSecondNode;
    }

    void setOrientedTowardsSecondNode(bool orientedTowardsSecondNode) {
        Source::orientedTowardsSecondNode = orientedTowardsSecondNode;
    }

    virtual bool isIdeal() = 0;

    virtual void setIdeal() = 0;

    void setValue(double value) {
        Source::value = value;
    }

    double getValue() const {
        return value;
    }
};

class VoltageSource: public Source {
public:
    VoltageSource(const string &name, double value, bool orientedTowardsSecondNode, double internalResistance = 0) :
    Source(name, value, orientedTowardsSecondNode) {
        this->setInternalResistance(internalResistance);
    }

    void setIdeal() override {
        this->setInternalResistance(0);
    }

    bool isIdeal() override {
        return fabs(this->getInternalResistance()) < EPSILON;
    }
};

class CurrentSource: public Source {
public:
    CurrentSource(const string &name, double value, bool orientedTowardsSecondNode, double internalResistance = -1) :
    Source(name, value, orientedTowardsSecondNode) {
        this->setInternalResistance(internalResistance);
    }

    bool isIdeal() override {
        return fabs(1 + getInternalResistance()) < EPSILON;
    }

    void setIdeal() override {
        this->setInternalResistance(-1);
    }
};

class Resistor: public Component {
    double value;
public:
    Resistor(const string &name, double value) : Component(name) {
        setValue(value);
    };

    void setValue(double value) {
        Resistor::value = value;
    }

    double getValue() const {
        return value;
    }
};

class Instrument: public Component {
    bool orientedTowardsSecondNode;
    double internalResistance;
    double displayedValue;
public:
    Instrument(const string &name, bool orientedTowardsSecondNode, double internalResistance) : Component(name),
        orientedTowardsSecondNode(orientedTowardsSecondNode), internalResistance(internalResistance) {}

    bool isOrientedTowardsSecondNode() const {
        return orientedTowardsSecondNode;
    }

    void setOrientedTowardsSecondNode(bool orientedTowardsSecondNode) {
        Instrument::orientedTowardsSecondNode = orientedTowardsSecondNode;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        Instrument::internalResistance = internalResistance;
    }

    double getDisplayedValue() const {
        return displayedValue;
    }

    void setDisplayedValue(double displayedValue) {
        Instrument::displayedValue = displayedValue;
    }

    virtual void setIdeal() = 0;

    virtual bool isIdeal() = 0;
};

class Voltmeter : public Instrument {
public:
    Voltmeter(const string &name, bool orientedTowardsSecondNode, double internalResistance) :
        Instrument(name, orientedTowardsSecondNode, internalResistance) {}

    void setIdeal() override {
        this->setInternalResistance(0);
    }

    bool isIdeal() override {
        return fabs(getInternalResistance()) < EPSILON;
    }
};

class Ampermeter : public Instrument {
public:
    Ampermeter(const string &name, bool orientedTowardsSecondNode, double internalResistance) : Instrument(name,
                                                                                                           orientedTowardsSecondNode,
                                                                                                           internalResistance) {}

    void setIdeal() override {
        this->setInternalResistance(-1);
    }

    bool isIdeal() override {
        return fabs(getInternalResistance() + 1) < EPSILON;
    }
};

class Wattmeter : public Instrument {
    Voltmeter voltmeter;
    Ampermeter ampermeter;
};

class Node {
    string name;
    double voltage;


public:
    Node();
    Node(const string &name) : name(name) {}

    const string &getName() const {
        return name;
    }

    void setName(const string &name) {
        Node::name = name;
    }

    double getVoltage() const {
        return voltage;
    }

    virtual void setVoltage(double voltage) {
        Node::voltage = voltage;
    }
};

class Ground : public Node {
public:
    Ground(const string &name) : Node(name) {
        this->setVoltage(0);
    }
};

class Branch {
    double current;
    string name;
    pair<Node, Node> nodes;
    vector<Component> components;

public:


    Branch(string name, const pair<Node, Node> &nodes, const vector<Component> &components) : name(name), nodes(nodes), components(components){}



    const string &getName() const {
        return name;
    }



    void setName(const string &name) {
        Branch::name = name;
    }

    double getCurrent() const {
        return current;
    }

    void setCurrent(double current) {
        Branch::current = current;
    }

    pair<Node, Node> &getNodes() {
        return nodes;
    }

    void setNodes(const pair<Node, Node> &nodes) {
        Branch::nodes = nodes;
    }

    const Node &getFirstNode() const {
        return nodes.first;
    };

    const Node &getSecondNode() const {
        return nodes.second;
    }

    void setFirstNode(const Node &node) {
        nodes.first = node;
    }

    void setSecondNode(const Node &node) {
        nodes.second = node;
    }

    vector<Component> &getComponents() {
        return components;
    }

    void setComponents(const vector<Component> &components) {
        Branch::components = components;
    }

    void addComponent(const Component &component) {
        components.push_back(component);
    }

    void removeComponent(const Component &component) {
        for(int i = 0; i < components.size(); i++) {
            if (components.at(i).getName() == component.getName()) {
                components.erase(components.begin() + i);
                break;
            }
        }
    }
};

//int main() {


//}