template <typename T> struct ObjectMap {
    ObjectMap() = default;

    std::shared_ptr<T> at(uint64_t ptr) {
        std::lock_guard<std::mutex> guard(this->mutex);

        return this->map.at(ptr);
    }

    uint64_t insert(std::shared_ptr<T> impl) {
        std::lock_guard<std::mutex> guard(this->mutex);

        this->map.insert({ (uint64_t)impl.get(), impl });

        return (uint64_t)impl.get();
    }

    void erase(uint64_t ptr) {
        std::lock_guard<std::mutex> guard(this->mutex);

        if (this->map.find(ptr) != this->map.end()) {
            if (this->map.at(ptr).use_count() == 1) {
                this->map.erase(ptr);
            }
        }
    }
    private:
        ObjectMap(const ObjectMap<T> &) = delete;
        ObjectMap(ObjectMap<T> &&) = delete;

        ObjectMap<T> &operator=(const ObjectMap<T> &) = delete;
        ObjectMap<T> &operator=(ObjectMap<T> &&) = delete;

        std::mutex mutex;
        std::map<uint64_t, std::shared_ptr<T>> map;
};
