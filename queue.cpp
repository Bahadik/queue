#include <iostream>
#include <fstream>
#include <string>
#include <memory>

struct my_queue_empty : std::exception {
	const char* what() const noexcept override {
		return "queue is empty!";
	}
};

template<class T = int, class Alloc = std::allocator<int>>
class my_queue {
private:
	struct node {
		T value;
		node* next;
		
		node(T value_, node* next_ = nullptr) :
			value(value_), 
			next(next_) {}

		node& operator=(const node& rhs) {
            value = rhs.value;
            next = rhs.next;

            return *this;
		}
	};
	
	node* head_;
	node* tail_;

	using NodeAllocator = typename Alloc::template rebind<node>::other;

public:
	using value_type = T;
	using value_type_ref = T&;

	my_queue() :
		head_(nullptr),
		tail_(nullptr) {}

	my_queue(const my_queue& rhs) :
		head_(nullptr),
		tail_(nullptr) {
		node* current_node = rhs.head_;
		while (current_node != rhs.tail_) {
			push(current_node->value);
			current_node = current_node->next;
		}
		if (current_node) {
			push(current_node->value);
		}
	}

	my_queue& operator=(my_queue rhs) {
		std::swap(head_, rhs.head_);
		std::swap(tail_, rhs.tail_);
		
    	return *this;
	}
	
	void push(const T& value) {
		NodeAllocator allocator;
		if (empty()) {
			head_ = allocator.allocate(1);
			allocator.construct(head_, value);
			tail_ = head_;
		} else {
			typename NodeAllocator::pointer new_element = allocator.allocate(1);
			allocator.construct(new_element, value);
			tail_->next = new_element;
			tail_ = new_element;
		}
	}


	void pop() {
		if (!empty()) {
			node* next_node = head_->next;
			delete head_;
			head_ = next_node;
		} else {
		    throw(my_queue_empty());
        }
	}

	T& top() const {
		if (head_) {
			return head_->value;
		} else {
      	    throw (my_queue_empty());
        }
	}

	bool empty() const noexcept {
		return !head_;
	}

	~my_queue() {
	  	node* current_node = head_;
	  	while (current_node) {
	   		node* next_node = current_node->next;
	    	delete current_node;
	    	current_node = next_node;
      	}
    }
  
    class iterator {
	private:
	    node* host_;
    public:
    	iterator(node* host) : host_(host) {};
           
	    iterator operator++() {
	      	host_ = host_->next;
	      	return *this;
	    }
       
	    bool operator==(const iterator& rhs) {
	     	return host_ == rhs.host_;
	    }
       
	    bool operator!=(const iterator& rhs) {
	      	return host_ != rhs.host_;
	    }
	       
	    node* operator*() {
	      	return host_;
	    }
	};

  	iterator begin() {
    	return iterator(head_);
 	}
    
  	iterator end() {
    	return iterator(nullptr);
  	}
};

template<class T, class Alloc>
std::ostream& operator<<(std::ostream& os, my_queue<T, Alloc> q) {
	while (!q.empty()) {
		os << q.top() << std::endl;
		q.pop();
	}
	return os;
}

template<class T, class Alloc>
std::istream& operator>>(std::istream& is, my_queue<T, Alloc>& q) {
	T temp;
	std::string s;
	while (!is.eof()) {
		is >> temp;
		if (!is.fail()) {
			q.push(temp);
		} else {
			is.clear();
			is >> s;
		}
	}
	return is;
}

int main(int argc, const char* argv[]) {
	my_queue<> queue;
	try {
		std::cout << queue.top() << std::endl;
	} catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	std::string filename_in, filename_out;
	if (argc > 2) {
		filename_in = argv[1];
		filename_out = argv[2];
	} else {
		std::cin >> filename_in;
		std::cin >> filename_out;
	}
	
	std::ifstream ifile(filename_in.c_str());
	ifile >> queue;
	
	my_queue<> queue_temp = queue;
	std::ofstream ofile(filename_out.c_str());
	ofile << queue_temp;
	return 0;
}
