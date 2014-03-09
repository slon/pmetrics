#include <pm/tree.h>

#include <ctime>

namespace pm {

bool tree_branch_t::remove_empty_nodes() {
	std::lock_guard<std::mutex> guard(mutex_);

	for(auto it = childs_.begin(); it != childs_.end();) {
		auto node = it++;

		if(node->second.branch) {
			bool empty = node->second.branch->remove_empty_nodes();
			if(empty && node->second.branch.unique()) {
				childs_.erase(node);
			}
		} else if(node->second.leaf.expired()) {
			childs_.erase(node);
		}
	}

	return childs_.empty();
}

void tree_branch_t::print(tree_printer_t* printer) {
	std::lock_guard<std::mutex> guard(mutex_);

	printer->start_node();
	for(const auto& child : childs_) {
		if(child.second.branch) {
			printer->child(child.first);
			child.second.branch->print(printer);
		} else if(std::shared_ptr<tree_leaf_t> leaf = child.second.leaf.lock()) {
			printer->child(child.first);
			leaf->print(printer);
		}
	}
	printer->end_node();
}

std::shared_ptr<tree_branch_t> tree_branch_t::get_branch(const std::string& name) {
	std::lock_guard<std::mutex> guard(mutex_);

	auto& child = childs_[name];
	if(!child.branch) {
		child.leaf.reset();
		child.branch = std::make_shared<tree_branch_t>();
	}

	return child.branch;
}

void tree_branch_t::add_leaf(const std::string& name, std::weak_ptr<tree_leaf_t> leaf) {
	std::lock_guard<std::mutex> guard(mutex_);

	auto& child = childs_[name];
	if(child.branch) {
		child.branch.reset();
	}

	child.leaf = leaf;
}

} // namespace pm
