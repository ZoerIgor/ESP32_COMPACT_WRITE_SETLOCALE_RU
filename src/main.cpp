#include "main.h"

#define DEBUG
#define SIZE_TOTAL_EEPROM 512
#define DELAY 500

String str1 = "Hello World 1995@!";
String str2 = "Ёлка или Елка, ёж или еж?";
String str3;
String str4;

// TYPE ASCII
void MemoryASCII::ASCII_UTF8_RU::use7LetterAlphabet(bool value)
{
    _letter7 = value;
}
byte MemoryASCII::ASCII_UTF8_RU::charToByte(const char symbol)
{
    byte value = symbol;
    return value;
}
char MemoryASCII::ASCII_UTF8_RU::byteToChar(const byte value)
{
    char symbol = value;
    return symbol;
}
String MemoryASCII::ASCII_UTF8_RU::getValue(const byte &key)
{
    String value = "";
    if (key >= _secondByteBegin && key <= _secondByteEnd)
    {
        for (auto it = _encoding.begin(); it != _encoding.end(); it++)
        {
            if (it->first == key)
            {
                value += it->second;
                return value;
            }
        }
    }
    char v = byteToChar(key);
    value += v;
    return value;
}
byte MemoryASCII::ASCII_UTF8_RU::getKey(const char &value)
{
    byte val = charToByte(value);
    if (val == _firstByteBegin)
    {
        _fb208 = true;
        return 0;
    }
    if (val == _firstByteEnd)
    {
        _fb209 = true;
        return 0;
    }
    if (val == 129 && _fb208)
    {
        if (_letter7)
        {
            val = 208;
        }
        else
        {
            val = 149;
        }
    }
    if (val == 145 && _fb209)
    {
        if (_letter7)
        {
            val = 209;
        }
        else
        {
            val = 181;
        }
    }
    if (_fb208){_fb208 = false;}
    if (_fb209){_fb209 = false;}
    return val;
}
MemoryASCII::MemoryASCII(const size_t address, const size_t range)
{
    _address = address;
    _range = range;
}
byte MemoryASCII::read(const size_t address)
{
    return EEPROM.readByte(address);
}
void MemoryASCII::readArray(String &target)
{
    size_t counter = 0;
    target = "";
    while (counter < _range)
    {
        byte v = read(_address + counter++);
        if (v == 0)
        {
            break;
        }
        if (v == 208)
        {
            target += "Ё";
        }
        else if (v == 209)
        {
            target += "ё";
        }
        else
        {
            target += ru.getValue(v);
        }
    }
}
bool MemoryASCII::write(const size_t address, const byte &value)
{
    byte oldValue = EEPROM.readByte(address);
    if (value != oldValue)
    {
        EEPROM.writeByte(address, value);
        return true;
    }
    return false;
}
void MemoryASCII::writeArray(const String &source)
{
    size_t length = source.length();
    size_t counter = 0;
    size_t indx = 0;
    while (counter < length)
    {
        byte value = ru.getKey(source[counter]);
        if (value != 0)
        {
            write(_address + indx++, value);
        }
        counter++;
    }
    if (counter < length)
    {
        write(_address + counter, 0);
    }
}

void BeginEeprom()
{
    if (!EEPROM.begin(SIZE_TOTAL_EEPROM))
    {
        #ifdef DEBUG0
            Serial.println("Failed to initialise EEPROM");
            Serial.println("Restarting...");
            Serial.println("-------------------------------");
        #endif
        ESP.restart();
    }
    delay(DELAY);
}
void CleanEeprom()
{
    for (auto i = 0; i < SIZE_TOTAL_EEPROM; i++)
    {
        if (EEPROM.readByte(i) != 0)
        {
            EEPROM.write(i, 0);
        }
    }
}
void ReadEeprom()
{
    for (auto i = 0; i < SIZE_TOTAL_EEPROM; i++)
    {
        Serial.print(EEPROM.readByte(i));
        Serial.print(" ");
    }
}

void setup()
{
    setlocale(LC_ALL, "ru");
    Serial.begin(921600);
    Serial.println();

    BeginEeprom();
    CleanEeprom();
    
    
    MemoryASCII test1(0, 30);
    test1.writeArray(str1);

    MemoryASCII test2(30, 30);
    test2.writeArray(str2);

    test1.readArray(str3);
    test2.readArray(str4);

    Serial.println(str3);
    Serial.println(str4);

    //ReadEeprom();
}

void loop()
{
    // put your main code here, to run repeatedly:
}