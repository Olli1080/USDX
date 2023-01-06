#pragma once

#include <chrono>

#include <sqlite_orm/sqlite_orm.h>

/**
 *  This is where magic happens. To tell sqlite_orm how to act
 *  with SysDays we have to create a few service classes
 *  specializations (traits) in sqlite_orm namespace.
 */
namespace sqlite_orm {

    /**
     *  First of all is a type_printer template class.
     *  It is responsible for sqlite type string representation.
     *  We want SysDays to be `TEXT` so let's just derive from
     *  text_printer. Also there are other printers: real_printer and
     *  integer_printer. We must use them if we want to map our type to `REAL` (double/float)
     *  or `INTEGER` (int/long/short etc) respectively.
     */
    template<>
    struct type_printer<std::chrono::system_clock::time_point> : public text_printer {};

    /**
     *  This is a binder class. It is used to bind c++ values to sqlite queries.
     *  Here we have to create sysday string representation and bind it as string.
     *  Any statement_binder specialization must have `int bind(sqlite3_stmt*, int, const T&)` function
     *  which returns bind result. Also you can call any of `sqlite3_bind_*` functions directly.
     *  More here https://www.sqlite.org/c3ref/bind_blob.html
     */
    template<>
    struct statement_binder<std::chrono::system_clock::time_point> {

        int bind(sqlite3_stmt* stmt, int index, const std::chrono::system_clock::time_point& value) const {
            return statement_binder<std::string>().bind(stmt, index, std::to_string(value.time_since_epoch().count()));
        }
    };

    /**
     *  field_printer is used in `dump` and `where` functions. Here we have to create
     *  a string from mapped object.
     */
    template<>
    struct field_printer<std::chrono::system_clock::time_point> {
        std::string operator()(const std::chrono::system_clock::time_point& t) const {
            return std::to_string(t.time_since_epoch().count());
        }
    };

    /**
     *  This is a reverse operation: here we have to specify a way to transform string received from
     *  database to our sysdays object. Here we call `sysDaysFromString` and throw `std::runtime_error` if it returns null.
     *  Every `row_extractor` specialization must have `extract(const char*)`, `extract(sqlite3_stmt *stmt, int columnIndex)`
     *	and `extract(sqlite3_value* value)`
     *  functions which return a mapped type value.
     */
    template<>
    struct row_extractor<std::chrono::system_clock::time_point> {
        std::chrono::system_clock::time_point extract(const char* row_value) const {
            if (row_value) {
                return std::chrono::system_clock::time_point(std::chrono::system_clock::duration(std::stoll(row_value)));
            }
            else {
                // ! row_value
                throw std::runtime_error("incorrect date string (nullptr)");
            }
        }

        std::chrono::system_clock::time_point extract(sqlite3_stmt* stmt, int columnIndex) const {
            const auto str = sqlite3_column_text(stmt, columnIndex);
            return this->extract(reinterpret_cast<const char*>(str));
        }
        std::chrono::system_clock::time_point extract(sqlite3_value* row_value) const {
            const auto characters = reinterpret_cast<const char*>(sqlite3_value_text(row_value));
            return extract(characters);
        }
    };
}
