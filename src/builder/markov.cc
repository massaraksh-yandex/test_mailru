#include <builder/markov.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/iterator_range.hpp>


namespace builder {

namespace {
template<class Iter>
std::string log(Iter begin, Iter end) {
    return boost::algorithm::join(boost::iterator_range<Iter>(begin, end), ",");
}
}

ReadChunkResult::Words insertToMarkov(common::Markov& net, ReadChunkResult::Words words) {
    BOOST_LOG_TRIVIAL(debug) << __PRETTY_FUNCTION__
                             << " words_length: " << words.size()
                             << " words: [" << log(words.begin(), words.end()) << "]";

    if (words.size() < net.dimension()) {
        return words;
    }

    auto first = words.begin();
    auto endOfSeq = words.begin();
    std::advance(endOfSeq, net.dimension());

    do {
        BOOST_LOG_TRIVIAL(debug) << __PRETTY_FUNCTION__
                                 << " sequense: [" << log(first, endOfSeq) << "]";
        net.append(first++, endOfSeq, 1);
    } while (endOfSeq++ != words.end());

    words.erase(words.begin(), first);

    return words;
}

void lastInsertToMarkov(common::Markov& net, ReadChunkResult::Words words) {
    if (words.size() < net.dimension()) {
        std::fill_n(std::back_inserter(words), net.dimension() - words.size(), "");
    }

    insertToMarkov(net, std::move(words));
}

}
