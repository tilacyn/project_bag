#include "write.h"

using namespace std;


int main(int argc, char* argv[]){

    /*
    map <int, int> mapka;
    mapka[0] = 0, mapka[4] = 40, mapka[6] = -120;
    sort(mapka.begin(), mapka.end());
    for(std::map<int, int>::iterator i = mapka.begin(); i < mapka.end(); i++){
        //std::cout << i->first << " " << i->second << "\n";

    }



    assert(0);
    */
    vector <Chunk> chunks;
    vector <Connection> connections;
    vector <ChunkInfo> chunkinfo;
    vector <IndexData> indexdata;
    std::ifstream ifs("example.bag", std::ios::binary);
    std::cout << "IFS ZAVEDEN\n";
    ifs.ignore(13);
    /*
    BagHeader bh;
    ifs >> bh;
    std::cout << "Header len " << bh.header_len << "\n";
    std::cout << "Data len" << bh.data_len << "\n";
    std::cout << "Index pos " << bh.index_pos << "\n";
    std::cout << "Chunk count " << bh.chunk_count << "\n";
    std::cout << "Conn count " << bh.conn_count << "\n";
    */
    while(ifs.peek() != std::ifstream::traits_type::eof()){
        char op = read_op(ifs);
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
            BagHeader bh;
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
            //c.skip_header(ifs);
            //c.skip_data(ifs);
            connections.push_back(c);
            cout << "Record Info con " << c.header_len << " " << c.data_len << "\n";
        }
    }
   // std::ifstream ifs1("first_part_of_maze.bag", std::ios::binary);
    std::cout << ifs.tellg() << "\n";
    seq_chunk_to_info(chunks, chunkinfo);

    cout << chunks.size() << "\n";
    ifstream ifs1("example.bag", std::ios::binary);
    for(unsigned int i = 0; i < chunks.size(); i++){
        chunks[i].seq_id_to_conn(ifs1);
    }

    long long time_start = 0;
    long long time_end = 1e16;
    Select s;
    s.make_map(time_start, time_end, "", chunks, ifs1);
    return 0;
}
