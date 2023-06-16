#ifndef KEYS_HPP
#define KEYS_HPP

#include <functional>
#include <iostream>

#include "structures.hpp"

class Key {
    private:
    union Value {
        struct {
            unsigned char lowercase, uppercase;
        } ascii;
        int special;
        Value(unsigned char ascii) {
            this->ascii.lowercase = tolower(ascii);
            if (ascii >= 'a' && ascii <= 'z' || ascii >= 'A' && ascii <= 'Z') {
                this->ascii.uppercase = toupper(ascii);
            }
        }
        Value(int special) : special(special) {}
    } value;
    std::string name;
    std::string description;
    std::function<void()> onDown;
    std::function<void()> onUp;
    bool isAscii;
    bool isAlphabetical;

    public:
    Key(char ascii, std::string description, bool &toggle) : Key(ascii, description, [&toggle] { toggle = !toggle; }) {}
    Key(char ascii, std::string name, std::string description, bool &toggle) : Key(ascii, name, description, [&toggle] { toggle = !toggle; }) {}
    Key(unsigned char ascii, std::string name, std::string description, bool &toggle) : Key(ascii, name, description, [&toggle] { toggle = !toggle; }) {}
    Key(int special, std::string name, std::string description, bool &toggle) : Key(special, name, description, [&toggle] { toggle = !toggle; }) {}

    Key(char ascii, std::string description, std::function<void()> onDown = nullptr, std::function<void()> onUp = nullptr) : Key((unsigned char) ascii, std::string(1, ascii), description, onDown, onUp) {}
    Key(char ascii, std::string name, std::string description, std::function<void()> onDown = nullptr, std::function<void()> onUp = nullptr) : Key((unsigned char) ascii, name, description, onDown, onUp) {}
    Key(unsigned char ascii, std::string name, std::string description, std::function<void()> onDown = nullptr, std::function<void()> onUp = nullptr)
        : value(ascii), name(name), description(description), onDown(onDown), onUp(onUp), isAscii(true), isAlphabetical(ascii >= 'a' && ascii <= 'z' || ascii >= 'A' && ascii <= 'Z') {}
    Key(int special, std::string name, std::string description, std::function<void()> onDown = nullptr, std::function<void()> onUp = nullptr)
        : value(special), name(name), description(description), onDown(onDown), onUp(onUp), isAscii(false) {}

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }

    bool operator==(unsigned char &ascii) const { return isAscii && (ascii == value.ascii.lowercase || isAlphabetical && ascii == value.ascii.uppercase); }
    friend bool operator==(unsigned char &ascii, Key &key) { return key == ascii; }
    bool operator==(int &special) const { return !isAscii && special == value.special; }
    friend bool operator==(int &special, Key &key) { return key == special; }

    void down() const {
        if (onDown != nullptr) onDown();
    }
    void up() const {
        if (onUp != nullptr) onUp();
    }
};

#endif