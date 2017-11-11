#pragma once

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <boost/system/error_code.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>


namespace logging {

inline void init(boost::log::trivial::severity_level level) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
}

inline void error(const std::string& file, int line, const std::string& what) {
    BOOST_LOG_TRIVIAL(error) << file << ":" << line << " " << what;
}

inline void error(const std::string& file, int line, boost::system::error_code ec) {
    BOOST_LOG_TRIVIAL(error) << file << ":" << line << " " << ec.category().name() << " " << ec.message();
}

inline void error(const std::string& file, int line, std::error_code ec) {
    BOOST_LOG_TRIVIAL(error) << file << ":" << line << " " << ec.category().name() << " " << ec.message();
}

}
