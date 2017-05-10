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
        cout << ifs.tellg() << "\n";
        if(op == 0x02){
            MessageData md;
            ifs >> md;
            cout << "Record Info md " << md.header_len << " " << md.data_len << "\n";
        } else if(op == 0x04){
            IndexData id;
            ifs >> id;
            assert(!chunks.empty());
            cout << "Record Info id " << id.header_len << " " << id.data_len << " " << id.conn << "\n";
            chunks[chunks.size() - 1].indexdata[id.conn] = id;
        } else if(op == 0x06){
            ChunkInfo ci;
            ifs >> ci;
            cout << "Record Info ci " << ci.header_len << " " << ci.data_len << "\n";
            chunkinfo.push_back(ci);
        } else if(op == 0x03){
            ifs >> bh;
            cout << "Record Info bh " << bh.header_len << " " << bh.data_len << "\n";
        } else if(op == 0x05){
            Chunk c;
            ifs >> c;
            cout << "Record Info ch " << c.header_len << " " << c.data_len << "\n";
            chunks.push_back(c);
        } else if(op == 0x07){
            Connection c;
            ifs >> c;
            connections.push_back(c);
            cout << "Record Info con " << c.header_len << " " << c.data_len << "\n";
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
}

void write_file(const char* in_file, const char* out_file){
    ifstream ifs(in_file, ios::binary);
    ofstream ofs(out_file, ios::binary);

    writev(first_symbols, 13);
    cout << "LOL\n";
    write(bh, ifs, ofs);
    write(selects, chunks, ifs, ofs);
    cout << "wow";
    write_all_the_chunk_info(chunks, ifs, ofs);
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
    cout << "Map made\n";
    write_file(in_file, out_file);
    cout << "Writting ended\n";
    delete[] in_file;
    delete[] out_file;
    return 0;
}
