#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cmath>
using namespace std;

enum LST : int8_t;
typedef uint16_t LID;

class Lit {

private:

    LID id;
    LST st;

public:

    Lit(LID id, LST st) : id(id), st(st) {}

    [[nodiscard]] LST state() const {
        return st;
    }

    [[nodiscard]] LID getId() const {
        return id;
    }

    void reverse() {
        st = (LST)(-st);
    }
};

typedef vector<Lit> Clause;
typedef vector<Clause> Problem;

enum LST : int8_t {
    FALSE = -1,
    UNDEF = 0,
    TRUE = 1
};

uint64_t numClauses;
LID numVars;

Problem clauses;
vector<LST> model;
vector<Lit> modelStack;

vector<vector<Clause*>> cLitTrue;
vector<vector<Clause*>> cLitFalse;

vector<LID> lRank;

uint nextIndex;
uint level;

void setLit(Lit);

void setLit(LID, LST);

LST currentModelValue(Lit);

int printSat();

int printNotSat();

void readInput();

void unitClauses();

void checkModel();

void backtrack();

bool clauseConflict(const Clause&);

bool propagateGivesConflict () {

    for (; nextIndex < modelStack.size(); ++nextIndex) {

        Lit &l = modelStack[nextIndex];

        switch (l.state()) {
            case FALSE:
                for (Clause* c: cLitTrue[l.getId()])
                    if (clauseConflict(*c))
                        return true;
            break;
            case UNDEF:
            break;
            case TRUE:
                for (Clause* c: cLitFalse[l.getId()])
                    if (clauseConflict(*c))
                        return true;
            break;
        }
    }

    return false;
}

LID nextDecision() {
/*
    for (LID id: lRank)
        if (model[id] == UNDEF)
            return id;*/

    for (uint i = 1; i <= numVars; ++i)
        if (model[i] == UNDEF)
            return i;

    //no UNDEF lit found: terminate program

    checkModel();
    exit(printSat());
}

void makeDecision() {

    LID id = nextDecision();

    modelStack.emplace_back(0, UNDEF);
    ++nextIndex;
    ++level;
    setLit(id, TRUE);
}

void initClauseIndex() {

    cLitTrue.resize(numVars + 1, vector<Clause*>());
    cLitFalse.resize(numVars + 1, vector<Clause*>());

    for (Clause& c: clauses)
        for (Lit l: c)
            switch (l.state()) {

                case FALSE:
                    cLitFalse[l.getId()].push_back(&c);
                break;
                case UNDEF:
                break;
                case TRUE:
                    cLitTrue[l.getId()].push_back(&c);
                break;
            }
}

void compPriority() {

    vector<double> value = vector<double>(numVars + 1, 0);

    double i = 1;

    for (const Clause& c: clauses) {

        for (const Lit& l: c)
            value[l.getId()] += sqrt(i);

        ++i;
    }

    lRank.resize(numVars);

    for (LID n = 0; n < lRank.size(); ++n)
        lRank[n] = n + 1;

    std::sort(lRank.begin(), lRank.end(), [&value] (LID i0, LID i1) -> bool {

        return value[i0] < value[i1];
    });
}

int main(){

    readInput();

    model.resize(numVars + 1,UNDEF);

    nextIndex = 0;
    level = 0;

    compPriority();

    initClauseIndex();

    unitClauses();

    while (true) {

        while (propagateGivesConflict()) {

            if (level == 0)
                exit(printNotSat());

            backtrack();
        }

        makeDecision();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void unitClauses() {

    // Take care of initial unit clauses, if any
    for (Clause& c: clauses) {

        if (c.size() != 1)
            continue;

        switch (currentModelValue(c[0])) {

            case FALSE:
                exit(printNotSat());
            case UNDEF:
                setLit(c[0]);
                break;
            case TRUE:
                break;
        }
    }
}

void readInput() {

    // Skip comments
    char c = cin.get();

    while (c == 'c') {

        while (c != '\n')
            c = cin.get();

        c = cin.get();
    }

    // Read "cnf numVars numClauses"
    string aux;
    cin >> aux >> numVars >> numClauses;

    clauses.resize(numClauses);

    // Read clauses
    for (uint i = 0; i < numClauses; ++i) {

        int lit;

        while (cin >> lit and lit != 0) {

            if (lit > 0)
                clauses[i].emplace_back(lit, TRUE);
            else
                clauses[i].emplace_back(-lit, FALSE);
        }
    }
}

char stateToSymbol(LST st) {

    switch (st) {
        case FALSE:
            return '-';
        case TRUE:
            return '+';
        case UNDEF:
            return ' ';
        default:
            return '?';
    }
}

void printErrorTerm(const Clause& c) {

    cout << "Error in model, clause is not satisfied:";

    for (Lit l: c)
        cout << "[" << stateToSymbol(l.state()) << "]" << l.getId() << " ";

    cout << endl;
    exit(1);
}

bool someLitTrue(const Clause& c) {

    return any_of(c.begin(), c.end(), [](Lit l) -> bool {
        return currentModelValue(l) == TRUE;
    });
}

void checkModel() {

    for (Clause& c: clauses)
        if (not someLitTrue(c))
            printErrorTerm(c);
}

int printSat() {

    cout << "SATISFIABLE" << endl;
    return 20;
}

int printNotSat() {

    cout << "UNSATISFIABLE" << endl;
    return 10;
}

LST currentModelValue(Lit l) {

    if (model[l.getId()] == UNDEF)
        return UNDEF;

    if (model[l.getId()] == l.state())
        return TRUE;

    return FALSE;
}

void backtrack() {

    Lit l = Lit(0, UNDEF);

    for (auto it = modelStack.rbegin();
         it != modelStack.rend();
         ++it, modelStack.pop_back()
            ) {

        if (it->getId() == 0)
            break;

        l = *it;
        model[l.getId()] = UNDEF;
    }

    modelStack.pop_back();
    --level;
    nextIndex = modelStack.size();

    l.reverse();
    setLit(l);
}



void setLit(Lit l) {

    modelStack.push_back(l);

    model[l.getId()] = l.state();
}

void setLit(LID id, LST st) {

    modelStack.emplace_back(id, st);

    model[id] = st;
}

bool clauseConflict(const Clause& c) {

    int num = 0;

    Lit lastLitUndef = Lit(0, UNDEF);

    for (Lit l: c)
        switch (currentModelValue(l)) {
            case FALSE:
                break;
            case UNDEF:
                ++num;
                lastLitUndef = l;
                break;
            case TRUE:
                return false;
        }

    switch (num) {
        case 0:
            return true;
        case 1:
            setLit(lastLitUndef);
        default:
            return false;
    }
}