#include "lib/header.h"

#include "glog/logging.h"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_real.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

using qi::int_;
using qi::double_;
using qi::char_;
using qi::phrase_parse;
using ascii::space;
using qi::_1;
using qi::_val;

struct Function;

using Node = boost::variant<int, boost::recursive_wrapper<Function>>;

struct Function {
  std::string name;
  std::vector<Node> children;

  Function& operator+=(const Node& node) {
    children.emplace_back(node);
    return *this;
  }

  void operator=(const std::string& name_) { name = name_; }
};

void print(int x) { LOG(INFO) << "print: " << x; }

template <typename Iterator>
struct FunctionGrammar
    : public qi::grammar<Iterator, Node(), ascii::space_type> {
  FunctionGrammar() : FunctionGrammar::base_type(expr, "expr") {
    atom = int_[_val = _1];
    functionName = +char_("a-zA-Z")[_val += _1];
    function = (functionName[_val = _1] >> "(" >> expr[_val += _1] >>
                *("," >> expr[_val += _1]) >> ")") ||
               (functionName[_val = _1] >> "()");
    expr = function[_val = _1] || atom[_val = _1];
  }

  qi::rule<Iterator, Node(), ascii::space_type> expr;
  qi::rule<Iterator, Function(), ascii::space_type> function;
  qi::rule<Iterator, std::string(), ascii::space_type> functionName;
  qi::rule<Iterator, int(), ascii::space_type> atom;
};

int eval(const Node& node) {
  int result = 0;
  switch (node.which()) {
    case 0:
      result = boost::get<int>(node);
      LOG(INFO) << OUT(result);
      break;
    case 1:
      const auto& f = boost::get<Function>(node);
      LOG(INFO) << OUT(f.name) << OUT(f.children.size());
      for (const auto& x : f.children) {
        result += eval(x);
      }
      break;
  }
  return result;
}

int main() {
  /*
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
  */

  string input("sum(sum(2, 2), 2)");
  auto iter = input.cbegin();
  auto endIter = input.cend();
  FunctionGrammar<string::const_iterator> grammar;
  Node parsedNode;
  phrase_parse(iter, endIter, grammar, space, parsedNode);
  LOG(INFO) << (iter == endIter);
  LOG(INFO) << OUT(eval(parsedNode));
  return 0;
}
