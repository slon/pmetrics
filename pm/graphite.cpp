#include <pm/graphite.h>

namespace pm {

graphite_printer_t::graphite_printer_t(const std::string& prefix)
    : path_({prefix}), timestamp_(time(NULL)) {}

graphite_printer_t::graphite_printer_t(const std::string& prefix,
                                       int64_t timestamp)
    : path_({prefix}), timestamp_(timestamp) {}

void graphite_printer_t::start_node() {}

void graphite_printer_t::end_node() { path_.pop_back(); }

void graphite_printer_t::child(const std::string& name) {
    path_.push_back(name);
}

void graphite_printer_t::value(double v) {
    bool first = true;
    for (const auto& part : path_) {
        if (!first) result_ << ".";

        result_ << part;
        first = false;
    }

    result_ << " " << v << " " << timestamp_ << "\n";
    path_.pop_back();
}

void graphite_printer_t::value(int64_t v) { value(double(v)); }

std::string graphite_printer_t::result() const { return result_.str(); }

}  // namespace pm
