#include "lib/header.h"

#include "glog/logging.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
using qi::int_;
using qi::double_;
using qi::phrase_parse;
using ascii::space;
using qi::_1;

int main() {
    string input("876,1234.56");
    auto iter = input.cbegin();
    auto endIter = input.cend();
    int varI = -1;
    double varD = -1;
    phrase_parse(iter, endIter,
                 int_[boost::phoenix::ref(varI) = _1] >> ',' >>
                     double_[boost::phoenix::ref(varD) = _1],
                 space);
    LOG(INFO) << OUT(varI) << OUT(varD);

    return 0;
}
