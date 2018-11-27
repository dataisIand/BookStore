#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>
#include <vector>
#include <cstdio>
#include "database.hpp"
#include "treap.hpp"

namespace sjtu{
    struct book{
        using string = std::string;
    
        string ISBN;
        string name;
        string author;

        long long price, count;
        std::vector<string> keyword;

        book () { price = count = 0; }
        
        static const int BOOKSIZE = 200;

        void clear() {
            ISBN = "EMPTY";
            name = "EMPTY";
            author = "EMPTY";
            price = count = 0;
            keyword.clear();
        }
        string combineName() const{ return name + ISBN; }
        string combineAuthor() const{ return author + ISBN; }
        string combineKeyword(int x) const{ return keyword[x] + ISBN; }
        string combinePrice() const{ return std::to_string(price) + ISBN; }

        friend std::ostream& operator<< (std::ostream& os, const book& u) {
            char outbuf[BOOKSIZE]; int last = 0;
            last = sprintf(outbuf, "%s %s %s %lld %lld %lu",
                           u.ISBN.c_str(), u.name.c_str(), u.author.c_str(), u.price, u.count, u.keyword.size());
            for (auto &s : u.keyword)
                last += sprintf(outbuf + last, " %s", s.c_str());
            for (int i = last; i < BOOKSIZE - 1; ++i)
                outbuf[i] = ' ';
            outbuf[BOOKSIZE - 1] = '\n';
            os << outbuf;
            return os;
        }

        friend std::istream& operator>> (std::istream& is, book& u) {
            int count = 0;
            is >> u.ISBN >> u.name >> u.author >> u.price >> u.count >> count;
            u.keyword.resize(count);
            for (auto &s : u.keyword)
                is >> s;
            return is;
        }
    };

    class bookSystem : public DataBase<book, book::BOOKSIZE>{
    public:
        bookSystem () : DataBase<book, book::BOOKSIZE>("book.bin") {
            isbn.set("isbn.bin", [&](int x) { return get(x).ISBN; });
            name.set("name.bin", [&](int x) { return get(x).combineName(); });
            author.set("author.bin", [&](int x) { return get(x).combineAuthor(); });
            keyword.set("keyword.bin", [&](int x) { return get(x / 100).combineKeyword(x % 100); });
            price.set("price.bin", [&](int x) { return get(x).combinePrice(); });
            id = -1; bk.clear();
        }

        void select(std::string _isbn) {
            int idx = isbn.find(_isbn);
            if (idx == -1) {
                bk.clear();
                bk.ISBN = _isbn;
                idx = insert(bk);
            }else
                bk = get(idx);
            id = idx;
        }

        bool check(const std::string &para,
                   const std::string &tag,
                   bool flag = true) {
            return para.size() > tag.size() && para.substr(0, tag.size()) == tag && (flag || para.back() == '"');
        }

        void erase(book bk, int id) {
            isbn.erase(id);
            name.erase(id);
            author.erase(id);
            price.erase(id);
            for (size_t i = 0; i < bk.keyword.size(); ++i)
                keyword.erase(id * 100 + i);
        }

        int insert(book bk) {
            int d = append(bk);
            isbn.insert(d);
            name.insert(d);
            author.insert(d);
            price.insert(d);
            for (size_t i = 0; i < bk.keyword.size(); ++i)
                keyword.insert(d * 100 + i);
            return d;
        }
        
        void modify(parameter para) {
            if (id == -1) error();
            else {
                book tmp = bk;
                for (auto e : para) {
                    if (check(e, "-ISBN=")) tmp.ISBN = e.substr(6, e.size() - 6);
                    else if (check(e, "-name=\"", false)) tmp.name = e.substr(7, e.size() - 8);
                    else if (check(e, "-anthor=\"", false)) tmp.author = e.substr(9, e.size() - 10);
                    else if (check(e, "-price=")) tmp.author = to_int(e.substr(7, e.size() - 7));
                    else if (check(e, "-keyword=\"", false)) tmp.keyword = split(e.substr(10, e.size() - 11), '|');
                    else { error(); return; }
                }
                int idx = isbn.find(tmp.ISBN);
                if (idx != -1 && idx != id) {
                    error();
                    return;
                }
                erase(bk, id);
                id = insert(tmp);
                bk = tmp;
            }
        }

        void show(const string& para) {
            // if (check(e, "-ISBN=")) 
            // else if (check(e, "-name=")) 
            // else if (check(e, "-anthor=")) tmp.author = e.substr(8, e.size() - 8);
            // else if (check(e, "-price=")) tmp.author = to_int(e.substr(7, e.size() - 7));
        }
    
    private:
        Treap<std::string> isbn, name, author, keyword, price;
        int id; book bk;
    };

}
#endif
