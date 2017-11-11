#include <functional>
#include <iostream>
#include <fstream>
#include <memory>

#include <builder/service.h>


int realMain(int argc, char** argv) {
    builder::Configuration conf;
    std::vector<std::string> urls;
    bool helpWasRequested = false;
    std::tie(conf, urls, helpWasRequested) = builder::parse(argc, argv);

    logging::init(conf.level);

    BOOST_LOG_TRIVIAL(info) << "parallelTasks=" << conf.parallelTasks
                            << ", threads=" << conf.threads
                            << ", filename=" << conf.filename
                            << ", dimension=" << conf.dimension
                            << ", level=" << conf.level
                            << ", urls=" << boost::algorithm::join(urls, " ");

    if (helpWasRequested) {
        return EXIT_SUCCESS;
    }

    auto result = builder::Service(conf).makeMarkov(std::move(urls));

    if (result.success) {
        common::Markov uniteNet(conf.dimension);

        std::for_each(std::make_move_iterator(result.result.begin()),
                      std::make_move_iterator(result.result.end()),
                      [&] (auto&& n) { uniteNet.append(std::move(n.net)); });

        std::wostream* out;
        std::wofstream fout;
        if (!conf.filename.empty()) {
            fout.open(conf.filename);
            out = &fout;
        } else {
            out = &std::wcout;
        }

        (*out) << uniteNet;

        return EXIT_SUCCESS;
    } else {
        BOOST_LOG_TRIVIAL(error) << "net was not saved";
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
