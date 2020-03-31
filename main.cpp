#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <utility>


using std::string;
using std::pair;
using std::vector;

const double EPSILON = 10e-7;

//Source is referred to by its ID. No two sources should have the same ID.
//naturalOrientation means that the + contact is to the right or up, depending on the circuit.
//in a Branch, naturalOrientation = true means the Source is oriented towards the second Node in the Branch

class VoltageSource {
    int id;
    double voltage;
    double internalResistance;
    bool naturalOrientation;
public:
    explicit VoltageSource(int id, double voltage = 10, double internalResistance = 0, bool naturalOrientation = true) {
        setId(id);
        setVoltage(voltage);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    };

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0) throw std::domain_error("IDs start at 0!");
        VoltageSource::id = id;
    }

    double getVoltage() const {
        return voltage;
    }

    void setVoltage(double voltage) {
        if(voltage < 0) {
            voltage *= -1;
            this->naturalOrientation = !this->naturalOrientation;
        }
        this->voltage = voltage;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0)
            throw std::domain_error("Resistance can't be negative!");
        VoltageSource::internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        this->naturalOrientation = naturalOrientation;
    }

    void toggleOrientation() {
        this->naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = 0;
    }

    //operators

    friend bool operator == (const VoltageSource &e1, const VoltageSource &e2) {
        return e1.id == e2.id;
    }

    friend bool operator != (const VoltageSource &e1, const VoltageSource &e2) {
        return !(e1 == e2);
    }

    friend bool operator < (const VoltageSource &e1, const VoltageSource &e2) {
        return e1.id < e2.id;
    }

    friend bool operator <= (const VoltageSource &e1, const VoltageSource &e2) {
        return e1 < e2 || e1 == e2;
    }

    VoltageSource &operator +=(const VoltageSource &e) { //sources in series
        if(this->naturalOrientation == e.naturalOrientation)
            this->voltage += e.voltage;
        else {
            if(this->voltage > e.voltage)
                this->voltage = this->voltage - e.voltage;
            else {
                this->toggleOrientation();
                this->voltage = this->voltage - e.voltage;
            }
        }
        this->internalResistance += e.internalResistance;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const VoltageSource &e) {
        if(e.isNaturalOrientation())
            os << "E" << e.getId() << " (" << e.getVoltage() << "V)+";
        else
            os << "E" << e.getId() << " +(" << e.getVoltage() << "V)";

        return os;
    }
};

//internal resistance of an ideal CurrentSource is infinite... since we don't want to play with huge values, we'll use -1

class CurrentSource {
    int id;
    double current;
    double internalResistance;
    bool naturalOrientation;
public:

    CurrentSource(int id, double current = 1, double internalResistance = -1, bool naturalOrientation = true) {
        setId(id);
        setCurrent(current);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::domain_error("IDs start at 0!");
        CurrentSource::id = id;
    }

    double getCurrent() const {
        return current;
    }

    void setCurrent(double current) {
        if(current < 0) {
            current *= -1;
            this->naturalOrientation = !this->naturalOrientation;
        }
        this->current = current;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0 && fabs(1 + internalResistance) > EPSILON)
            throw std::domain_error("Resistance can't be negative!");
        this->internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        CurrentSource::naturalOrientation = naturalOrientation;
    }

    void toggleNaturalOrientation() {
        naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(1 + internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = -1;
    }

    //operators

    friend bool operator == (const CurrentSource &i1, const CurrentSource &i2) {
        return i1.id == i2.id;
    }

    friend bool operator != (const CurrentSource &i1, const CurrentSource &i2) {
        return !(i1 == i2);
    }

    friend bool operator < (const CurrentSource &i1, const CurrentSource &i2) {
        return i1.id < i2.id;
    }

    friend bool operator <= (const CurrentSource &i1, const CurrentSource &i2) {
        return i1 < i2 || i1 == i2;
    }

    friend std::ostream& operator<<(std::ostream& os, const CurrentSource &i)
    {
        if(i.isNaturalOrientation())
            os << "I" << i.getId() << " (" << i.getCurrent() << "A->)";
        else
            os << "I" << i.getId() << " (<-" << i.getCurrent() << "A)";

        return os;
    }
};

class Resistor {
    int id;
    double resistance;
public:
    Resistor(int id, double resistance){
        setId(id);
        setResistance(resistance);
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::domain_error("IDs start at 0!");
        Resistor::id = id;
    }

    double getResistance() const {
        return resistance;
    }

    void setResistance(double resistance) {
        if(resistance < 0) throw std::domain_error("Resistance can't be negative!");
        this->resistance = resistance;
    }

    //operators

    friend bool operator == (const Resistor &r1, const Resistor &r2) {
        return r1.id == r2.id;
    }

    friend bool operator != (const Resistor &r1, const Resistor &r2) {
        return !(r1 == r2);
    }

    friend bool operator < (const Resistor &r1, const Resistor &r2) {
        return r1.id < r2.id;
    }

    friend bool operator <= (const Resistor &r1, const Resistor &r2) {
        return r1 < r2 || r1 == r2;
    }

    Resistor &operator +=(const Resistor &r) { //sources in series
        this->resistance += r.resistance;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Resistor &r)
    {
        os << "R" << r.getId() << " -[" << r.getResistance() << "Ohm]-";
        return os;
    }
};

//ideal Voltmeter has an infinite resistance, we used -1 instead

