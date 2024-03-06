#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <Windows.h>
#include <set>

#pragma execution_character_set("utf-8")

class Book;

class Publisher {
public:
    std::string name = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");
    }
};

class Stock;

class Book {
public:
    std::string title = "";
    Wt::Dbo::ptr<Publisher> publisher;
    Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, title, "title");
        Wt::Dbo::belongsTo(a, publisher, "publisher");
        Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "book");
    }
};

class Shop {
public:
    std::string name = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "shop");
    }
};

class Stock {
public:
    Wt::Dbo::ptr<Book> book;
    Wt::Dbo::ptr<Shop> shop;
    int count = 0;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::belongsTo(a, book, "book");
        Wt::Dbo::belongsTo(a, shop, "shop");
        Wt::Dbo::field(a, count, "count");
    }
};

class Sale {
public:
    double price;
    std::string date_sale;
    Wt::Dbo::ptr<Stock> stock;
    int count = 0;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, price, "price");
        Wt::Dbo::field(a, date_sale, "date_sale");
        Wt::Dbo::belongsTo(a, stock, "stock");
        Wt::Dbo::field(a, count, "count");
    }
};

int main()
{
    std::cout << "Hello World!\n";
//    setlocale(LC_ALL, "RUSSIAN");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        std::string conn =
            "host=localhost "
            "port=5432 "
            "dbname=Lesson06DB "
            "user=postgres "
            "password=my_password";

        auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(conn);
        Wt::Dbo::Session session;
        session.setConnection(std::move(postgres));
        session.mapClass<Publisher>("publisher");
        session.mapClass<Book>("book");
        session.mapClass<Shop>("shop");
        session.mapClass<Stock>("stock");
        session.mapClass<Sale>("sale");

        try {
            session.createTables();
        }
        catch (...) {   
        }

        Wt::Dbo::Transaction translation{ session };

        std::unique_ptr<Publisher> publisher1{ new Publisher() };
        publisher1->name = "Издатель 1";
        Wt::Dbo::ptr<Publisher> publisher1Ptr = session.add(std::move(publisher1));

        std::unique_ptr<Publisher> publisher2{ new Publisher() };
        publisher2->name = "Издатель 2";
        Wt::Dbo::ptr<Publisher> publisher2Ptr = session.add(std::move(publisher2));

        std::unique_ptr<Publisher> publisher3{ new Publisher() };
        publisher3->name = "Издатель 3";
        Wt::Dbo::ptr<Publisher> publisher3Ptr = session.add(std::move(publisher3));

        std::unique_ptr<Book> book1_1{ new Book() };
        book1_1->title = "Книга 1_1";
        book1_1->publisher = publisher1Ptr;
        Wt::Dbo::ptr<Book> book1_1Ptr = session.add(std::move(book1_1));

        std::unique_ptr<Book> book1_2{ new Book() };
        book1_2->title = "Книга 1_2";
        book1_2->publisher = publisher1Ptr;
        Wt::Dbo::ptr<Book> book1_2Ptr = session.add(std::move(book1_2));

        std::unique_ptr<Book> book2_1{ new Book() };
        book2_1->title = "Книга 2_1";
        book2_1->publisher = publisher2Ptr;
        Wt::Dbo::ptr<Book> book2_1Ptr = session.add(std::move(book2_1));

        std::unique_ptr<Book> book3_1{ new Book() };
        book3_1->title = "Книга 3_1";
        book3_1->publisher = publisher3Ptr;
        Wt::Dbo::ptr<Book> book3_1Ptr = session.add(std::move(book3_1));

        std::unique_ptr<Book> book3_2{ new Book() };
        book3_2->title = "Книга 3_2";
        book3_2->publisher = publisher3Ptr;
        Wt::Dbo::ptr<Book> book3_2Ptr = session.add(std::move(book3_2));

        std::unique_ptr<Shop> shop1{ new Shop() };
        shop1->name = "Магазин 1";
        Wt::Dbo::ptr<Shop> shop1Ptr = session.add(std::move(shop1));

        std::unique_ptr<Shop> shop2{ new Shop() };
        shop2->name = "Магазин 2";
        Wt::Dbo::ptr<Shop> shop2Ptr = session.add(std::move(shop2));

        std::unique_ptr<Stock> stock1_1{ new Stock() };
        stock1_1->book = book1_1Ptr;
        stock1_1->shop = shop1Ptr;
        stock1_1->count = 21;
        Wt::Dbo::ptr<Stock> stock1_1Ptr = session.add(std::move(stock1_1));

        std::unique_ptr<Stock> stock1_2{ new Stock() };
        stock1_2->book = book3_1Ptr;
        stock1_2->shop = shop1Ptr;
        stock1_2->count = 13;
        Wt::Dbo::ptr<Stock> stock1_2Ptr = session.add(std::move(stock1_2));

        std::unique_ptr<Stock> stock2_1{ new Stock() };
        stock2_1->book = book2_1Ptr;
        stock2_1->shop = shop2Ptr;
        stock2_1->count = 3;
        Wt::Dbo::ptr<Stock> stock2_1Ptr = session.add(std::move(stock2_1));

        std::unique_ptr<Stock> stock2_2{ new Stock() };
        stock2_2->book = book1_2Ptr;
        stock2_2->shop = shop2Ptr;
        stock2_2->count = 1;
        Wt::Dbo::ptr<Stock> stock2_2Ptr = session.add(std::move(stock2_2));

        Wt::Dbo::collection<Wt::Dbo::ptr<Publisher>> publishers = session.find<Publisher>();
        std::cout << "В базе данных имеется информация о следующих издателях: " << std::endl;
        for (const Wt::Dbo::ptr<Publisher>& publisher : publishers) {
            std::cout << "ID = " << publisher.id() << " " << publisher->name << std::endl;
        }

        while (1) {
            int id;
            std::cout << "\nВведите номер ID издателя для получения информации или '0' для выхода: ";
            std::cin >> id;
            if (id == 0) break;
            // получим коллекцию книг, которые издаются выбранным издателем
            Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books = session.find<Book>().where("publisher_id = ?").bind(id);
            if (books.empty()) {
                std::cout << "Книги указанного издателя отсутствуют в базе данных, либо неверный ввод ID издателя \n\n";
                continue;
            }
            // результирующий контейнер с уникальными указателями на магазины
            std::set<Wt::Dbo::ptr<Shop>> results;
            for (const Wt::Dbo::ptr<Book>& book : books) {
                for (const Wt::Dbo::ptr<Stock>& stock : book->stocks) {
                    results.insert(stock->shop);
                }
            }
            // выводим в консоль найденные магазины
            for (const auto& shop : results) {
                std::cout << shop->name << std::endl;
            }
        }      
        translation.commit();
    }
    catch (const Wt::Dbo::Exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}

