//
// Created by kepler-22b on 10/03/23.
//

#ifndef LI_SAT_SOLVER_DSTACK_H

#include "satBasicDef.h"
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <set>

#include <iostream>

class DStack {

private:

    class DepNode {

    private:

        std::vector<LID> deps;

    public:

        inline explicit DepNode(const std::vector<LID>& cause) : deps(cause) {}

        inline static void recGen(std::set<L>& acc, DStack& root, LID id) {

            //std::cout << id << ":";

            acc.insert(*root.ind[id]);

            if (root.depTree[id] == nullptr)
                return;

            for (LID i: root.depTree[id]->deps)
                recGen(acc, root,i);
        }
    };

    class Decision {

    private:

        DStack&        root;
        L              deci;
        std::vector<L> prop;

    public:

        inline Decision(LID id, LST st, DStack& root) : deci(id, st), prop(), root(root) {

            root.check.push(id);

            root.ind[id] = &deci;

            root.model[id] = st;
        }

        inline void registerProp(LID id, LST st, const std::vector<LID>& cause) {

            root.check.push(id);

            prop.emplace_back(id, st);

            root.depTree[id] = new DStack::DepNode(cause);

            root.ind[id] = &deci;

            root.model[id] = st;
        }

        inline void registerProp(LID id, LST st) {

            root.check.push(id);

            prop.emplace_back(id, st);

            root.ind[id] = (new L(id, st)); //modif

            root.model[id] = st;
        }

        inline ~Decision() {

            root.ind[deci.getId()] = nullptr;

            root.model[deci.getId()] = UNDEF;

            for (const L& l: prop) {

                delete(root.depTree[l.getId()]);
                root.depTree[l.getId()] = nullptr;

                root.ind[l.getId()] = nullptr;

                root.model[l.getId()] = UNDEF;
            }
        }

        [[nodiscard]] inline L getDeci() const {

            return deci;
        }
    };

    std::vector<DepNode*> depTree;
    std::stack<Decision>  dStack;
    std::vector<L*>       ind;

    std::queue<LID>       check;
    std::vector<LST>      model;

    void registerProp(LID id, LST st) {

        if (dStack.empty()) {

            check.push(id);
            model[id] = st;

            ind[id] = (new L(id, st));

            //std::cout << "Empty stack" << std::endl;

            return;
        }

        dStack.top().registerProp(id, st);
    }

public:

    explicit DStack(LID num) {

        depTree = std::vector<DepNode*>(num, nullptr);
        ind = std::vector<L*>(num, nullptr);
        dStack = std::stack<Decision>();
        check = std::queue<LID>();
        model = std::vector<LST>(num, UNDEF);
    }

    [[nodiscard]] std::vector<PL>* popConflict(const std::vector<LID>& conflict) {

        std::set<L> acc = std::set<L>();

        for (const LID& id: conflict)
            DepNode::recGen(acc, *this, id);

        //std::cout << std::endl;

        check = std::queue<LID>();

        L d = dStack.top().getDeci();

        dStack.pop();

        d.reverse();

        //std::cout << "<<" << d << std::endl;

        std::vector<PL>& ret = *(new std::vector<PL>(acc.size()));

        std::transform(acc.begin(), acc.end(), ret.begin(), [this] (const L& l) -> PL {

            return {l.getId(), &model[l.getId()], (LST)-l.getSt()};
        });

        registerProp(d.getId(), d.getSt());
/*
        for (const PL& l: ret)
            std::cout << L(l.getId(), l.getSt()) << " ";
        std::cout << std::endl;*/

        return &ret;
    }

    void registerProp(LID id, LST st, const std::vector<LID>& cause) {

        if (dStack.empty()) {

            check.push(id);
            model[id] = st;

            ind[id] = (new L(id, st));

            //std::cout << "Empty stack" << std::endl;

            return;
        }

        if (cause.size() < 2) {

            check.push(id);
            model[id] = st;

            ind[id] = (new L(id, st));

            return;
        }

        dStack.top().registerProp(id, st, cause);
    }

    void setDecision(LID id, LST st) {

        dStack.emplace(id, st, *this);

        //std::cout << ">>" << L(id, st) << std::endl;
    }

    [[nodiscard]] inline std::queue<LID>& getQueue() {

        return check;
    }

    [[nodiscard]] inline std::vector<LST>& getModel() {

        return model;
    }

    [[nodiscard]] inline bool end() const {

        return dStack.empty();
    }
};

#define LI_SAT_SOLVER_DSTACK_H

#endif //LI_SAT_SOLVER_DSTACK_H
