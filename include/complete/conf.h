#pragma once

#include <vector>

#include <common/conf.h>

#include <boost/program_options/variables_map.hpp>


namespace complete {

struct Configuration: public common::Configuration {
    std::vector<std::string> words;
    std::size_t count = 0;
};

inline std::pair<Configuration, bool> parse(int argc, char** argv) {
    namespace po = boost::program_options;
    Configuration ret;

    po::options_description desc("Complete text with markov net");
    auto options = desc.add_options();
    common::addOptions(options, ret);
    options
            ("count,c", po::value<std::size_t>(&ret.count)->default_value(10),
                "print C completed words")
            ("word,w", po::value<std::vector<std::string>>(&ret.words)
                ->composing()
                ->notifier([&](const std::vector<std::string>& val) {
                     if (val.size() != ret.dimension-1) {
                         throw po::validation_error(po::validation_error::invalid_option_value,
                                                    "words size is bigger than dimension", std::to_string(val.size()));
                     }
                })
                ->required(),
                "words to complete, you should pass N-1 words")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    const bool helpRequested = vm.count("help");

    if (helpRequested) {
        std::cout << desc << std::endl;
    } else {
        po::notify(vm);
    }

    return std::make_pair(ret, helpRequested);
}

}
