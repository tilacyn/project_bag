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

void write_name(std::string str, std::ofstream& ofs){
    for(unsigned int i = 0; i < str.length(); i++){
        writev(str[i], 1);
    }
    char c = '=';
    writev(c, 1);
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
        } else ifs.seekg((long long)ifs.tellg() + field_len - field_name.length() - 1);
    }
    return op;
}

int read_conn(std::ifstream& ifs){
    int conn = 0;
    std::streampos start = ifs.tellg();
    int header_len;
    readv(header_len, 4);
    while(ifs.tellg() - start < header_len){
        int field_len;
        std::string field_name;
        readv(field_len, 4);
        field_name = read_name(ifs);
        if(field_name == "conn"){
            readv(conn, 4);
            ifs.seekg(start);
            return conn;
        } else ifs.seekg((long long)ifs.tellg() + field_len - field_name.length() - 1);
    }
    return conn;
}

void Record::skip_data(std::ifstream& ifs){
    readv(data_len, 4);
    ifs.seekg((long long) ifs.tellg() + data_len);
}

void Record::skip_header(std::ifstream& ifs){
    readv(header_len, 4);
    ifs.seekg((long long) ifs.tellg() + header_len);
}

long long Record::header_end(){ return pos + 4 + header_len;}
long long Record::data_end(){return pos + 8 + header_len + data_len;}
long long Record::data_start(){return pos + 8 + header_len;}



long long Record::record_len(std::ifstream& ifs){
    skip_header(ifs);
    skip_data(ifs);
    return header_len + data_len + 8;
}


void seq_chunk_to_info(std::vector<Chunk>& chunks, std::vector<ChunkInfo>& chunkinfo){
    for(unsigned int i = 0; i < chunkinfo.size(); i++){
        std::lower_bound(chunks.begin(), chunks.end(), chunkinfo[i].chunk_pos, compare_chunks)->ci = chunkinfo[i];
    }
}

bool compare_chunks(Chunk& c, long long offset){
    return c.pos < offset;
}


