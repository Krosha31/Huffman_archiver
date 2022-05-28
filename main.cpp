#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

const int BIT8 = 8;
const int BYTE32 = 32;
const int CODE_SIZE = 256;

struct node {
    unsigned char symb = 0;
    unsigned char isSymb = false;
    unsigned int freq = 0;
    unsigned char len = 0;
    std::string code;
    node* left = nullptr;
    node* right = nullptr;
    node* next = nullptr;
};


union bit2char {
    char symb;
    struct bit {
        unsigned b1 : 1;
        unsigned b2 : 1;
        unsigned b3 : 1;
        unsigned b4 : 1;
        unsigned b5 : 1;
        unsigned b6 : 1;
        unsigned b7 : 1;
        unsigned b8 : 1;
    }mbit;
};



node* AddToList(node* phead, node* pnew) {
    if (phead == nullptr) {
        return pnew;
    }
    node* temp = phead;
    if (pnew->freq < temp->freq) {
        pnew->next = temp;
        return pnew;
    }
    while (temp->next && temp->next->freq <= pnew->freq) {
        temp = temp->next;
    }
    pnew->next = temp->next;
    temp->next = pnew;
    return phead;
}


void DeleteList(node* phead) {
    while (phead) {
        node* temp = phead;
        phead = phead->next;
        delete temp;
    }
}

void DeleteTree(node* ptree) {
    if (ptree->isSymb) {
        delete ptree;
        return;
    }
    DeleteTree(ptree->left);
    DeleteTree(ptree->right);
    delete ptree;
}

void PrintList(const node* phead) {
    while (phead) {
        std::cout << phead->symb << " " << phead->code << std::endl;
        phead = phead->next;
    }
    std::cout << std::endl;
}


void PrintTree(const node* phead) {
    if (!phead)
        return;
    if (phead->isSymb) {
        std::cout << phead->symb << " " << phead->code << std::endl;
    }
    else {
        PrintTree(phead->left);
        PrintTree(phead->right);
    }
}

node* MakeTree(node* phead)
{
    while (phead->next) {
        node* res = new node;
        node* left = phead;
        node* right = phead->next;
        res->freq = left->freq + right->freq;
        res->left = left;
        res->right = right;
        phead = phead->next->next;
        left->next = right->next = nullptr;
        phead = AddToList(phead, res);
    }
    return phead;
}


node* MakeList(node* ptree) {
    if (ptree == nullptr)
        return nullptr;
    if (ptree->isSymb) {
        node* phead = new node;
        phead->symb = ptree->symb;
        phead->code = ptree->code;
        return phead;
    }
    node* left = MakeList(ptree->left);
    node* right = MakeList(ptree->right);
    if (left) {
        node* temp = left;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = right;
        return left;
    }
    return nullptr;
}



void MakeCode(node* phead)
{
    if (phead->left)
    {
        phead->left->code = phead->code + '0';
        MakeCode(phead->left);
    }
    if (phead->right)
    {
        phead->right->code = phead->code + '1';
        MakeCode(phead->right);
    }
}





std::string MakeNewString(std::string input_string, node* phead) {
    std::string new_string;
    for (auto symb: input_string) {
        node* temp = phead;
        while (temp) {
            if (temp->symb == symb) {
                new_string += temp->code;
                break;
            }
            temp = temp->next;
        }
    }
    return new_string;
}


char ReturnBit(int i, int k, unsigned char &count_zero, const std::string &str) {
    //Фактически бесполезна. Можно эту строчку просто вставить на места вызова
    return str[i * BIT8 + k];
}


void Coding(const std::string &str, std::string &res, unsigned char count_zero, int len) {
    //Функция Лупановой(почти). Она переделывает 8 символов 0 и 1 в один байт типа char
    bit2char symb;
    for (int i = 0; i < len; i++)
    {
        symb.mbit.b1 = ReturnBit(i, 0, count_zero, str);
        symb.mbit.b2 = ReturnBit(i, 1, count_zero, str);
        symb.mbit.b3 = ReturnBit(i, 2, count_zero, str);
        symb.mbit.b4 = ReturnBit(i, 3, count_zero, str);
        symb.mbit.b5 = ReturnBit(i, 4, count_zero, str);
        symb.mbit.b6 = ReturnBit(i, 5, count_zero, str);
        symb.mbit.b7 = ReturnBit(i, 6, count_zero, str);
        symb.mbit.b8 = ReturnBit(i, 7, count_zero, str);
        symb.symb = symb.mbit.b1 * pow(2, 7) + symb.mbit.b2 * pow(2, 6) +
                    symb.mbit.b3 * pow(2, 5) + symb.mbit.b4 * pow(2, 4) +
                    symb.mbit.b5 * pow(2, 3) + symb.mbit.b6 * pow(2, 2) +
                    symb.mbit.b7 * pow(2, 1) + symb.mbit.b8 * pow(2, 0);
        res += symb.symb;
    }
}


