#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/IntrusiveList.h"

#include <random>

struct Item :
    public Common::IntrusiveListEntry<Item>
{
    int value = 0;
};

TEST_CASE( "IntrusiveList", "[Common][IntrusiveList]" )
{
    { // Single item list
        Common::ListBuilder<Item> builder;
        Item item;
        builder.add(&item);
        Item *list = builder.build();

        REQUIRE(list == &item);
        REQUIRE(item.prev[0] == &item);
        REQUIRE(item.next[0] == &item);
    }

    Item items[100];
    Item *listItems;
    {
        int i = 0;
        Common::ListBuilder<Item> builder;
        for (Item &item :items) {
            builder.add(&item);
            item.value = i++;
        }
        listItems = builder.build();
    }

    { // check that the list was built correcly
        REQUIRE(listItems == items);
        REQUIRE(items[std::size(items)-1].next[0] == &items[0]);
        REQUIRE(items[0].prev[0] == &items[std::size(items)-1]);

        for (int i=1; i < std::size(items); ++i) {
            REQUIRE(items[i-1].next[0] == &items[i]);
            REQUIRE(items[i].prev[0] == &items[i-1]);
        }
    }

    SECTION("Unlink")
    {
        items[1].unlinkAll();

        REQUIRE(items[1].prev[0] == nullptr);
        REQUIRE(items[1].next[0] == nullptr);

        REQUIRE(items[0].next[0] == &items[2]);
        REQUIRE(items[2].prev[0] == &items[0]);
    }


    SECTION("List")
    {
        Common::IntrusiveList<Item> list;
        list.insert_back(listItems);


        REQUIRE(list.validateList());
        REQUIRE(list.size() == 100);


        for (int i=0; i < std::size(items); ++i) {
            items[i].unlinkAll();

            REQUIRE(list.validateList());
            REQUIRE(list.size() == 99-i);
        }
    }


}