#pragma once

#include <algorithm>
#include <string>
#include <list>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/join.hpp>
#include <boost/tokenizer.hpp>


namespace builder {

struct ReadChunkResult {
    using Words = std::list<std::string>;
    Words words;
    std::string lastWord;

    void append(ReadChunkResult result) {
        std::copy(result.words.begin(), result.words.end(), std::back_inserter(words));
        lastWord = std::move(result.lastWord);
    }
};

template<class Iterator>
ReadChunkResult readChunk(Iterator begin, Iterator end, std::string lastWord) {
    if (begin == end) {
        return ReadChunkResult{{}, lastWord};
    }

    std::string data = lastWord;
    std::copy(begin, end, std::back_inserter(data));

    boost::tokenizer<> tokens(data);

    std::list<std::string> words;
    boost::copy(tokens, std::back_inserter(words));


    if (words.empty()) {
        lastWord += std::string(begin, end);
    } else {
        const bool lastSymbolIsNotADelimiter = words.back().back() == *(end-1);
        if (lastSymbolIsNotADelimiter) {
            lastWord = words.back();
            words.pop_back();
        } else {
            lastWord = std::string();
        }
    }

    return ReadChunkResult{std::move(words), std::move(lastWord)};
}

}
