#include <iostream>

using std::string;

class Component {
    string name;
    int value;
public:
    Component(const string &name, int value) : name(name), value(value) {}

    void setName(const string &name) {
        Component::name = name;
    }

    const string &getName() const {
        return name;
    }

    void setValue(int value) {
        Component::value = value;
    }

    int getValue() const {
        return value;
    }
};

class Source : Component {
    int internal_resistance;
    bool orientedTowardsSecondNode;
    bool ideal;
public:
    Source(const string &name, int value, int internalResistance, bool orientedTowardsSecondNode, bool ideal)
            : Component(name, value), internal_resistance(internalResistance),
              orientedTowardsSecondNode(orientedTowardsSecondNode), ideal(ideal) {}

    int getInternalResistance() const {
        return internal_resistance;
    }

    void setInternalResistance(int internalResistance) {
        internal_resistance = internalResistance;
    }

    bool isOrientedTowardsSecondNode() const {
        return orientedTowardsSecondNode;
    }

    void setOrientedTowardsSecondNode(bool orientedTowardsSecondNode) {
        Source::orientedTowardsSecondNode = orientedTowardsSecondNode;
    }

    bool isIdeal() const {
        return ideal;
    }

    void setIdeal(bool ideal) {
        Source::ideal = ideal;
    }
};
int main() {

}