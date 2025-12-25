#pragma once
#include <map>
#include <string>
#include <unordered_map>

namespace jsonc::detail {

struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
    size_t operator()(const std::string& s) const noexcept { return std::hash<std::string>{}(s); }
    size_t operator()(const char* s) const noexcept { return std::hash<std::string_view>{}(s); }
};

struct StringEqual {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
};

template <typename T>
using StringHashMap = std::unordered_map<std::string, T, StringHash, StringEqual>;

template <typename T>
class OrderedStringHashMap : public StringHashMap<T> {
    std::map<size_t, std::string> mInsertIndex{};
    detail::StringHashMap<size_t> mKeyIndex{};
    size_t                        mNextInsertIndex{};

public:
    template <bool _Const, bool _Reverse>
    class _Iterator {
    public:
        using reference = std::conditional_t<_Const, const std::pair<const std::string, T>, std::pair<const std::string, T>>&;
        using pointer   = std::add_pointer_t<reference>;

    public:
        [[nodiscard]] reference operator*() const noexcept {
            auto result = mStorage.find(mIterator->second);
            if (result != mStorage.end()) { return *result; }
            std::unreachable();
        }
        [[nodiscard]] pointer operator->() const noexcept { return std::addressof(**this); }

        _Iterator& operator++() noexcept {
            ++mIterator;
            return *this;
        }
        _Iterator operator++(int) noexcept {
            _Iterator tmp = *this;
            ++*this;
            return tmp;
        }
        _Iterator& operator--() noexcept {
            --mIterator;
            return *this;
        }
        _Iterator operator--(int) noexcept {
            _Iterator tmp = *this;
            --*this;
            return tmp;
        }

        [[nodiscard]] bool operator==(_Iterator const& rhs) const noexcept { return mIterator == rhs.mIterator; }

    private:
        friend class OrderedStringHashMap;
        using StorageType  = std::conditional_t<_Const, const detail::StringHashMap<T>, detail::StringHashMap<T>>;
        using IteratorType = std::conditional_t<
            _Reverse,
            std::conditional_t<_Const, std::map<size_t, std::string>::const_reverse_iterator, std::map<size_t, std::string>::reverse_iterator>,
            std::conditional_t<_Const, std::map<size_t, std::string>::const_iterator, std::map<size_t, std::string>::iterator>>;

        StorageType& mStorage;
        IteratorType mIterator;

        explicit _Iterator(StorageType& storage, const IteratorType& iterator) : mStorage(storage), mIterator(iterator) {}
    };

public:
    using iterator               = _Iterator<false, false>;
    using const_iterator         = _Iterator<true, false>;
    using reverse_iterator       = _Iterator<false, true>;
    using const_reverse_iterator = _Iterator<true, true>;

public:
    iterator begin() noexcept { return iterator(*this, mInsertIndex.begin()); }
    iterator end() noexcept { return iterator(*this, mInsertIndex.end()); }

    const_iterator begin() const noexcept { return const_iterator(*this, mInsertIndex.begin()); }
    const_iterator end() const noexcept { return const_iterator(*this, mInsertIndex.end()); }

    const_iterator cbegin() const noexcept { return const_iterator(*this, mInsertIndex.begin()); }
    const_iterator cend() const noexcept { return const_iterator(*this, mInsertIndex.end()); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(*this, mInsertIndex.rbegin()); }
    reverse_iterator rend() noexcept { return reverse_iterator(*this, mInsertIndex.rend()); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(*this, mInsertIndex.rbegin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(*this, mInsertIndex.rend()); }

    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(*this, mInsertIndex.crbegin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(*this, mInsertIndex.crend()); }

    const_iterator find(std::string_view _Keyval) const {
        auto _Result = mKeyIndex.find(_Keyval);
        if (_Result != mKeyIndex.end()) { return const_iterator(*this, mInsertIndex.find(_Result->second)); }
        return const_iterator(*this, mInsertIndex.end());
    }
    iterator find(std::string_view _Keyval) {
        auto _Result = mKeyIndex.find(_Keyval);
        if (_Result != mKeyIndex.end()) { return iterator(*this, mInsertIndex.find(_Result->second)); }
        return iterator(*this, mInsertIndex.end());
    }

    template <class... _Mappedty>
    std::pair<iterator, bool> try_emplace(std::string_view _Keyval, _Mappedty&&... _Mapval) {
        const auto _Result = StringHashMap<T>::try_emplace(std::string(_Keyval), std::forward<_Mappedty>(_Mapval)...);
        auto       _Iter   = mInsertIndex.try_emplace(mNextInsertIndex, _Keyval);
        mKeyIndex.try_emplace(std::string(_Keyval), mNextInsertIndex);
        mNextInsertIndex++;
        return {iterator(*this, _Iter.first), _Result.second};
    }
};

} // namespace jsonc::detail