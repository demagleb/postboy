#include <gtest/gtest.h>

#include <common/csv/reader.hpp>
#include <common/csv/writer.hpp>
#include <common/util/defer.hpp>

#include <fstream>

TEST(Csv, SimpleOperations) {
    const std::string filename = "temp.txt";
    common::csv::Row headers = {"a", "b", "c"};

    std::ofstream out(filename);
    defer {
        out.close();
    };

    common::csv::Writer writer(out, headers);
    writer.WriteRow(headers);
    writer.WriteRow({"1", "2", "3"});
    writer.WriteRow({"4", "5", "6"});
    writer.Dump();

    std::ifstream inp(filename);
    defer {
        inp.close();
    };
    common::csv::Reader reader(inp, headers, true);

    auto& row = reader.Read();

    ASSERT_TRUE(row.has_value());
    ASSERT_EQ((*row)[0], "1");
    ASSERT_EQ((*row)[1], "2");
    ASSERT_EQ((*row)[2], "3");

    auto& rowMap = reader.ReadMap();
    ASSERT_TRUE(row.has_value());
    ASSERT_EQ((*rowMap)["a"], "4");
    ASSERT_EQ((*rowMap)["b"], "5");
    ASSERT_EQ((*rowMap)["c"], "6");

    row = reader.Read();
    ASSERT_FALSE(row.has_value());
}
