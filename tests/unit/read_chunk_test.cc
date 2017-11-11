#define BOOST_TEST_MODULE read_chunk
#define BOOST_TEST_DYN_LINK


#include <boost/test/unit_test.hpp>

#include <builder/read_chunk.h>


BOOST_AUTO_TEST_SUITE(ReadChunkResult_append)

BOOST_AUTO_TEST_CASE(shouldConcatPassedWordsAndReplaceLastWord) {
    builder::ReadChunkResult chunk{{"ololo"}, "azaza"};

    chunk.append(builder::ReadChunkResult{{"word2"}, "uzuzu"});

    BOOST_TEST(chunk.lastWord == "uzuzu");
    builder::ReadChunkResult::Words expected = {"ololo", "word2"};
    BOOST_TEST(chunk.words == expected, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(readChunk)

BOOST_AUTO_TEST_CASE(shouldSplitWords) {
    const std::string data = "a b\tc\rd\ne.,/\\;:f ";
    const std::string lastWord = "";

    const auto chunk = builder::readChunk(data.begin(), data.end(), lastWord);

    const std::vector<std::string> expected = {
        "a",
        "b",
        "c",
        "d",
        "e",
        "f"
    };

    BOOST_TEST(chunk.words == expected, boost::test_tools::per_element());
    BOOST_TEST(chunk.lastWord == "");
}

BOOST_AUTO_TEST_CASE(shouldSaveWordAsLastWordInCaseOfLastSymbolsAreNotDelimiter) {
    const std::string data = "a b";
    const std::string lastWord = "";

    const auto chunk = builder::readChunk(data.begin(), data.end(), lastWord);

    const std::vector<std::string> expected = {
        "a"
    };

    BOOST_TEST(chunk.words == expected, boost::test_tools::per_element());
    BOOST_TEST(chunk.lastWord == "b");
}

BOOST_AUTO_TEST_CASE(shouldConcatLastWordAndData) {
    const std::string data = "a b";
    const std::string lastWord = "a";

    const auto chunk = builder::readChunk(data.begin(), data.end(), lastWord);

    const std::vector<std::string> expected = {
        "aa"
    };

    BOOST_TEST(chunk.words == expected, boost::test_tools::per_element());
    BOOST_TEST(chunk.lastWord == "b");
}

BOOST_AUTO_TEST_CASE(shouldDoNothingOnEmptyData) {
    const std::string data = "";
    const std::string lastWord = "a";

    const auto chunk = builder::readChunk(data.begin(), data.end(), lastWord);

    BOOST_TEST(chunk.words.empty());
    BOOST_TEST(chunk.lastWord == lastWord);
}

BOOST_AUTO_TEST_SUITE_END()
