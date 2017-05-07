#include "..\include\records.h"

using namespace std;


int main(int argc, char** argv){
    vector <Chunk> chunks;
    vector <Connection> connections;
    std::ifstream ifs("example.bag", std::ios::binary);
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
            cout << "Record Info id " << id.header_len << " " << id.data_len << "\n";
        } else if(op == 0x06){
            ChunkInfo ci;
            ifs >> ci;
            cout << "Record Info ci " << ci.header_len << " " << ci.data_len << "\n";
        } else if(op == 0x03){
            BagHeader bh;
            ifs >> bh;
            cout << "Record Info bh " << bh.header_len << " " << bh.data_len << "\n";
        } else if(op == 0x05){
            Chunk c;
            ifs >> c;
            cout << "Record Info ch " << c.header_len << " " << c.data_len << "\n";
        } else if(op == 0x07){
            Connection c;
            ifs >> c;
            connections.push_back(c);
            cout << "Record Info con " << c.header_len << " " << c.data_len << "\n";
        }
    }
    cout << "Connections count (vector size): " << connections.size() << "\n";
    cout << "Chunks count (vector size): " << chunks.size() << "\n";
    return 0;
}
