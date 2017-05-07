#include "..\include\records.h"

std::string read_name(std::ifstream& ifs){
    char c;
    readv(c, 1);
    std::string s = "";
    while(c != '='){
        s += c;
        readv(c, 1);
    }
    return s;
}

std::string read_str(std::ifstream& ifs){
    char c;
    readv(c, 1);
    std::string s = "";
    while(c != '\0'){
        s += c;
        readv(c, 1);
    }
    return s;
}

char read_op(std::ifstream& ifs){
    char op = ' ';
    std::streampos start = ifs.tellg();
    int header_len;
    readv(header_len, 4);
    while(ifs.tellg() - start < header_len){
        int field_len;
        std::string field_name;
        readv(field_len, 4);
        field_name = read_name(ifs);
        if(field_name == "op"){
            readv(op, 1);
            ifs.seekg(start);
            return op;
        } else ifs.seekg((int)ifs.tellg() + field_len - field_name.length() - 1);
    }
    return op;
}

void Record::skip_data(std::ifstream& ifs){
    readv(data_len, 4);
    ifs.ignore(data_len);
}


