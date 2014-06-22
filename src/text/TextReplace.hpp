#pragma once

#include <string>
#include <map>

namespace antifurto {
namespace text {

/// This class allows to replace a set of variables in a text with the
/// corresponding values
class TextReplace
{
public:
    TextReplace(std::string varPrefix = "${", std::string varSuffix = "}");
    void addVariable(const std::string& var, std::string value);
    std::string replaceVariables(const std::string& input);

private:
    std::string varPrefix_;
    std::string varSuffix_;
    std::map<std::string, std::string> varMap_;
};

} // namespace text
} // namespace antifurto
