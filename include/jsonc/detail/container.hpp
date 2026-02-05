#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <utility>

namespace jsonc::inline abi_v1_2_0::detail {

struct string_hash {
    using is_transparent = void;
    std::size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
    std::size_t operator()(const std::string& s) const noexcept { return std::hash<std::string>{}(s); }
    std::size_t operator()(const char* s) const noexcept { return std::hash<std::string_view>{}(s); }
};

struct string_equall {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
};

template <typename T>
using string_hash_map = std::unordered_map<std::string, T, string_hash, string_equall>;

template <typename T>
class ordered_string_hash_map {
public:
    template <bool _Const, bool _Reverse>
    class _Iterator {
    public:
        using reference = std::conditional_t<_Const, const std::pair<const std::string, T>, std::pair<const std::string, T>>&;
        using pointer   = std::add_pointer_t<reference>;

    public:
        [[nodiscard]] reference operator*() const noexcept { return *storage_.find(iterator_->second); }
        [[nodiscard]] pointer   operator->() const noexcept { return std::addressof(**this); }

        _Iterator& operator++() noexcept {
            ++iterator_;
            return *this;
        }
        _Iterator operator++(int) noexcept {
            _Iterator _Tmp = *this;
            ++*this;
            return _Tmp;
        }
        _Iterator& operator--() noexcept {
            --iterator_;
            return *this;
        }
        _Iterator operator--(int) noexcept {
            _Iterator _Tmp = *this;
            --*this;
            return _Tmp;
        }

        [[nodiscard]] bool operator==(const _Iterator& rhs) const noexcept { return iterator_ == rhs.iterator_; }

    private:
        friend class ordered_string_hash_map;
        using StorageType  = std::conditional_t<_Const, const string_hash_map<T>, string_hash_map<T>>;
        using IteratorType = std::conditional_t<
            _Reverse,
            std::conditional_t<
                _Const,
                std::map<std::size_t, std::string>::const_reverse_iterator,
                std::map<std::size_t, std::string>::reverse_iterator>,
            std::conditional_t<_Const, std::map<std::size_t, std::string>::const_iterator, std::map<std::size_t, std::string>::iterator>>;

        StorageType& storage_;
        IteratorType iterator_;

        explicit _Iterator(StorageType& _Storage, const IteratorType& _Iter) noexcept : storage_(_Storage), iterator_(_Iter) {}
    };

public:
    using iterator               = _Iterator<false, false>;
    using const_iterator         = _Iterator<true, false>;
    using reverse_iterator       = _Iterator<false, true>;
    using const_reverse_iterator = _Iterator<true, true>;

public:
    [[nodiscard]] ordered_string_hash_map() = default;
    [[nodiscard]] ordered_string_hash_map(std::initializer_list<std::pair<std::string, T>> val) {
        for (const auto& [k, v] : val) { try_emplace(k, v); }
    }

    [[nodiscard]] std::size_t size() const noexcept { return storage_.size(); }
    [[nodiscard]] bool        empty() const noexcept { return storage_.empty(); }

    [[nodiscard]] bool contains(std::string_view _Keyval) const noexcept { return storage_.contains(_Keyval); }

    [[nodiscard]] bool erase(std::string_view _Keyval) noexcept {
        auto _Index = key_index_.find(_Keyval);
        if (_Index != key_index_.end()) {
            key_index_.erase(_Index);
            insert_index_.erase(_Index->second);
            return storage_.erase(std::string(_Keyval));
        }
        return false;
    }

    [[nodiscard]] iterator begin() noexcept { return iterator(storage_, insert_index_.begin()); }
    [[nodiscard]] iterator end() noexcept { return iterator(storage_, insert_index_.end()); }

    [[nodiscard]] const_iterator begin() const noexcept { return const_iterator(storage_, insert_index_.begin()); }
    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(storage_, insert_index_.end()); }

    [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(storage_, insert_index_.begin()); }
    [[nodiscard]] const_iterator cend() const noexcept { return const_iterator(storage_, insert_index_.end()); }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(storage_, insert_index_.rbegin()); }
    [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(storage_, insert_index_.rend()); }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(storage_, insert_index_.rbegin()); }
    [[nodiscard]] const_reverse_iterator rend() const noexcept { return const_reverse_iterator(storage_, insert_index_.rend()); }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(storage_, insert_index_.crbegin()); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return const_reverse_iterator(storage_, insert_index_.crend()); }

    [[nodiscard]] const_iterator find(std::string_view _Keyval) const JSONC_EXCEPTION_TYPE {
        auto _Result = key_index_.find(_Keyval);
        if (_Result != key_index_.end()) { return const_iterator(storage_, insert_index_.find(_Result->second)); }
        return const_iterator(storage_, insert_index_.end());
    }
    [[nodiscard]] iterator find(std::string_view _Keyval) JSONC_EXCEPTION_TYPE {
        auto _Result = key_index_.find(_Keyval);
        if (_Result != key_index_.end()) { return iterator(storage_, insert_index_.find(_Result->second)); }
        return iterator(storage_, insert_index_.end());
    }

    template <typename... _Mappedty>
    std::pair<iterator, bool> try_emplace(std::string_view _Keyval, _Mappedty&&... _Mapval) JSONC_EXCEPTION_TYPE {
        const auto _Result = storage_.try_emplace(std::string(_Keyval), std::forward<_Mappedty>(_Mapval)...);
        if (_Result.second) {
            auto _Iter = insert_index_.try_emplace(next_insert_index_, _Keyval);
            key_index_.try_emplace(std::string(_Keyval), next_insert_index_);
            next_insert_index_++;
            return {iterator(storage_, _Iter.first), _Result.second};
        }
        auto _Index = key_index_.find(_Keyval);
        return {iterator(storage_, insert_index_.find(_Index->second)), _Result.second};
    }

    void clear() noexcept {
        storage_.clear();
        insert_index_.clear();
        key_index_.clear();
        next_insert_index_ = 0;
    }

    [[nodiscard]] const std::string& key_index(std::size_t index) const noexcept {
        auto it = std::next(insert_index_.begin(), static_cast<std::map<std::size_t, std::string>::difference_type>(index));
        if (it != insert_index_.end()) { return it->second; }
        std::unreachable();
    }

    [[nodiscard]] bool operator==(const ordered_string_hash_map& other) const JSONC_EXCEPTION_TYPE { return storage_ == other.storage_; }

private:
    string_hash_map<T>                 storage_{};
    std::map<std::size_t, std::string> insert_index_{};
    string_hash_map<std::size_t>       key_index_{};
    std::size_t                        next_insert_index_{};
};

} // namespace jsonc::inline abi_v1_2_0::detail