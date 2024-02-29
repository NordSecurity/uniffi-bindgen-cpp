#include "lib_todolist.hpp"

#include <algorithm>

std::shared_ptr<todolist::TodoList> todolist::get_default_list() {
    std::lock_guard<std::mutex> lock(todolist::default_list_mutex);
    return todolist::default_list;
}

void todolist::set_default_list(std::shared_ptr<todolist::TodoList> list) {
    std::lock_guard<std::mutex> lock(todolist::default_list_mutex);
    todolist::default_list = list;
}

todolist::TodoEntry todolist::create_entry_with(const std::string &todo) {
    if (todo.empty()) {
        throw todolist::todo_error::EmptyString("Cannot add empty string as entry");
    }

    return todolist::TodoEntry(todo);
}

void todolist::TodoList::add_item(const std::string &todo) {
    if (todo.empty()) {
        throw todolist::todo_error::EmptyString("Cannot add empty string as item");
    }

    std::lock_guard<std::mutex> lock(this->items_mutex);
    if (std::find(this->items.begin(), this->items.end(), todo) != this->items.end()) {
        throw todolist::todo_error::DuplicateTodo("Item already exists");
    }

    this->items.push_back(todo);
}

void todolist::TodoList::add_entry(const todolist::TodoEntry &entry) {
    this->add_item(entry.text);
}

std::vector<todolist::TodoEntry> todolist::TodoList::get_entries() {
    std::lock_guard<std::mutex> lock(this->items_mutex);
    std::vector<todolist::TodoEntry> entries;
    for (const auto &item : this->items) {
        entries.push_back(todolist::TodoEntry(item));
    }
    return entries;
}

std::vector<std::string> todolist::TodoList::get_items() {
    std::lock_guard<std::mutex> lock(this->items_mutex);
    return this->items;
}

void todolist::TodoList::add_entries(const std::vector<todolist::TodoEntry> &entries) {
    for (const auto &entry : entries) {
        this->add_entry(entry);
    }
}

void todolist::TodoList::add_items(const std::vector<std::string> &items) {
    for (const auto &item : items) {
        this->add_item(item);
    }
}

todolist::TodoEntry todolist::TodoList::get_last_entry() {
    std::lock_guard<std::mutex> lock(this->items_mutex);
    if (this->items.empty()) {
        throw todolist::todo_error::EmptyTodoList("List is empty");
    }

    return todolist::TodoEntry(this->items.back());
}

std::string todolist::TodoList::get_last() {
    return this->get_last_entry().text;
}

std::string todolist::TodoList::get_first() {
    std::lock_guard<std::mutex> lock(this->items_mutex);
    if (this->items.empty()) {
        throw todolist::todo_error::EmptyTodoList("List is empty");
    }

    return this->items.front();
}

void todolist::TodoList::clear_item(const std::string &todo) {
    std::lock_guard<std::mutex> lock(this->items_mutex);
    auto it = std::find(this->items.begin(), this->items.end(), todo);
    if (it == this->items.end()) {
        throw todolist::todo_error::TodoDoesNotExist("Item not found");
    }

    this->items.erase(it);
}

void todolist::TodoList::make_default() {
    todolist::set_default_list(this->shared_from_this());
}

#include <todolist_cpp_scaffolding.cpp>
