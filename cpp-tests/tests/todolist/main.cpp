#include <test_common.hpp>

#include <todolist.hpp>

bool compare_lists(std::vector<todolist::TodoEntry> a, std::vector<todolist::TodoEntry> b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].text.compare(b[i].text) != 0) {
            return false;
        }
    }

    return true;
}

int main() {
    auto todo = todolist::TodoList::init();

    EXPECT_EXCEPTION(todo->get_last(), todolist::TodoError);
    EXPECT_EXCEPTION(todolist::create_entry_with(""), todolist::EmptyString);

    todo->add_item("Write test");
    ASSERT_EQ("Write test", todo->get_last());

    todo->add_item("Write test 2");
    ASSERT_EQ("Write test 2", todo->get_last());

    auto entry = todolist::create_entry_with("Write test with entry");
    todo->add_entry(entry);
    ASSERT_EQ("Write test with entry", todo->get_last());
    ASSERT_EQ("Write test with entry", todo->get_last_entry().text);

    todo->add_item("Test Ünicode hàndling without an entry 🤣");
    ASSERT_EQ("Test Ünicode hàndling without an entry 🤣", todo->get_last());

    auto entry2 = todolist::create_entry_with("Test Ünicode hàndling with an entry 🤣");
    todo->add_entry(entry2);
    ASSERT_EQ("Test Ünicode hàndling with an entry 🤣", todo->get_last());
    ASSERT_EQ(5, todo->get_entries().size());

    todo->add_entries({ todolist::create_entry_with("List entry 1"), todolist::create_entry_with("List entry 2") });
    ASSERT_EQ(7, todo->get_entries().size());
    ASSERT_EQ("List entry 2", todo->get_last_entry().text);

    todo->add_items({"List 1", "List 2"});
    ASSERT_EQ(9, todo->get_entries().size());
    ASSERT_EQ("List 1", todo->get_items()[7]);

    ASSERT_EQ(std::nullopt, todolist::get_default_list());

    auto todo2 = todolist::TodoList::init();
    {
        todolist::set_default_list(*todo.get());
        auto default_list_opt = todolist::get_default_list();
        ASSERT_TRUE(default_list_opt.has_value());
        auto default_list = default_list_opt.value();

        ASSERT_TRUE(compare_lists(todo->get_entries(), default_list.get_entries()));    
        ASSERT_FALSE(compare_lists(todo2->get_entries(), default_list.get_entries()));
    }

    {
        todo2->make_default();
        auto default_list = todolist::get_default_list().value();

        ASSERT_FALSE(compare_lists(todo->get_entries(), default_list.get_entries()));
        ASSERT_TRUE(compare_lists(todo2->get_entries(), default_list.get_entries()));
    }

    todo->add_item("Entry after default list change");
    ASSERT_EQ("Entry after default list change", todo->get_last());

    todo2->add_item("New default entry");
    ASSERT_EQ("New default entry", todolist::get_default_list().value().get_last());

    return 0;
}
