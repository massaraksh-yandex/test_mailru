#include <builder/downloader.h>
#include <builder/markov.h>
#include <common/log.h>

#include <boost/asio/read.hpp>
#include <boost/process.hpp>

#include <functional>
#include <iostream>
#include <list>


namespace builder {

Downloader::Downloader(std::size_t dimension, OnExit callbackOnExit, boost::asio::io_service& ios)
    : work_(std::make_shared<boost::asio::io_service::work>(ios))
    , pipe_(work_->get_io_service())
    , callbackOnExit_(callbackOnExit)
    , buffer_(bufferSize, ' ')
    , net_(dimension)
{

}

void Downloader::start(const std::string& url) {
    url_ = url;

    auto exit = std::bind(&Downloader::onExit, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
    try {
        curl_ = boost::process::child(boost::process::search_path("curl"),
                                      "-s",
                                      url_,
                                      work_->get_io_service(),
                                      boost::process::std_out > pipe_,
                                      boost::process::on_exit(exit));

        fetch();
    } catch (const std::exception& ex) {
        logging::error(__FILE__, __LINE__, ex.what());
    } catch(...) {
        logging::error(__FILE__, __LINE__, "unknown error");
    }
}

void Downloader::fetch() {
    boost::asio::async_read(pipe_, boost::asio::buffer(buffer_, bufferSize),
                            [self = this->shared_from_this()] (boost::system::error_code ec, std::size_t length) {

        BOOST_LOG_TRIVIAL(debug) << __PRETTY_FUNCTION__
                                 << "url: " << self->url_
                                 << " length: " << length
                                 << " ec: " << ec.message()
                                 << " message: " << std::string(self->buffer_.begin(), self->buffer_.begin()+length);

        const bool endOfStream = ec == boost::asio::error::eof;
        if (!ec || endOfStream) {
            if (self->readData(length, endOfStream) && !endOfStream) {
                self->fetch();
                return;
            }
        } else if (!endOfStream) {
            logging::error(__FILE__, __LINE__, ec);
        }

        self->pipe_.async_close();
    });
}

void Downloader::onExit(int status, const std::error_code& ec) {
    BOOST_LOG_TRIVIAL(debug) << __PRETTY_FUNCTION__
                             << "url: " << url_
                             << " status: " << status
                             << " ec: " << ec.message();

    if (status) {
        logging::error(__FILE__, __LINE__, "curl's status code: " + std::to_string(status));
    }

    if (ec) {
        logging::error(__FILE__, __LINE__, ec);
    }

    callbackOnExit_(!status && !ec);

    work_.reset();
}

bool Downloader::readData(std::size_t length, bool lastTime) {
    try {
        if (length) {
            auto end = buffer_.begin();
            std::advance(end, length);
            data_.append(readChunk(buffer_.begin(), end, std::move(data_.lastWord)));
            data_.words = insertToMarkov(net_, std::move(data_.words));
        }

        if (lastTime) {
            if (!data_.lastWord.empty()) {
                data_.words.emplace_back(std::move(data_.lastWord));
            }

            lastInsertToMarkov(net_, std::move(data_.words));
        }

        return true;
    } catch (const std::exception& ex) {
        logging::error(__FILE__, __LINE__, ex.what());
    } catch (...) {
        logging::error(__FILE__, __LINE__, "unknown error");
    }

    return false;
}

}
