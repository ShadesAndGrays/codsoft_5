#pragma once

#include <ctime>
#include <mariadb/conncpp.hpp>
#include <mariadb/conncpp/Connection.hpp>
#include <mariadb/conncpp/Exception.hpp>
#include <mariadb/conncpp/jdbccompat.hpp>
#include <memory>
#include <vector>

namespace schema{
    struct Book{
        std::string title;
        std::string author;
        std::string isbn;
        int noOfCopies;
    };

    struct Log{
        int borrowId;
        time_t borrowDate;
        bool hasReturned;
        std::string isbn;
    };
    void printBooks(std::vector<schema::Book> books);
    void printlogs(std::vector<schema::Log> logs);
}

class DatabaseManger{


        DatabaseManger() = default;
    private:
        std::shared_ptr<sql::Connection> conn;
        static std::shared_ptr<DatabaseManger> dbManager;

    public:
        static std::shared_ptr<DatabaseManger> getInstance();
        std::shared_ptr<sql::Connection> getConnection();
        std::vector<schema::Book> getBooks();
        std::vector<schema::Book> getBooks(std::string column, std::string cmp, bool exact = false);
        std::vector<schema::Book> getBooks(std::vector<std::string> isbnOfBooks);
        void addBook(schema::Book bk);
        void addBook(std::vector<std::string> isbnOfBooks,int number);
        void borrowBook(std::vector<std::string> &isbnOfBooks);
        void returnBook(int borrowBookID);
        std::vector<schema::Log> getBorrowLog(int borrowId);
        std::vector<schema::Log> getBorrowLog();
        void cleanUp();
        int nextBorrowID();


};

// requires ENVIRONMENT VARIABLES
