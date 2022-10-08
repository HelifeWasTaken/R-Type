#include "RServer/async_queue.hpp"

namespace rtype {
namespace net {

    template <typename T> class async_automated_sparse_array {
    private:
        std::mutex _mut;
        async_queue<size_t> _unused_indexes;
        std::vector<boost::shared_ptr<T>> _array;

    public:
        HL_AUTO_COMPLETE_CANONICAL_FORM(async_automated_sparse_array);

        boost::shared_ptr<T> async_get(size_t index)
        {
            std::lock_guard<std::mutex> lock(this->_mut);
            if (index >= this->_array.size())
                return nullptr;
            return this->_array.at(index);
        }

        size_t async_set(boost::shared_ptr<T> value)
        {
            std::lock_guard<std::mutex> lock(this->_mut);
            size_t index;

            if (this->_unused_indexes.async_pop(index)) {
                this->_array.at(index) = value;
                return index;
            }
            this->_array.push_back(value);
            return this->_array.size() - 1;
        }

        void async_remove(size_t index)
        {
            std::lock_guard<std::mutex> lock(_mut);
            if (index >= this->_array.size())
                return;
            this->_array.at(index) = nullptr;
            this->_unused_indexes.async_push(index);
        }

        // TODO: Optimize size by avoiding unused indexes
        size_t async_size()
        {
            std::lock_guard<std::mutex> lock(this->_mut);
            return this->_array.size();
        }
    };

}
}