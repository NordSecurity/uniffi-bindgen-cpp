{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

template<typename T> struct HandleMap {
    HandleMap() = default;

    std::shared_ptr<T> at(uint64_t handle) {
        std::lock_guard<std::mutex> guard(this->mutex);

        return this->map.at(handle);
    }

    uint64_t insert(std::shared_ptr<T> impl) {
        std::lock_guard<std::mutex> guard(this->mutex);

        auto handle = this->cur_handle;

        this->map.insert({ handle, impl });
        this->cur_handle += 1;

        return handle;
    }

    void erase(uint64_t handle) {
        std::lock_guard<std::mutex> guard(this->mutex);

        this->map.erase(handle);
    }
private:
    HandleMap(const HandleMap<T> &) = delete;
    HandleMap(HandleMap<T> &&) = delete;

    HandleMap<T> &operator=(const HandleMap<T> &) = delete;
    HandleMap<T> &operator=(HandleMap<T> &&) = delete;

    std::mutex mutex;
    uint64_t cur_handle = 0;
    std::map<uint64_t, std::shared_ptr<T>> map;
};

struct {{ ffi_converter_name|class_name }} {
    static std::shared_ptr<{{ class_name }}> lift(uint64_t);
    static uint64_t lower(std::shared_ptr<{{ class_name }}>);
    static std::shared_ptr<{{ class_name }}> read(uniffi::RustStream &);
    static void write(RustStream &, uint64_t);

    static int32_t allocation_size(const {{ class_name }} &);

private:
    static int callback_stub(uint64_t, uint32_t, uint8_t *, int32_t, RustBuffer *);

    static std::once_flag once;
    static HandleMap<{{ class_name }}> callbacks;
};
