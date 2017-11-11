#pragma once

#include <tuple>
#include <list>

#include <common/conf.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>


namespace builder {

struct Configuration: public common::Configuration {
    std::size_t parallelTasks = 0;
    std::size_t threads = 0;
};

inline std::tuple<Configuration, std::vector<std::string>, bool> parse(int argc, char** argv) {
    namespace po = boost::program_options;
    Configuration ret;
    std::vector<std::string> urls;

    po::options_description desc("Building markov net");
    auto options = desc.add_options();

    common::addOptions(options, ret);
    options
            ("parallel,p", po::value<std::size_t>(&ret.parallelTasks)->default_value(4),
                "number of curl subprocesses to run at the same time")
            ("threads,t", po::value<std::size_t>(&ret.threads)->default_value(4),
                "number of working threads")
            ("urls,u", po::value<std::vector<std::string>>(&urls)->multitoken()->required(),
                "urls to download")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    const bool helpRequested = vm.count("help");

    if (helpRequested) {
        std::cout << desc << std::endl;
    } else {
        po::notify(vm);
    }

    return std::make_tuple(ret, urls, helpRequested);
}

}
