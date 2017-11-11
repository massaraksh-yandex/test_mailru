#pragma once

#include <iostream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <map>

#include <common/log.h>


namespace common {

std::wstring toLowerWstring(const std::string& str);
std::wstring toLowerWstring(std::wstring str);
std::string toString(const std::wstring& str);

struct MarkovException: public std::runtime_error {
    MarkovException(const std::string& str)
        : std::runtime_error(str)
    { }
};

struct WrongDimension: public MarkovException {
    WrongDimension(std::size_t expected, std::size_t got)
        : MarkovException(std::string("wrong dimension: expected=") + std::to_string(expected) +
                          std::string(" got=") + std::to_string(got))
    { }
};

struct UnexistentNode: public MarkovException {
    UnexistentNode(const std::wstring& name)
        : MarkovException(std::string("missing node: name=") + toString(name))
    { }
};

struct Markov {
    struct Node {
        using Children = std::map<std::wstring, Node>;
        std::size_t count = 0;
        Children children;

        const Node& operator[](const std::wstring& str) const;
        Children::const_iterator find(const std::wstring& str) const;
        Children::const_iterator end() const {
            return children.end();
        }
    };

    Markov(std::size_t n)
        : dimension_(n)
    { }

    std::size_t count() const {
        return parent.count;
    }

    std::size_t dimension() const {
        return dimension_;
    }

    const Node& root() const {
        return parent;
    }

    const Node& operator[](const std::wstring& str) const {
        return parent[str];
    }

    std::list<std::wstring> complete(std::list<std::wstring> vec) const;

    template<class Iterator>
    void append(Iterator begin, Iterator end, std::size_t count) {
        auto distance = std::distance(begin, end);
        if (distance != dimension_) {
            throw WrongDimension(dimension_, distance);
        }

        Node* current = &parent;
        current->count += count;
        while(begin != end) {
            current = &current->children[toLowerWstring(*begin++)];
            current->count += count;
        }
    }

    Markov& append(Markov&& other);

private:
    static std::wstring delimiter;
    std::size_t dimension_;
    Node parent;

    template<class Handler>
    friend void forEachLeaf(const Markov& net, Handler h);
    friend std::wostream& operator<<(std::wostream& out, const Markov& net);
    friend std::wistream& operator>>(std::wistream& in, Markov& net);
};

}
