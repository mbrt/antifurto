#pragma once

#include <string>
#include <map>
#include <iosfwd>

namespace antifurto {
namespace text {

/// This class allows to replace a set of variables in a text with the
/// corresponding values
class TextReplace
{
public:
    TextReplace(std::string varPrefix = "${", std::string varSuffix = "}");
    void addVariable(const std::string& var, std::string value);
    std::string replaceVariables(const std::string& input) const;
    void replaceVariables(std::istream& in, std::ostream& out) const;

private:
    std::string varPrefix_;
    std::string varSuffix_;
    std::map<std::string, std::string> varMap_;
};

/// Read the whole stream and returns a string of its contents
std::string readContents(std::istream& in);

} // namespace text
} // namespace antifurto
