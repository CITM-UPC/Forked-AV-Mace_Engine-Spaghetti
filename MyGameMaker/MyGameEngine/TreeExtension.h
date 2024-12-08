#pragma once

#include <list>
#include <memory>
#include "readOnlyView.h"

template <class T>
class TreeExtension
{
private:
    T* _parent = nullptr;
    std::list<std::shared_ptr<T>> _children;

public:
    auto& parent() { return *_parent; }
    const auto& parent() const { return _parent; }
    bool hasParent() const { return _parent != nullptr; }
	auto& setParent(T* parent) { 
        _parent = parent; 
        return *static_cast<T*>(this);
    }

    //auto& root() const { return hasParent() ? _parent->root() : *this; }
    bool isRoot() const { return !hasParent(); }

    auto children() const { return readOnlyListView<std::shared_ptr<T>>(_children); }  // Devuelve vista solo lectura de punteros compartidos

    auto& addChild(const std::shared_ptr<T>& child) {
        child->setParent(static_cast<T*>(this));
        _children.push_back(child); 
        return child;
    }

    template <typename ...Args>
    auto& emplaceChild(Args&&... args) {
        auto child = std::make_shared<T>(std::forward<Args>(args)...);
        child->setParent(static_cast<T*>(this));
        _children.push_back(child);
        return *child;
    }

    void removeChild(const std::shared_ptr<T>& child) {
        _children.remove(child);
    }
    // Unparent this node (removes from parent's children)
    void unparent() {
        if (_parent) {
            auto sharedThis = std::shared_ptr<T>(static_cast<T*>(this));
            _parent->_children.remove(sharedThis);
            _parent = nullptr;
        }
    }
    // Reparent this node to a new parent
    void reparent(T* newParent) {
        if (newParent != _parent) {
            // Remove from old parent
            unparent();

            // Add to new parent
            if (newParent) {
                setParent(newParent);
                auto sharedThis = std::shared_ptr<T>(static_cast<T*>(this));
                newParent->_children.push_back(sharedThis);
            }
        }
    }
    // Check if this node is a descendant of potential ancestor
    bool isDescendantOf(const T* potentialAncestor) const {
        if (!_parent) return false;
        if (_parent == potentialAncestor) return true;
        return _parent->isDescendantOf(potentialAncestor);
    }

};