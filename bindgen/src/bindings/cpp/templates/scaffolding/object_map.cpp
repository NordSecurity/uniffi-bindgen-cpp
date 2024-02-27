template <typename T> struct ObjectMap {
    ObjectMap() = default;

    std::shared_ptr<T> at(uint64_t ptr) {
        std::lock_guard<std::mutex> guard(this->mutex);

        return this->map.at(ptr).ptr;
    }

    uint64_t insert(std::shared_ptr<T> impl) {
        std::lock_guard<std::mutex> guard(this->mutex);
        auto key = (uint64_t)impl.get();

        if (this->map.contains(key)) {
            this->map.at(key).ref_count += 1;
        } else {
            this->map.insert({ key, {impl, 1} });
        }

        return key;
    }

    void erase(uint64_t ptr) {
        std::lock_guard<std::mutex> guard(this->mutex);

        auto &wrapper = this->map.at(ptr);
        wrapper.ref_count -= 1;

        if (wrapper.ref_count == 0) {
            this->map.erase(ptr);
        }
    }
    private:
        ObjectMap(const ObjectMap<T> &) = delete;
        ObjectMap(ObjectMap<T> &&) = delete;

        ObjectMap<T> &operator=(const ObjectMap<T> &) = delete;
        ObjectMap<T> &operator=(ObjectMap<T> &&) = delete;

        struct PtrWrapper {
            std::shared_ptr<T> ptr = nullptr;
            uint64_t ref_count = 0;
        };

        std::mutex mutex;
        std::map<uint64_t, PtrWrapper> map;
};
