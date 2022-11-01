#pragma once

namespace paa {
template<typename T>
class Backtrace {
private:
    const std::size_t _max_backlog;

    std::vector<T> _backtrace;
    std::size_t _index = 0;

public:
    Backtrace(std::size_t max_backlog = 10)
        : _max_backlog(max_backlog)
    {
    }

    void push(const T& element)
    {
        if (_index == _backtrace.size()) {
            _backtrace.push_back(element);
            _index++;
        } else {
            _backtrace[_index] = element;
            _index++;
        }
        if (_max_backlog != 0 && _backtrace.size() > _max_backlog) {
            _backtrace.erase(_backtrace.begin());
            _index--;
        }
    }

    bool pop(T& element)
    {
        if (_index == 0) {
            return false;
        }
        element = _backtrace[_index - 1];
        _index--;
        return true;
    }

    void clear()
    {
        _backtrace.clear();
        _index = 0;
    }

    bool has_backlog() const
    {
        return _index != 0;
    }
};
}