class Voltmeter {
    int id;
    double voltage;
    double internalResistance;
    bool naturalOrientation;
public:
    explicit Voltmeter(int id, double internalResistance = -1, bool naturalOrientation = true) {
        setId(id);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    };

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0) throw std::domain_error("IDs start at 0!");
        Voltmeter::id = id;
    }

    double getVoltage() const {
        return voltage;
    }

    void setVoltage(double voltage) {
        this->voltage = voltage;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0 && fabs(1 + internalResistance) > EPSILON)
            throw std::domain_error("Resistance can't be negative!");
        Voltmeter::internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        this->naturalOrientation = naturalOrientation;
    }

    void toggleOrientation() {
        this->naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(1+internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = -1;
    }

    //operators

    friend bool operator == (const Voltmeter &e1, const Voltmeter &e2) {
        return e1.id == e2.id;
    }

    friend bool operator != (const Voltmeter &e1, const Voltmeter &e2) {
        return !(e1 == e2);
    }

    friend bool operator < (const Voltmeter &e1, const Voltmeter &e2) {
        return e1.id < e2.id;
    }

    friend bool operator <= (const Voltmeter &e1, const Voltmeter &e2) {
        return e1 < e2 || e1 == e2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Voltmeter &v)
    {
        if(v.isNaturalOrientation())
            os << "V" << v.getId() << " ([" << v.getVoltage() << "V])+";
        else
            os << "V" << v.getId() << " +([" << v.getVoltage() << "V])";

        return os;
    }
};

class Ampermeter {
    int id;
    double current;
    double internalResistance;
    bool naturalOrientation;
public:
    Ampermeter(int id, double internalResistance = -1, bool naturalOrientation = true) {
        setId(id);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::domain_error("IDs start at 0!");
        Ampermeter::id = id;
    }

    double getCurrent() const {
        return current;
    }

    void setCurrent(double current) {
        this->current = current;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0 && fabs(1 + internalResistance) > EPSILON)
            throw std::domain_error("Resistance can't be negative!");
        this->internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        Ampermeter::naturalOrientation = naturalOrientation;
    }

    void toggleNaturalOrientation() {
        naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(1 + internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = -1;
    }

    //operators

    friend bool operator == (const Ampermeter &a1, const Ampermeter &a2) {
        return a1.id == a2.id;
    }

    friend bool operator != (const Ampermeter &a1, const Ampermeter &a2) {
        return !(a1 == a2);
    }

    friend bool operator < (const Ampermeter &a1, const Ampermeter &a2) {
        return a1.id < a2.id;
    }

    friend bool operator <= (const Ampermeter &a1, const Ampermeter &a2) {
        return a1 < a2 || a1 == a2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Ampermeter &a)
    {
        if(a.isNaturalOrientation())
            os << "A" << a.getId() << " ([" << a.getCurrent() << "A]->)";
        else
            os << "I" << a.getId() << " (<-[" << a.getCurrent() << "A])";

        return os;
    }
};

class Wattmeter {
    Voltmeter voltmeter;
    Ampermeter ampermeter;
    //to be implemented
};

class Node {
    int id;
    double voltage;

public:
    Node() {}

    Node(int id) {
        setId(id);
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::range_error("IDs start at 0!");
        Node::id = id;
    }

    double getVoltage() const {
        return voltage;
    }

    virtual void setVoltage(double voltage) {
        Node::voltage = voltage;
    }

    //operators

    friend bool operator == (const Node &n1, const Node &n2) {
        return n1.id == n2.id;
    }

    friend bool operator != (const Node &n1, const Node &n2) {
        return !(n1 == n2);
    }

    friend bool operator < (const Node &n1, const Node &n2) {
        return n1.id < n2.id;
    }

    friend bool operator <= (const Node &n1, const Node &n2) {
        return n1 < n2 || n1 == n2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Node &n)
    {
        os << "N" << n.getId() << " " << n.getId() << "V";
        return os;
    }
};

class Ground : public Node {
public:
    Ground() {
        Node::setVoltage(0);
    }

    void setVoltage(double voltage) = 0;
};

class Branch {
    int id;
    pair<Node, Node> nodes;
    vector<std::shared_ptr<Component>> components;
    double current;
public:
  
    Branch(string name, const pair<Node, Node> &nodes, vector<std::shared_ptr<Component>> components) : name(name), nodes(nodes), components(std::move(components)) {}

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

    vector<std::shared_ptr<Components>> getComponents() {
        return components;
    }

    void setComponents(vector<std::shared_ptr<Component>> components) {
        Branch::components = components;
    }

    void addComponent(std::shared_ptr<Component> component) {
        std::shared_ptr<Component> c = std::move(component);
    }

    void removeComponent(std::shared_ptr<Component> component) {
        for(int i = 0; i < components.size(); i++) {
            if (components.at(i)->getName() == component->getName()) {
                components.erase(components.begin() + i);
                break;
            }
        }
    }

    friend bool operator == (const Branch &b1, const Branch &b2) {
        return b1.getName() == b2.getName();
    }

    friend bool operator != (const Branch &b1, const Branch &b2) {
        return !(b1 == b2);
    }

    friend bool operator < (const Branch &b1, const Branch &b2) {
        return std::stoi(b1.getName().substr(1, string::npos)) < std::stoi(b2.getName().substr(1, string::npos));
    }

    friend bool operator <= (const Branch &b1, const Branch &b2) {
        return b1 < b2 || b1 == b2;
    }
};
//int main() {
//return 0;
//}