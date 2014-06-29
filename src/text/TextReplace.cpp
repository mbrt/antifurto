#include "TextReplace.hpp"

#include <boost/xpressive/xpressive_static.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <iostream>

namespace antifurto {
namespace text {

TextReplace::TextReplace(std::string varPrefix, std::string varSuffix)
    : varPrefix_{std::move(varPrefix)}
    , varSuffix_{std::move(varSuffix)}
{ }

void TextReplace::addVariable(const std::string& var, std::string value)
{
    varMap_[varPrefix_ + var + varSuffix_] = std::move(value);
}

// see http://ericniebler.com/2010/09/27/boost-xpressive-ftw/
std::string TextReplace::replaceVariables(const std::string& input) const
{
    using namespace boost::xpressive;
    local<std::string const *> pstr;
    sregex const rx = (a1 = varMap_)[pstr = &a1];
    return regex_replace(input, rx, *pstr);
}

void TextReplace::replaceVariables(std::istream& in, std::ostream& out) const
{
    out << replaceVariables(readContents(in));
    out.flush();
}

std::string readContents(std::istream& in)
{
    in.seekg(0, std::ios::end);
    std::size_t size = in.tellg();
    std::string buffer;
    buffer.resize(size);
    in.seekg(0);
    in.read(&buffer[0], size);
    return buffer;
}

} // namespace text
} // namespace antifurto