void FindLength(node* phead) {
    //Находит длину списка
    node* temp = phead;
    unsigned char count = 0;
    while (temp) {
        count++;
        temp = temp->next;
    }
    phead->len = count;
}




void WriteFile(std::string& str, node* phead, const std::string& filename) {
    //Запись в файл. Советую тестировать те файлы, которые лежат вместе с файлом main.exe, т.к. нормальной работы с
    //абсолютными(и даже относительными путями) пока нет
    //Открываем файл, записываем туда длину списка
    std::string Fname = "(comp)" + filename;
    std::ofstream fout(Fname);
    int len = (int)str.size() / BIT8 + 1;
    std::cout << len << "\n";
    std::string res;
    fout << phead->len;
    unsigned char count_zero = 0;
    node* current = phead;
    //Запись информации об алфавите в файл. Записывается: 1)исходная буква 2)Количество байт которое нужно для записи
    // кода буквы(в типе char) 3) Количество бесполезных нулей(то есть могут быть коды 001 и 0001, переводя их в char
    //мы одинаково получаем 1, то есть с точки зрения char'a они неразличимы. Поэтому мы храним количество бесполензых нулей
    // В первом случае будет 5, во втором 4.
    while (current) {
        unsigned char byte_for_symbol = 0;
        //Считаем количество байт, которое нам нужно для записи кода(делим на 8 тк чаром мы записываем сразу 8 разрядов)
        if (current->code.size() % 8 != 0) {
            byte_for_symbol = 1;
        }
        byte_for_symbol += current->code.size() / 8;
        count_zero = byte_for_symbol * BIT8 - current->code.size();
        //Добавляем нули чтобы длина кода делилась на 8 и функция Coding нормально работала
        for (int k = 0; k < count_zero; k++) {
            current->code  = '0' + current->code;
        }
        //выводим в файл
        fout << current->symb << byte_for_symbol << count_zero;
        //выводим код представленный в char'ах
        Coding(current->code, res, count_zero, byte_for_symbol);
        fout << res;

        res = "";
        current = current->next;
    }
    // В сущности делаем то же самое только для сообщения
    count_zero = len * BIT8 - str.size();
    for (int i = 0; i < count_zero; i++) {
        str = '0' + str;
    }
    fout << len << count_zero;
    Coding(str, res, count_zero, len);
    for (char i: res) {
        fout << i;
    }
    fout.close();
}


void From10To2(unsigned char number, std::string &result) {
    //Перевод из  сhar обратно в двоичный код
    std::string dop;
    while (number != 0) {
        dop += number % 2 + '0';
        number = number / 2;
    }
    if (dop.size() < BIT8) {
        for (int i = 0; i < BIT8 - dop.size(); i++) {
            result += "0";
        }
    }
    for (int i = dop.size() - 1; i >= 0; i--) {
        result += dop[i];
    }

}


void DecodeString(std::string &new_string, std::string &old_string, node* phead, int minlen) {
    //Декодирование строки. На вход подается строка из 0 и 1(с уже обрезанными лишними нулями) и начинается поиск
    // в списке phead соответсвующей буквы. Напомню, код префиксный
    std::string current;
    int count = 0;
    bool flag = true;
    while (flag) {
        flag = false;
        for (auto symb: new_string) {
            current += symb;
            count++;
            if (count < minlen) {
                continue;
            }
            node *temp = phead;
            while (temp) {
                if (temp->code == current) {
                    old_string += (char) temp->symb;
                    new_string = new_string.substr(count, new_string.size() - count);
                    current = "";
                    count = 0;
                    flag = true;
                    break;
                }
                temp = temp->next;
            }
            if (flag)
                break;
        }
    }
}


