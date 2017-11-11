#pragma once

#include <string>

#include <common/log.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>


namespace common {

struct Configuration {
    std::string filename;
    std::size_t dimension = 0;
    boost::log::trivial::severity_level level;

    static constexpr std::size_t MAX_DIMENSION = 10;
    static constexpr std::size_t MIN_DIMENSION = 2;
};

inline void addOptions(boost::program_options::options_description_easy_init& options, Configuration& conf) {
    namespace po = boost::program_options;

    const std::string helperMessage = std::string("dimension of markov net\n") +
            std::string("must be in interval [")+std::to_string(Configuration::MIN_DIMENSION)+"; "+
            std::to_string(Configuration::MAX_DIMENSION)+"]";

    options
            ("help", "show this message")
            ("file,f", po::value<std::string>(&conf.filename),
                "file with Markov net")
            ("dimension,n", po::value<std::size_t>(&conf.dimension)
                ->default_value(2)
                ->notifier([](std::size_t val) {
                    if (val < Configuration::MIN_DIMENSION || val > Configuration::MAX_DIMENSION ) {
                        throw po::validation_error(po::validation_error::invalid_option_value,
                                                   "dimension", std::to_string(val));
                    }
                }),
                helperMessage.c_str())
            ("severity", po::value<std::string>()
                ->default_value("info")
                ->notifier([&conf] (const std::string& val) {
                    if (val == "debug") {
                        conf.level = boost::log::trivial::debug;
                    } else if (val == "info") {
                        conf.level = boost::log::trivial::info;
                    } else if (val == "error") {
                        conf.level = boost::log::trivial::error;
                    } else {
                        throw po::validation_error(po::validation_error::invalid_option_value, "verbose", val);
                    }
                }),
                "logging level, must be (debug|info|error)")
    ;
}

}
