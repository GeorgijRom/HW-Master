#include <iostream>
#include <string>
#include <cassert>

#include <sstream>

#include "collector.h"

using namespace std;

const size_t MAX_TITLE_LENGTH       = 100;
const size_t MAX_AUTHOR_LENGTH     = 150;
const size_t MAX_PUBLISHER_LENGTH  = 300;
const size_t MAX_PUBLISHING_DATE   = 2021;
const size_t MAX_GENRE_LENGTH      = 100;

class Book : public ICollectable
{
    string   _title;
    string   _author;
    string   _publisher;
    uint16_t _date;
    string   _genre;

protected:
    bool invariant() const
    {

        return (

            !_title.empty() && _title.length() <= MAX_TITLE_LENGTH
            && !_author.empty() && _author.length() <= MAX_AUTHOR_LENGTH
            && !_publisher.empty() && _publisher.length() <= MAX_PUBLISHER_LENGTH
            && _date <= MAX_PUBLISHING_DATE
            && _genre.length() <= MAX_GENRE_LENGTH

        );

    }

public:
    Book() = delete;
    Book(const Book & p) = delete;

    Book & operator = (const Book & p) = delete;

    Book(const string & title, const string & author, const string & publisher, uint16_t date, const string & genre)
        : _title(title), _author(author), _publisher(publisher), _date(date), _genre(genre)
    {
        assert(invariant());
    }

    const string & getTitle() const { return _title; }
    const string & getAuthor() const { return _author; }
    const string & getPublisher() const { return _publisher; }
    uint16_t       getDate() const { return _date; }
    const string & getGenre() const { return _genre; }

    virtual bool   write(ostream& os) override
    {

        writeString(os, _title);
        writeString(os, _author);
        writeString(os, _publisher);
        writeNumber(os, _date);
        writeString(os, _genre);

        return os.good();

    }
};

class ItemCollector: public ACollector
{
public:

    virtual shared_ptr<ICollectable> read(istream& is) override
    {

        string   title      = readString(is, MAX_TITLE_LENGTH);
        string   author    = readString(is, MAX_AUTHOR_LENGTH);
        string   publisher = readString(is, MAX_PUBLISHER_LENGTH);
        uint16_t date      = readNumber<uint16_t>(is);
        string   genre     = readString(is, MAX_GENRE_LENGTH);

        return make_shared<Book>(title, author, publisher, date, genre);

    }

};

bool performCommand(const vector<string> & args, ItemCollector & col)
{
    if (args.empty())
        return false;

    if (args[0] == "l" || args[0] == "load")
    {
        string filename = (args.size() == 1) ? "hw.data" : args[1];

        if (!col.loadCollection(filename))
        {
            cerr << "file load error '" << filename << "'" << endl;
            return false;
        }

        return true;
    }

    if (args[0] == "s" || args[0] == "save")
    {
        string filename = (args.size() == 1) ? "hw.data" : args[1];

        if (!col.saveCollection(filename))
        {
            cerr << "file save error '" << filename << "'" << endl;
            return false;
        }

        return true;
    }

    if (args[0] == "c" || args[0] == "clean")
    {
        if (args.size() != 1)
        {
            cerr << "wrong argument number in clean command" << endl;
            return false;
        }

        col.clean();

        return true;
    }

    if (args[0] == "a" || args[0] == "add")
    {
        if (args.size() != 6)
        {
            cerr << "wrong argument number in add command" << endl;
            return false;
        }
        col.addItem(make_shared<Book>(args[1].c_str(), args[2].c_str(), args[3].c_str(), stoul(args[4]), args[5].c_str()));
        return true;
    }

    if (args[0] == "r" || args[0] == "remove")
    {
        if (args.size() != 2)
        {
            cerr << "wrong argument number in remowe command" << endl;
            return false;
        }

        col.removeItem(stoul(args[1]));
        return true;
    }

    if (args[0] == "u" || args[0] == "update")
    {
        if (args.size() != 6)
        {
            cerr << "wrong argument number in update command" << endl;
            return false;
        }

        col.updateItem(stoul(args[1]), make_shared<Book>(args[1].c_str(), args[2].c_str(), args[3].c_str(), stoul(args[4]), args[5].c_str()));
        return true;
    }

    if (args[0] == "v" || args[0] == "view")
    {
        if (args.size() != 1)
        {
            cerr << "wrong argument number in view command" << endl;
            return false;
        }

        size_t count = 0;
        for(size_t i=0; i < col.getSize(); ++i)
        {
            
            const Book & item = static_cast<Book &>(*col.getItem(i));

            if (!col.isRemoved(i))
            {

                cout << "[" << i << "] "
                        << item.getTitle() << " "
                        << item.getAuthor() << " "
                        << item.getPublisher() << " "
                        << item.getDate() << " "
                        << item.getGenre() << " "
                        << endl;
                count ++;

            }

        }

        cout << "elements in collection: " << count << endl;

        return true;
    }

    cerr << "invalid command '" << args[0] << "'" << endl;
    return false;
}

int main(int , char **)
{
    ItemCollector col;

    for (string line; getline(cin, line); )
    {
        if (line.empty())
            break;

        istringstream  iss(line);
        vector<string> args;

        for(string str; iss.good();)
        {
            iss >> str;
            args.emplace_back(str);
        }

        if (!performCommand(args, col))
            return 1;
    }

    cout << "Execution completed successfully" << endl;

    return 0;
}
