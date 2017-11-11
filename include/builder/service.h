#pragma once

#include <common/log.h>

#include <builder/downloader.h>
#include <builder/conf.h>

#include <boost/asio/io_service.hpp>


namespace builder {

class Service {
    bool success_;
    std::size_t runningTasks_;

    const Configuration& conf_;
    boost::asio::io_service ios_;
    std::list<std::string> urls_;

    std::vector<DownloaderPtr> tasks_;
    std::recursive_mutex mutex_;

    void onExit(bool isChildWasSuccess);
    bool spawn();

public:
    struct Result {
        struct SingleResult {
            std::string url;
            common::Markov net;
        };

        std::vector<SingleResult> result;
        bool success = true;
    };

    Service(const Configuration& conf)
        : success_(true)
        , runningTasks_(0)
        , conf_(conf)
    { }

    ~Service();

    Result makeMarkov(std::vector<std::string>&& urls);
};

}



