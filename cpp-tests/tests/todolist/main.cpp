#include <test_common.hpp>

#include <todolist.hpp>

int main() {
    auto todo = todolist::TodoList::init();

    EXPECT_EXCEPTION(todo->get_last(), todolist::TodoError);
    EXPECT_EXCEPTION(todolist::create_entry_with(""), todolist::EmptyString);

    todo->add_item("Write support");
    ASSERT_EQ("Write support", todo->get_last());

    todo->add_item("Write test 2");
    ASSERT_EQ("Write test 2", todo->get_last());

    auto entry = todolist::create_entry_with("Write tests 3");
    todo->add_entry(entry);
    ASSERT_EQ("Write test 3", todo->get_last());
    ASSERT_EQ("Write test 3", todo->get_last_entry().text);

    todo->add_item("Test Ünicode hàndling without an entry 🤣");
    ASSERT_EQ("Test Ünicode hàndling without an entry 🤣", todo->get_last());

    auto entry2 = todolist::create_entry_with("Test Ünicode hàndling with an entry 🤣");
    todo->add_entry(entry2);
    ASSERT_EQ("Test Ünicode hàndling with an entry 🤣", todo->get_last());

    ASSERT_EQ(5, todo->get_entries().size());

    todo->add_entries({todolist::create_entry_with("Item"), todolist::create_entry_with("Item2")});
    ASSERT_EQ(7, todo->get_entries().size());
    ASSERT_EQ("Item2", todo->get_last_entry().text);

    todo->add_items({"Item3", "Item4"});
    ASSERT_EQ(9, todo->get_entries().size());
    ASSERT_EQ("Item3", todo->get_items()[7]);

    ASSERT_EQ(std::nullopt, todolist::get_default_list());


    return 0;
}