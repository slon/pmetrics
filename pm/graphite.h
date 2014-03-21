#pragma once

#include <pm/tree.h>

namespace pm {

class graphite_printer_t : public tree_printer_t {
public:
    graphite_printer_t(const std::string& prefix);
    graphite_printer_t(const std::string& prefix, int64_t timestamp);

    virtual void start_node();
    virtual void end_node();

    virtual void child(const std::string& name);
    virtual void value(double value);
    virtual void value(int64_t value);

    virtual std::string result() const;

private:
    std::vector<std::string> path_;
    int64_t timestamp_;

    std::stringstream result_;
};

}  // namespace pm
