#define BOOST_TEST_MODULE markov
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <common/markov.h>
#include <common/log.h>


inline common::Markov getNet() {
    constexpr std::size_t dimencion = 3;
    return common::Markov(dimencion);
}

inline common::Markov getFilledNet() {
    auto net = getNet();

    std::vector<std::vector<std::string>> words = {
        { "A", "B", "C" },
        { "A", "B", "D" },
        { "d", "B", "d" }
    };

    net.append(words[0].begin(), words[0].end(), 1);
    net.append(words[1].begin(), words[1].end(), 1);
    net.append(words[2].begin(), words[2].end(), 1);

    return net;
}

void compateNet(const common::Markov& net) {
    BOOST_TEST(net.count() == 3);
    BOOST_TEST(net[L"a"].count == 2);
    BOOST_TEST(net[L"a"][L"b"].count == 2);
    BOOST_TEST(net[L"a"][L"b"][L"c"].count == 1);
    BOOST_TEST(net[L"a"][L"b"][L"d"].count == 1);
    BOOST_TEST(net[L"d"].count == 1);
    BOOST_TEST(net[L"d"][L"b"].count == 1);
    BOOST_TEST(net[L"d"][L"b"][L"d"].count == 1);
}



BOOST_AUTO_TEST_SUITE(append)

BOOST_AUTO_TEST_CASE(shouldInsertWordsToNet) {
    auto net = getNet();

    std::vector<std::string> words = { "a", "b", "c" };

    net.append(words.begin(), words.end(), 1);

    BOOST_TEST(net.count() == 1);
    BOOST_TEST(net[L"a"].count == 1);
    BOOST_TEST(net[L"a"][L"b"].count == 1);
    BOOST_TEST(net[L"a"][L"b"][L"c"].count == 1);
}

BOOST_AUTO_TEST_CASE(shouldInsertLowerCaseVersionOfWordsToNet) {
    auto net = getNet();

    std::vector<std::string> words = { "A", "B", "C" };

    net.append(words.begin(), words.end(), 1);

    BOOST_TEST(net.count() == 1);
    BOOST_TEST(net[L"a"].count == 1);
    BOOST_TEST(net[L"a"][L"b"].count == 1);
    BOOST_TEST(net[L"a"][L"b"][L"c"].count == 1);
}

BOOST_AUTO_TEST_CASE(shouldIncrementCountersOnInsertingWords) {
    auto net = getFilledNet();

    compateNet(net);
}

BOOST_AUTO_TEST_CASE(shouldThrowAnExceptionOnWrongAppendSize) {
    auto net = getNet();

    std::vector<std::string> words = { "A", "B" };
    BOOST_CHECK_THROW(net.append(words.begin(), words.end(), 1), common::WrongDimension);

    words = { "A", "B", "C", "D" };
    BOOST_CHECK_THROW(net.append(words.begin(), words.end(), 1), common::WrongDimension);

    BOOST_TEST(net.count() == 0);
}

BOOST_AUTO_TEST_CASE(shouldThrowAnExceptionOnAppendingNetsWithDifferentDimensions) {
    common::Markov net(2);

    BOOST_CHECK_THROW(net.append(getNet()), common::WrongDimension);

    BOOST_TEST(net.count() == 0);
}

BOOST_AUTO_TEST_CASE(shouldAppendTheSecondNet) {
    common::Markov net = getNet();

    std::vector<std::string> words = { "A", "B", "C" };
    net.append(words.begin(), words.end(), 1);

    BOOST_TEST(net.count() == 1);
    BOOST_TEST(net[L"a"].count == 1);
    BOOST_TEST(net[L"a"][L"b"].count == 1);
    BOOST_TEST(net[L"a"][L"b"][L"c"].count == 1);

    net.append(getFilledNet());

    BOOST_TEST(net.count() == 4);
    BOOST_TEST(net[L"a"].count == 3);
    BOOST_TEST(net[L"a"][L"b"].count == 3);
    BOOST_TEST(net[L"a"][L"b"][L"c"].count == 2);
    BOOST_TEST(net[L"a"][L"b"][L"d"].count == 1);
    BOOST_TEST(net[L"d"].count == 1);
    BOOST_TEST(net[L"d"][L"b"].count == 1);
    BOOST_TEST(net[L"d"][L"b"][L"d"].count == 1);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(operatorSquareBrackets)

BOOST_AUTO_TEST_CASE(shouldThrowAnExceptionOnMissingKey) {
    auto net = getNet();

    std::vector<std::string> words = { "A", "B", "C" };
    net.append(words.begin(), words.end(), 1);

    BOOST_CHECK_THROW(net[L"d"], common::UnexistentNode);
}

BOOST_AUTO_TEST_CASE(shouldThrowAnExceptionOnNonlowerString) {
    auto net = getNet();

    std::vector<std::string> words = { "A", "B", "C" };
    net.append(words.begin(), words.end(), 1);

    BOOST_CHECK_NO_THROW(net[L"a"]);
    BOOST_CHECK_THROW(net[L"A"], common::UnexistentNode);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(operatorWiostream)

BOOST_AUTO_TEST_CASE(shouldSerializeAndDeserializeNet) {
    auto net = getFilledNet();
    auto newNet = getNet();

    std::wstringstream stream;
    stream << net;
    stream >> newNet;

    compateNet(net);
    compateNet(newNet);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(complete)

BOOST_AUTO_TEST_CASE(shouldCompleteWithTheMostFrequentCase) {
    auto net = getFilledNet();

    std::list<std::wstring> expected{L"a", L"b", L"c"};

    BOOST_TEST(net.complete({ L"a" }) == expected);
}

BOOST_AUTO_TEST_CASE(shouldCompleteWithTheLeastAndTheMostFrequentCase) {
    auto net = getFilledNet();


    BOOST_TEST(L"c" > L"a");


    std::list<std::wstring> expected{L"a", L"b", L"c"};
    BOOST_TEST(net.complete({ L"a" }) == expected);


    std::list<std::wstring> addition = {L"a", L"b", L"a"};
    net.append(addition.begin(), addition.end(), 1);
    expected = {L"a", L"b", L"a"};
    BOOST_TEST(net.complete({ L"a" }) == expected);
}

BOOST_AUTO_TEST_CASE(shouldThrowAnExceptionOnWrongDimension) {
    BOOST_CHECK_THROW(getFilledNet().complete({L"", L"", L"", L""}), common::WrongDimension);
}

BOOST_AUTO_TEST_CASE(shouldReturnShortListIfTheSequenceCannotBeCompleted) {
    BOOST_TEST(getFilledNet().complete({L"a", L"z"}).size() == 1);
}

BOOST_AUTO_TEST_SUITE_END()
