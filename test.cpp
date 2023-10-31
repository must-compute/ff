#include <gtest/gtest.h>

#include <fstream>
#include <filesystem>
#include <map>

class FuzzySearchFixture : public ::testing::Test {
protected:
    std::string temp_dir = "/tmp/fuzzy_search_test";
    std::map<std::string, std::string> files_and_content;

    void SetUp() override {
        std::filesystem::create_directory(temp_dir);

        files_and_content = {
                {"file1.txt", "TODO update some tests"},
                {"file2.md",  "file2 has a TODO as well"},
                {"file3.txt", "file3 stuff"}
        };

        for (const auto &[filename, content]: files_and_content) {
            std::ofstream outfile(temp_dir + "/" + filename);
            outfile << content;
            outfile.close();
        }
    }

    void TearDown() override {
        std::filesystem::remove_all(temp_dir);
    }
};

TEST_F(FuzzySearchFixture, PlaceholderTest) {
    EXPECT_EQ(1 + 1, 2);
}
