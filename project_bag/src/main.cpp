#include "write.h"

using namespace std;

vector <Chunk> chunks;
vector <Connection> connections;
vector <ChunkInfo> chunkinfo;
vector <IndexData> indexdata;
vector <Select> selects;
BagHeader bh;

char* first_symbols = new char[20];

void read_records(const char* in_file){
    std::ifstream ifs(in_file, std::ios::binary);
    readv(first_symbols, 13);
    while(ifs.peek() != std::ifstream::traits_type::eof()){
        char op = read_op(ifs);
        if(op == 0x02){
            MessageData md;
            ifs >> md;
        } else if(op == 0x04){
            IndexData id;
            ifs >> id;
            assert(!chunks.empty());
            chunks[chunks.size() - 1].indexdata[id.conn] = id;
        } else if(op == 0x06){
            ChunkInfo ci;
            ifs >> ci;
            chunkinfo.push_back(ci);
        } else if(op == 0x03){
            ifs >> bh;
        } else if(op == 0x05){
            Chunk c;
            ifs >> c;
            chunks.push_back(c);
        } else if(op == 0x07){
            Connection c;
            ifs >> c;
            connections.push_back(c);
        }
    }
    cout << "Reading finished\n";
    seq_chunk_to_info(chunks, chunkinfo);
    ifstream ifs1(in_file, std::ios::binary);
    for(unsigned int i = 0; i < chunks.size(); i++){
        chunks[i].seq_id_to_conn(ifs1);
    }
}

void select_data(const char* in_file, long long time_start, long long time_end, string topic){
    ifstream ifs(in_file, ios::binary);
    make_map(time_start, time_end, topic, chunks, selects, bh, ifs);
    cout << "Map Made\n";
}

void write_file(const char* in_file, const char* out_file){
    ifstream ifs(in_file, ios::binary);
    ofstream ofs(out_file, ios::binary);

    writev(first_symbols, 13);
    write(bh, ifs, ofs);
    write(selects, chunks, ifs, ofs);
    write_all_the_chunk_info(chunks, ifs, ofs);
    cout << "New file created\n";
}

string make_str(char* arr){
    string s = "";
    for(unsigned int i = 0; i < strlen(arr); i++){
        s += arr[i];
    }
    return s;
}


int main(int argc, char* argv[]){
    char* in_file = new char[100];
    char* out_file = new char[100];
    long long time_start = 0;
    long long time_end = 1e16;
    string topic;
    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i], "--in")){
            strcpy(in_file, argv[++i]);
        }else if(!strcmp(argv[i], "--out")){
            strcpy(out_file, argv[++i]);
        } else if(!strcmp(argv[i], "--time")){
            time_start = atoll(argv[++i]);
            time_end = atoll(argv[++i]);
        } else if(!strcmp(argv[i], "--topic")){
            topic = make_str(argv[++i]);
        }
    }


    read_records(in_file);
    select_data(in_file, time_start, time_end, "");
    write_file(in_file, out_file);
    delete[] in_file;
    delete[] out_file;
    return 0;
}
