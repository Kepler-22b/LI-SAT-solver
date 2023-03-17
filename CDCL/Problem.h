//
// Created by kepler-22b on 10/03/23.
//

#ifndef LI_SAT_SOLVER_PROBLEM_H

#include "DStack.h"
#include <iostream>
#include <algorithm>
#include <list>
#include <cmath>

class Problem {

private:

    typedef std::vector<PL> Clause;

    std::vector<Clause> root;

    std::vector<std::list<Clause*>> cLitTrue;
    std::vector<std::list<Clause*>> cLitFalse;

    DStack stack;

    std::queue<LID>*  check;
    std::vector<LST>* model;

    std::vector<Clause> conClauses;

    LID      numVars;
    uint64_t numClauses;

    [[nodiscard]] int printSat() const {

        for (const Clause& cl: conClauses)
            if (std::all_of(cl.begin(), cl.end(), PL::isFalseS)) {

                for (const PL& pl: cl)
                    std::cout << L(pl.getId(), pl.getSt());

                std::cout << std::endl;

                std::cout << "CONTRADICTION" << std::endl;
                break;
            }


        std::cout << "SATISFIABLE" << ' ' << std::endl;

        return 20;
    }

    static int printNotSat() {

        std::cout << "UNSATISFIABLE" << ' ' << std::endl;
        return 10;
    }

    bool clauseConflict(const Clause& c) {

        bool one = false;

        LID id;
        LST st;

        std::vector<LID> cause = std::vector<LID>();

        for (PL l: c)
            if (l.isTrue())
                return false;
            else if (l.isUndef()) {

                if (one) return false;

                id = l.getId();
                st = l.getSt();

                one = true;

            } else cause.push_back(l.getId());

        if (not one) return true;

        stack.registerProp(id, st, cause);
        return false;
    }

    void tryBacktrack(Clause* cl) {

        if (stack.end())
            exit(printNotSat());

        std::vector<LID> conflict = std::vector<LID>(cl->size());

        std::transform(cl->begin(), cl->end(), conflict.begin(),
                       [] (const PL &lit) -> LID {

                           return lit.getId();
                       });

        Clause* stop = stack.popConflict(conflict);

        conClauses.push_back(*stop);

        for (const PL& l: *stop)
            if (l.getSt() == TRUE)
                cLitTrue[l.getId()].push_front(stop);
            else
                cLitFalse[l.getId()].push_front(stop);
    }

    bool propagate() {

        while (not check->empty()) {

            LID id = check->front();
            check->pop();

            if ((*model)[id] == TRUE) {

                for (Clause* cl: cLitFalse[id])
                    if (clauseConflict(*cl)) {

                        tryBacktrack(cl);
                        return true;
                    }

            } else {

                for (Clause* cl: cLitTrue[id])
                    if (clauseConflict(*cl)) {

                        tryBacktrack(cl);
                        return true;
                    }
            }


        }

        return false;
    }

    void compPriority() {

        std::vector<float> value = std::vector<float>(numVars, 1);

        auto i = 1;

        for (auto it = root.rbegin(); it != root.rend(); ++it) {

            for (PL& pl: *it) {

                float &f = value[pl.getId()];

                f += static_cast<float>(log2(i));

                pl.setValue(f);
            }

            ++i;
        }

    }

    static void printErrorTerm(const Clause& c) {

        std::cout << "Error in model, clause is not satisfied:";

        for (const PL& l: c)
            std::cout << stateToSymbol(l.getSt()) << l.getId() + 1 << " ";

        std::cout << std::endl;
        exit(1);
    }

    static bool someLitTrue(const Clause& c) {

        return any_of(c.begin(), c.end(), PL::isTrueS);
    }

    void checkModel() {

        for (const Clause& c: root)
            if (not someLitTrue(c))
                printErrorTerm(c);
    }

    LID nextDecision() {

        uint8_t count = 0;

        LID ret    = 0;
        double val = 0;

        for (const Clause &c: root) {

            if (any_of(c.begin(), c.end(), PL::isTrueS)) continue;

            for (const PL &pl: c)
                if (pl.isUndef() && pl.getValue() >= val) {

                    ret = pl.getId();
                    val = pl.getValue();

                    ++count;

                    if (count > 5)
                        return ret;
                }

            if (count != 0)
                continue;

            std::cout << "Unexpected error" << std::endl;
            exit(-3);
        }

        if (count != 0)
            return ret;

        //no UNDEF lit found: terminate program

        checkModel();
        exit(printSat());
    }

    void makeDecision() {

        LID id = nextDecision();

        stack.setDecision(id, TRUE);

        //std::cout << id << stateToSymbol(stack.getModel()[id]) << std::endl;
    }

public:

    Problem() : numVars(), numClauses(), stack(0), conClauses() {

        // Skip comments
        char c;
        std::cin >> c;
        while (c == 'c') {
            while (c != '\n')
                std::cin >> c;
            std::cin >> c;
        }

        std::string aux;
        std::cin >> aux >> numVars >> numClauses;

        stack = DStack(numVars);

        root = std::vector<Clause>(numClauses, Clause());

        // Read clauses
        for (uint i = 0; i < numClauses; ++i) {

            int lit;

            while (std::cin >> lit and lit != 0) {

                if (lit > 0)
                    root[i].emplace_back(lit - 1, &stack.getModel()[lit -1], TRUE);
                else
                    root[i].emplace_back(-lit -1, &stack.getModel()[-lit -1], FALSE);
            }
        }

        cLitTrue  = std::vector<std::list<Clause*>>(numVars);
        cLitFalse = std::vector<std::list<Clause*>>(numVars);

        std::fill(cLitTrue.begin(), cLitTrue.end(), std::list<Clause*>());
        std::fill(cLitFalse.begin(), cLitFalse.end(), std::list<Clause*>());

        for (Clause& cl: root)
            for (const PL& l: cl)
                if (l.getSt() == TRUE)
                    cLitTrue[l.getId()].push_back(&cl);
                else
                    cLitFalse[l.getId()].push_back(&cl);

        check = &stack.getQueue();
        model = &stack.getModel();

        compPriority();
    }

    [[noreturn]] void run() {

        while (true) {

            while (propagate());

            makeDecision();
        }
    }
};

#define LI_SAT_SOLVER_PROBLEM_H

#endif //LI_SAT_SOLVER_PROBLEM_H
