#include <functional>
#include <iostream>
#include <fstream>
#include <memory>

#include <common/markov.h>
#include <common/log.h>

#include <complete/conf.h>

#include <boost/algorithm/string/join.hpp>


int realMain(int argc, char** argv) {
    complete::Configuration conf;
    bool helpWasRequested = false;
    std::tie(conf, helpWasRequested) = complete::parse(argc, argv);

    logging::init(conf.level);

    BOOST_LOG_TRIVIAL(info) << "filename=" << conf.filename
                            << ", words=" << boost::algorithm::join(conf.words, " ")
                            << ", count=" << conf.count
                            << ", dimension=" << conf.dimension
                            << ", level=" << conf.level;

    if (helpWasRequested) {
        return EXIT_SUCCESS;
    }

    common::Markov net(conf.dimension);

    std::wifstream in(conf.filename);

    if (in.is_open()) {
        in >> net;

        std::list<std::wstring> params;
        std::transform(conf.words.begin(), conf.words.end(), std::back_inserter(params), [] (auto&& str) {
            return common::toLowerWstring(str);
        });


        std::size_t count = 0;
        std::wcout << boost::algorithm::join(params, L" ");
        do {
            params = net.complete(std::move(params));

            if (params.size() == conf.dimension) {
                std::wcout << L" " << params.back();
                params.pop_front();
            } else {
                break;
            }
        } while(++count < conf.count);
        std::wcout << std::endl;

        return EXIT_SUCCESS;
    } else {
        BOOST_LOG_TRIVIAL(error) << "cannot open file=" << conf.filename;
        return EXIT_FAILURE;
    }
}

int main(int argc, char** argv) {
    try {
        return realMain(argc, argv);
    } catch(const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
    } catch(...) {
        BOOST_LOG_TRIVIAL(error) << "unknown expection";
    }

    return EXIT_FAILURE;
}
