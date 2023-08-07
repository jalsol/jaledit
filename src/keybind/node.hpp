#pragma once

#include "constants.hpp"
#include "utils.hpp"

#include <array>
#include <concepts>
#include <tuple>

namespace keybind {

class Node {
public:
    void set_parent(Node* parent);
    Node*& parent();

    std::array<Node*, constants::char_limit>& children();
    const std::array<Node*, constants::char_limit>& children() const;
    Node*& child(char c);
    virtual void call(bool _) { (void)_; };

    virtual bool is_func() { return false; };
    virtual ~Node();

protected:
    Node* m_parent{};
    std::array<Node*, constants::char_limit> m_children{};
};

template<std::invocable Func>
class FuncNode : public Node {
public:
    FuncNode(Func func, bool editable);
    void call(bool editable) override;

    bool is_func() override { return true; }

private:
    Func m_func;
    bool m_editable;
};

} // namespace keybind

#include "keybind/node_inl.hpp"