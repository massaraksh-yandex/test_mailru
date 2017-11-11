#include <builder/service.h>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/thread.hpp>


namespace builder {

Service::~Service() {
    ios_.stop();
}

void Service::onExit(bool isChildWasSuccess) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    success_ = success_ && isChildWasSuccess;
    runningTasks_--;
    while(spawn());
}

bool Service::spawn() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!success_ || urls_.empty() || runningTasks_ >= conf_.parallelTasks) {
        return false;
    }

    runningTasks_++;
    tasks_.push_back(std::make_shared<Downloader>(conf_.dimension,
                                                 std::bind(&Service::onExit, this, std::placeholders::_1),
                                                 ios_));

    std::string url = std::move(urls_.front());
    urls_.pop_front();
    tasks_.back()->start(std::move(url));

    return true;
}

Service::Result Service::makeMarkov(std::vector<std::string>&& urls) {
    urls_ = std::list<std::string>(std::make_move_iterator(urls.begin()),
                                   std::make_move_iterator(urls.end()));
    success_ = true;

    while(spawn());

    boost::thread_group threads;
    for(std::size_t i = 0; i < conf_.threads; i++) {
        threads.create_thread(boost::bind(&boost::asio::io_service::run, &ios_));
    }

    threads.join_all();
    ios_.stop();

    Service::Result ret;
    ret.success = success_;

    if (success_) {
        ret.result.reserve(tasks_.size());
        boost::copy(tasks_
                    | boost::adaptors::transformed([](auto& task) {
                        return Result::SingleResult{task->url(), std::move(task->net())};
                    })
                    , std::back_inserter(ret.result))
        ;
    }

    return ret;
}

}
