#include <common/markov.h>

#include <functional>
#include <vector>
#include <list>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>


namespace common {

std::wstring Markov::delimiter = L"\t";

std::string toString(const std::wstring& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
}

std::wstring toLowerWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wstr = conv.from_bytes(str);

    for (std::size_t i = 0; i < wstr.size(); i++) {
        wstr[i] = std::towlower(wstr[i]);
    }
    return wstr;
}

std::wstring toLowerWstring(std::wstring str) {
    for (std::size_t i = 0; i < str.size(); i++) {
        str[i] = std::towlower(str[i]);
    }
    return str;
}

const Markov::Node& Markov::Node::operator[](const std::wstring& str) const  {
    auto it = find(str);
    if (it == children.end()) {
        throw UnexistentNode(str);
    }

    return it->second;
}

Markov::Node::Children::const_iterator Markov::Node::find(const std::wstring& str) const {
    return children.find(str);
}

std::list<std::wstring> Markov::complete(std::list<std::wstring> vec) const {
    if (vec.size() > dimension()) {
        throw WrongDimension(dimension(), vec.size());
    } else if (vec.size() == dimension()) {
        return vec;
    }

    std::transform(std::make_move_iterator(vec.begin()),
                   std::make_move_iterator(vec.end()),
                   vec.begin(), [] (auto&& val) { return toLowerWstring(std::move(val)); });

    const Node* node = &root();
    std::list<std::wstring> ret;

    for(const auto& str: vec) {
        auto it = node->find(str);
        if (it == node->end()) {
            return ret;
        } else {
            node = &(*node)[str];
            ret.push_back(str);
        }
    }

    auto iter = std::back_inserter(ret);
    std::size_t count = dimension() - vec.size();
    for(std::size_t i = 0; i < count; i++) {
        auto max = std::max_element(node->children.begin(), node->children.end(), [](auto&& a, auto&& b) {
             return a.second.count < b.second.count;
        });

        if (max == node->children.end()) {
            throw MarkovException("unexpected end of chain");
        }

        *iter++ = std::move(max->first);
        node = &(*node)[max->first];
    }

    return ret;
}

namespace {
struct StackFrame {
    const Markov::Node& node;
    Markov::Node::Children::const_iterator iter;
    const Markov::Node::Children::const_iterator end;

    StackFrame(const Markov::Node& n)
        : node(n)
        , iter(node.children.begin())
        , end(node.children.end())
    { }

    const std::wstring& key() const {
        return iter->first;
    }
};
}

template<class Handler>
void forEachLeaf(const Markov& net, Handler h) {
    std::list<StackFrame> frames = { StackFrame(net.parent) };
    std::vector<std::wstring> line(net.dimension(), L"");

    while (!frames.empty()) {
        while(frames.size() != (net.dimension())) {
            line[frames.size()-1] = frames.back().key();
            frames.push_back(StackFrame(frames.back().iter->second));
        }

        for(auto it = frames.back().iter; it != frames.back().end; it++) {
            line.back() = it->first;
            h(line, it->second.count);
        }

        frames.pop_back();

        while (!frames.empty() && ++frames.back().iter == frames.back().end) {
            frames.pop_back();
        }
    }
}

std::wostream& operator<<(std::wostream& out, const Markov& net) {
    forEachLeaf(net, [&out](const auto& line, std::size_t count) {
        out << count << Markov::delimiter << boost::algorithm::join(line, Markov::delimiter) << std::endl;
    });

    return out;
}

Markov& Markov::append(Markov&& other) {
    if (dimension() != other.dimension()) {
        throw WrongDimension(dimension(), other.dimension());
    }

    forEachLeaf(other, [self=this] (const auto& parents, std::size_t count) {
        self->append(parents.begin(), parents.end(), count);
    });

    other.parent.children.clear();
    other.parent.count = 0;

    return *this;
}

std::wistream& operator>>(std::wistream& in, Markov& net) {
    while (in) {
        std::wstring line;
        std::getline(in, line, L'\n');

        if (line.empty()) {
            break;
        }

        std::vector<std::wstring> values;
        boost::split(values, line, boost::is_any_of(Markov::delimiter), boost::token_compress_on);

        if (values.size() != net.dimension()+1) {
            throw WrongDimension(net.dimension(), values.size()-1);
        }

        net.append(values.begin()+1, values.end(), std::stoul(*values.begin()));
    }

    return in;
}

}
