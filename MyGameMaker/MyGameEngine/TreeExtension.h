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
    // Get shared pointer to this object from a child's list
    std::shared_ptr<T> getSharedFromChildren(T* ptr) {
        for (const auto& child : _children) {
            if (child.get() == ptr) {
                return child;
            }
        }
        return nullptr;
    }
    // Unparent this node (removes from parent's children)
    bool unparent() {
        if (_parent) {
            // Find our shared_ptr in parent's children
            auto selfPtr = _parent->getSharedFromChildren(static_cast<T*>(this));
            if (selfPtr) {
                _parent->_children.remove(selfPtr);
                _parent = nullptr;
                return true;
            }
        }
        return false;
    }
    // Reparent this node to a new parent
    bool reparent(T* newParent) {
        if (!newParent || newParent == this) {
            return false;
        }

        // Check for circular reference
        T* ancestor = newParent;
        while (ancestor) {
            if (ancestor == this) {
                return false;
            }
            ancestor = ancestor->_parent;
        }

        if (newParent != _parent) {
            // Get our shared_ptr from current parent
            std::shared_ptr<T> selfPtr;
            if (_parent) {
                selfPtr = _parent->getSharedFromChildren(static_cast<T*>(this));
                if (!selfPtr) {
                    return false;
                }
                _parent->_children.remove(selfPtr);
            }

            // Set new parent
            _parent = newParent;
            if (selfPtr) {
                newParent->_children.push_back(selfPtr);
            }
            return true;
        }
        return false;   
    }
    // Check if this node is a descendant of potential ancestor
    bool isDescendantOf(const T* potentialAncestor) const {
        if (!_parent) return false;
        if (_parent == potentialAncestor) return true;
        return _parent->isDescendantOf(potentialAncestor);
    }

};