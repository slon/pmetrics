#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <map>

namespace pm {

struct tree_printer_t {
	virtual void start_node() = 0;
	virtual void end_node() = 0;

	virtual void child(const std::string& name) = 0;
	virtual void value(double value) = 0;
	virtual void value(int64_t value) = 0;
	virtual void value(uint64_t value_) { value (int64_t(value_)); };

	virtual std::string result() const = 0;

	virtual ~tree_printer_t() {}
};

struct tree_leaf_t {
	virtual void print(tree_printer_t* printer) = 0;
};

class tree_branch_t {
public:
	void print(tree_printer_t* printer);
	bool remove_empty_nodes();

	std::shared_ptr<tree_branch_t> get_branch(const std::string& name);
	void add_leaf(const std::string& name, std::weak_ptr<tree_leaf_t> leaf);

private:
	struct child_ptr_t {
		std::weak_ptr<tree_leaf_t> leaf;
		std::shared_ptr<tree_branch_t> branch;
	};

	std::mutex mutex_;
	std::map<std::string, child_ptr_t> childs_;
};

} // namespace pm
