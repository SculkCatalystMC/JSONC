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
class OrderedStringHashMap {
public:
    template <bool _Const, bool _Reverse>
    class _Iterator {
    public:
        using reference = std::conditional_t<_Const, const std::pair<const std::string, T>, std::pair<const std::string, T>>&;
        using pointer   = std::add_pointer_t<reference>;

    public:
        [[nodiscard]] reference operator*() const noexcept { return *mStorage.find(mIterator->second); }
        [[nodiscard]] pointer   operator->() const noexcept { return std::addressof(**this); }

        _Iterator& operator++() noexcept {
            ++mIterator;
            return *this;
        }
        _Iterator operator++(int) noexcept {
            _Iterator _Tmp = *this;
            ++*this;
            return _Tmp;
        }
        _Iterator& operator--() noexcept {
            --mIterator;
            return *this;
        }
        _Iterator operator--(int) noexcept {
            _Iterator _Tmp = *this;
            --*this;
            return _Tmp;
        }

        [[nodiscard]] bool operator==(const _Iterator& rhs) const noexcept { return mIterator == rhs.mIterator; }

    private:
        friend class OrderedStringHashMap;
        using StorageType  = std::conditional_t<_Const, const detail::StringHashMap<T>, detail::StringHashMap<T>>;
        using IteratorType = std::conditional_t<
            _Reverse,
            std::conditional_t<_Const, std::map<size_t, std::string>::const_reverse_iterator, std::map<size_t, std::string>::reverse_iterator>,
            std::conditional_t<_Const, std::map<size_t, std::string>::const_iterator, std::map<size_t, std::string>::iterator>>;

        StorageType& mStorage;
        IteratorType mIterator;

        explicit _Iterator(StorageType& _Storage, const IteratorType& _Iter) noexcept : mStorage(_Storage), mIterator(_Iter) {}
    };

public:
    using iterator               = _Iterator<false, false>;
    using const_iterator         = _Iterator<true, false>;
    using reverse_iterator       = _Iterator<false, true>;
    using const_reverse_iterator = _Iterator<true, true>;

public:
    [[nodiscard]] OrderedStringHashMap() = default;
    [[nodiscard]] OrderedStringHashMap(std::initializer_list<std::pair<std::string, T>> val) {
        for (const auto& [k, v] : val) { try_emplace(k, v); }
    }

    [[nodiscard]] size_t size() const noexcept { return mStorage.size(); }
    [[nodiscard]] bool   empty() const noexcept { return mStorage.empty(); }

    [[nodiscard]] bool contains(std::string_view _Keyval) const noexcept { return mStorage.contains(_Keyval); }

    [[nodiscard]] bool erase(std::string_view _Keyval) noexcept {
        auto _Index = mKeyIndex.find(_Keyval);
        if (_Index != mKeyIndex.end()) {
            mKeyIndex.erase(_Index);
            mInsertIndex.erase(_Index->second);
            return mStorage.erase(std::string(_Keyval));
        }
        return false;
    }

    [[nodiscard]] iterator begin() noexcept { return iterator(mStorage, mInsertIndex.begin()); }
    [[nodiscard]] iterator end() noexcept { return iterator(mStorage, mInsertIndex.end()); }

    [[nodiscard]] const_iterator begin() const noexcept { return const_iterator(mStorage, mInsertIndex.begin()); }
    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(mStorage, mInsertIndex.end()); }

    [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(mStorage, mInsertIndex.begin()); }
    [[nodiscard]] const_iterator cend() const noexcept { return const_iterator(mStorage, mInsertIndex.end()); }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(mStorage, mInsertIndex.rbegin()); }
    [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(mStorage, mInsertIndex.rend()); }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(mStorage, mInsertIndex.rbegin()); }
    [[nodiscard]] const_reverse_iterator rend() const noexcept { return const_reverse_iterator(mStorage, mInsertIndex.rend()); }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(mStorage, mInsertIndex.crbegin()); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return const_reverse_iterator(mStorage, mInsertIndex.crend()); }

    [[nodiscard]] const_iterator find(std::string_view _Keyval) const JSONC_EXCEPTION_TYPE {
        auto _Result = mKeyIndex.find(_Keyval);
        if (_Result != mKeyIndex.end()) { return const_iterator(mStorage, mInsertIndex.find(_Result->second)); }
        return const_iterator(mStorage, mInsertIndex.end());
    }
    [[nodiscard]] iterator find(std::string_view _Keyval) JSONC_EXCEPTION_TYPE {
        auto _Result = mKeyIndex.find(_Keyval);
        if (_Result != mKeyIndex.end()) { return iterator(mStorage, mInsertIndex.find(_Result->second)); }
        return iterator(mStorage, mInsertIndex.end());
    }

    template <class... _Mappedty>
    std::pair<iterator, bool> try_emplace(std::string_view _Keyval, _Mappedty&&... _Mapval) JSONC_EXCEPTION_TYPE {
        const auto _Result = mStorage.try_emplace(std::string(_Keyval), std::forward<_Mappedty>(_Mapval)...);
        if (_Result.second) {
            auto _Iter = mInsertIndex.try_emplace(mNextInsertIndex, _Keyval);
            mKeyIndex.try_emplace(std::string(_Keyval), mNextInsertIndex);
            mNextInsertIndex++;
            return {iterator(mStorage, _Iter.first), _Result.second};
        }
        auto _Index = mKeyIndex.find(_Keyval);
        return {iterator(mStorage, mInsertIndex.find(_Index->second)), _Result.second};
    }

    void clear() noexcept {
        mStorage.clear();
        mInsertIndex.clear();
        mKeyIndex.clear();
        mNextInsertIndex = 0;
    }

    [[nodiscard]] const std::string& key_index(size_t index) const noexcept {
        auto it = std::next(mInsertIndex.begin(), static_cast<std::map<size_t, std::string>::difference_type>(index));
        if (it != mInsertIndex.end()) { return it->second; }
        std::unreachable();
    }

    [[nodiscard]] bool operator==(const OrderedStringHashMap& other) const JSONC_EXCEPTION_TYPE { return mStorage == other.mStorage; }

private:
    StringHashMap<T>              mStorage{};
    std::map<size_t, std::string> mInsertIndex{};
    detail::StringHashMap<size_t> mKeyIndex{};
    size_t                        mNextInsertIndex{};
};

} // namespace jsonc::detail