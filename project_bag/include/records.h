#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#define readv(c, n) ifs.read((char*)&c, n)


class Record{
public:
    int header_len;
    int data_len;
    void skip_data(std::ifstream& ifs);
};

char read_op(std::ifstream& ifs);

std::string read_name(std::ifstream& ifs);

std::string read_str(std::ifstream& ifs);

class BagHeader : public Record{
public:
    long long index_pos;
    int conn_count;
    int chunk_count;
    friend std::ifstream& operator>>(std::ifstream& ifs, BagHeader& bh){
        readv(bh.header_len, 4);
        std::streampos start = ifs.tellg();
        while(bh.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "index_pos"){
                readv(bh.index_pos, 8);
            } else if(field_name == "conn_count"){
                readv(bh.conn_count, 4);
            } else if(field_name == "chunk_count"){
                readv(bh.chunk_count, 4);
            } else ifs.ignore(field_len - field_name.length() - 1);
        }
        bh.skip_data(ifs);
        return ifs;
    }
};

class Chunk : public Record{
    std::string compression;
    int size;
    friend std::ifstream& operator>>(std::ifstream& ifs, Chunk& c){
        readv(c.header_len, 4);
        std::streampos start = ifs.tellg();
        while(c.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "conn"){
                char first;
                readv(first, 1);
                if(first == 'b'){
                    ifs.ignore(2);
                    c.compression = "bz2";
                } else if(first == 'n'){
                    ifs.ignore(3);
                    c.compression = "none";
                }
            } else if(field_name == "size"){
                readv(c.size, 4);
            } else ifs.ignore(field_len - field_name.length() - 1);
        }
        c.skip_data(ifs);
        return ifs;
    }
};

class Connection : public Record{
public:
    std::string topic;
    int conn;
    friend std::ifstream& operator>>(std::ifstream& ifs, Connection& c){
        readv(c.header_len, 4);
        std::streampos start = ifs.tellg();
        while(c.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "topic"){
                c.topic = read_str(ifs);
            } else if(field_name == "conn"){
                readv(c.conn, 4);
            } else ifs.ignore(field_len - field_name.length() - 1);
        }
        c.skip_data(ifs);
        return ifs;
    }
};

class MessageData : public Record{
public:
    int conn;
    long long time;
    friend std::ifstream& operator>>(std::ifstream& ifs, MessageData& md){
        readv(md.header_len, 4);
        std::streampos start = ifs.tellg();
        while(md.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "time"){
                readv(md.time, 8);
            } else if(field_name == "conn"){
                readv(md.conn, 4);
            } else ifs.ignore(field_len - field_name.length() - 1);
        }
        md.skip_data(ifs);
        return ifs;
    }
};


class IndexData : public Record{
public:
    int ver;
    int conn;
    int count;
    friend std::ifstream& operator>>(std::ifstream& ifs, IndexData& id){
        readv(id.header_len, 4);
        std::streampos start = ifs.tellg();
        while(id.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "ver"){
                readv(id.ver, 4);
            } else if(field_name == "conn"){
                readv(id.conn, 4);
            } else if(field_name == "count"){
                readv(id.count, 4);
            } else ifs.ignore(field_len - field_name.length() - 1);
        }
        id.skip_data(ifs);
        return ifs;
    }
};

class ChunkInfo : public Record{
public:
    int ver;
    long long chunk_pos;
    long long start_time;
    long long end_time;
    int count;
    friend std::ifstream& operator>>(std::ifstream& ifs, ChunkInfo& ci){
        readv(ci.header_len, 4);
        std::streampos start = ifs.tellg();
        while(ci.header_len + start > ifs.tellg()){
            int field_len;
            readv(field_len, 4);
            std::string field_name = read_name(ifs);
            if(field_name == "ver"){
                readv(ci.ver, 4);
            } else if(field_name == "count"){
                readv(ci.count, 4);
            } else if(field_name == "chunk_pos"){
                readv(ci.chunk_pos, 8);
            } else if(field_name == "start_time"){
                readv(ci.start_time, 8);
            } else if(field_name == "end_time"){
                readv(ci.end_time, 8);
            } else ifs.ignore(field_len - field_name.length() - 1);
        }
        ci.skip_data(ifs);
        return ifs;
    }
};