void WriteDecodeFile(const std::string &filename) {
    //Не работает по любому что то здесь
    //Открываем файл на чтение, считываем длину списка с помощью get
    std::fstream fin(filename);
    node* phead = new node;
    auto len_phead = (unsigned char)fin.get();
    int minlen = 32 * 8 + 1;
    std::string old_string;
    for (int i = 0; i < len_phead; i++) {
        // Считывание каждого элемента списка. Сначала сам символ, потом количество занимаемых байт для кода и количество
        //бесполезных нулей
        unsigned char byte_for_symbol, count_zero;
        char symb;
        symb = fin.get();
        byte_for_symbol = fin.get();
        count_zero = fin.get();
        std::string code;
        //Считвание кода. Перевод его из char обратно в string и создание списка(вникать не надо, тут все работает хорошо)
        for (int j = 0; j < (int)byte_for_symbol; j++) {
            auto c = fin.get();
            std::string code_byte;
            if (count_zero > 0) {
                std::string code_byte_temp;
                From10To2(c, code_byte_temp);
                for (int t = count_zero; t < byte_for_symbol * BIT8; t++) {
                    code_byte += code_byte_temp[t];
                }
            }
            else {
                From10To2(c, code_byte);
            }
            code += code_byte;
        }
        if (i == 0) {
            phead->symb = symb;
            phead->code = code;
            phead->len = len_phead;
            minlen = std::min(minlen, (int)phead->code.size());
        }
        else {
            node* pnew = new node;
            pnew->symb = symb;
            pnew->code = code;
            phead = AddToList(phead, pnew);
            minlen = std::min(minlen, (int)pnew->code.size());
        }
    }
    //Беда предположительно начинается здесь. В сущности мы делаем абсолютно то де самое, что и шагом ранее, только для
    //самого закодированного сообщения. Считываем его длину(в char'ах) и раскодируем в 0 и 1
    //Но почему-то файл читается не до конца, это уже обсуждалось
    std::string new_string;
    int len = 0;
    //fin.read((char*)&len, sizeof(len));
    fin >> len;
    auto count_zero = (unsigned char)fin.get();
    for (int k = 0; k < len; k++) {
        auto c = (unsigned char)fin.get();
        //std::cout << (int)c << " ";
        std::string code_byte;
        if (count_zero > 0) {
            std::string code_byte_temp;
            From10To2(c, code_byte_temp);
            for (int i = count_zero; i < BIT8; i++) {
                code_byte += code_byte_temp[i];
            }
            count_zero = 0;
        }
        else {
            From10To2(c, code_byte);
        }
        new_string += code_byte;
        //Вызывается каждую итерацию цикла чтобы не захламлять память
        DecodeString(new_string, old_string, phead, minlen);
    }
    fin.close();
    DeleteList(phead);
    std::ofstream fout("(decode)" + filename);
    fout << old_string;
    fout.close();
}



int main() {
//    std::ofstream fout("fff.txt");
//    char a = 232;
//    fout << 'v' << a << EOF << a;
//    fout.close();
//    std::ifstream fin("fff.txt");
//    while(!fin.eof()) {
//        std::cout << (int)fin.get() << " ";
//    }
//    fin.close();
    std::cout << "What do you want? \nPress \"1\" to compress\nPress \"2\" to uncompress\n";
    std::string answer;
    std::cin >> answer;
    if (answer != "1" && answer != "2") {
        std::cout << "Error";
        return 0;
    }
    std::cout << "Enter the name of file:\n";
    std::string filename;
    std::cin >> filename;
    if (answer == "1") {
        std::string input_string;
        std::ifstream fin(filename);
        while (!fin.eof()) {
            std::string dop;
            std::getline(fin, dop);
            input_string += dop;
            if (!fin.eof()) {
                input_string += '\n';
            }
        }
        fin.close();
        int length = (int) input_string.size();
        unsigned char freq[256] = {0};
        for (int i = 0; i < length; ++i)
            freq[(unsigned char) input_string[i]]++;
        node *phead = nullptr;
        for (int i = 0; i < 256; i++) {
            if (freq[i] != 0) {
                node *pnew = new node;
                pnew->symb = i;
                pnew->freq = freq[i];
                pnew->isSymb = true;
                phead = AddToList(phead, pnew);
            }
        }
        node *ptree = MakeTree(phead);
        MakeCode(ptree);
        phead = MakeList(ptree);
        FindLength(phead);
        std::string new_string = MakeNewString(input_string, phead);
        WriteFile(new_string, phead, filename);
        DeleteTree(ptree);
        DeleteList(phead);
        WriteDecodeFile("(comp)" + filename);
    }
    else {
        WriteDecodeFile(filename);
    }
    std::cout << "OK";
    return 0;
}