#define BOOST_TEST_MODULE insert_to_markov
#define BOOST_TEST_DYN_LINK


#include <boost/test/unit_test.hpp>

#include <builder/markov.h>
#include <common/log.h>


inline common::Markov getNet() {
    constexpr std::size_t dimencion = 3;
    return common::Markov(dimencion);
}


BOOST_AUTO_TEST_SUITE(insertToMarkov)

BOOST_AUTO_TEST_CASE(shouldInsertWordsToNet) {
    auto net = getNet();

    builder::ReadChunkResult::Words words = { "a", "b", "c" };

    builder::insertToMarkov(net, words);

    BOOST_TEST(net.count() == 1);
    BOOST_TEST(net[L"a"][L"b"][L"c"].count == 1);
}

BOOST_AUTO_TEST_CASE(shouldNotInsertWordsToNetIfTheSequenseIsTooShort) {
    auto net = getNet();

    builder::ReadChunkResult::Words words = { "a", "b" };

    auto ret = builder::insertToMarkov(net, words);

    BOOST_TEST(net.count() == 0);
    BOOST_TEST(ret == words);
}

BOOST_AUTO_TEST_CASE(shouldInsertManyWordsToNetAndReturnWordsToConcatWithNextChunk) {
    auto net = getNet();

    builder::ReadChunkResult::Words words = { "a", "b", "c", "d", "e", "f" };
    builder::ReadChunkResult::Words toNextChunk = { "e", "f" };

    auto ret = builder::insertToMarkov(net, words);

    BOOST_TEST(net.count() == 4);
    BOOST_TEST(net[L"a"][L"b"][L"c"].count == 1);
    BOOST_TEST(net[L"b"][L"c"][L"d"].count == 1);
    BOOST_TEST(net[L"c"][L"d"][L"e"].count == 1);
    BOOST_TEST(net[L"d"][L"e"][L"f"].count == 1);
    BOOST_TEST(ret == toNextChunk);
}

BOOST_AUTO_TEST_CASE(_shouldInsertManyWordsToNetAndReturnWordsToConcatWithNextChunk) {
    auto net = common::Markov(2);

    builder::ReadChunkResult::Words words = { "a", "b", "c" };

    auto ret = builder::insertToMarkov(net, words);


        std::wcout << common::Markov(2).append(std::move(net)) << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(lastInsertToMarkov)

BOOST_AUTO_TEST_CASE(shouldFillMissingWordsWithEmptyStrings) {
    auto net = getNet();

    builder::ReadChunkResult::Words words = { "a", "b" };

    builder::lastInsertToMarkov(net, words);

    BOOST_TEST(net.count() == 1);
    BOOST_TEST(net[L"a"][L"b"][L""].count == 1);
}

BOOST_AUTO_TEST_SUITE_END()
