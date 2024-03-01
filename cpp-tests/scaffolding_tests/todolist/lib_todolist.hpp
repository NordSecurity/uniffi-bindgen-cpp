#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <mutex>

namespace {
    namespace todolist {
        class TodoError: public std::runtime_error {
        public:
            TodoError() : std::runtime_error("") {}
            TodoError(const std::string &what_arg) : std::runtime_error(what_arg) {}
        };

        namespace todo_error {
            class TodoDoesNotExist: public TodoError {
            public:
                TodoDoesNotExist() : TodoError("") {}
                TodoDoesNotExist(const std::string &what_arg) : TodoError(what_arg) {}
            };

            class EmptyTodoList: public TodoError {
            public:
                EmptyTodoList() : TodoError("") {}
                EmptyTodoList(const std::string &what_arg) : TodoError(what_arg) {}
            };

            class DuplicateTodo: public TodoError {
            public:
                DuplicateTodo() : TodoError("") {}
                DuplicateTodo(const std::string &what_arg) : TodoError(what_arg) {}
            };

            class EmptyString: public TodoError {
            public:
                EmptyString() : TodoError("") {}
                EmptyString(const std::string &what_arg) : TodoError(what_arg) {}
            };

            class DeligatedError: public TodoError {
            public:
                DeligatedError() : TodoError("") {}
                DeligatedError(const std::string &what_arg) : TodoError(what_arg) {}
            };
        }

        struct TodoEntry {
            std::string text;
        };

        class TodoList {
        public:
            TodoList() = default;

            void add_item(const std::string &item);
            void add_entry(const TodoEntry &entry);
            std::vector<TodoEntry> get_entries();
            std::vector<std::string> get_items();
            void add_entries(const std::vector<TodoEntry> &entries);
            void add_items(const std::vector<std::string> &items);
            TodoEntry get_last_entry();
            std::string get_last();
            std::string get_first();
            void clear_item(const std::string &item);
            void make_default(const std::shared_ptr<TodoList> &self);
        private:
            std::vector<std::string> items;
            std::mutex items_mutex;
        };

        std::shared_ptr<TodoList> get_default_list();
        void set_default_list(std::shared_ptr<TodoList> list);

        TodoEntry create_entry_with(const std::string &text);

        static std::shared_ptr<TodoList> default_list = nullptr;
        static std::mutex default_list_mutex;
    }
}
