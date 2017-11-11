#pragma once

#include <string>
#include <memory>
#include <array>

#include <boost/process/async_pipe.hpp>
#include <boost/process/child.hpp>
#include <boost/asio/io_service.hpp>

#include <common/markov.h>
#include <common/log.h>

#include <builder/read_chunk.h>


namespace builder {

using OnExit = std::function<void(bool)>;

class Downloader: public std::enable_shared_from_this<Downloader> {
    static constexpr std::size_t bufferSize = 1024;

    std::shared_ptr<boost::asio::io_service::work> work_;
    boost::process::async_pipe pipe_;
    boost::process::child curl_;
    OnExit callbackOnExit_;

    std::vector<char> buffer_;
    ReadChunkResult data_;

    common::Markov net_;
    std::string url_;

    bool readData(std::size_t length, bool lastTime);
    void onExit(int, const std::error_code&);
    void fetch();
public:
    Downloader(std::size_t dimension, OnExit callbackOnExit, boost::asio::io_service& ios);

    void start(const std::string& url);

    std::string url() const {
        return url_;
    }

    common::Markov& net() {
        return net_;
    }
};

using DownloaderPtr = std::shared_ptr<Downloader>;

}
