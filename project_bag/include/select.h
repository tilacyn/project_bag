#include "records.h"

class Select{
public:
    std::map <int, int> new_offset;
    void make_map(long long time_start, long long time_end, std::string topic, Chunk& c, std::ifstream& ifs);
    void sift_index_data(long long chunk_data_start, long long time_start, long long time_end, Connection& c, std::ifstream& ifs);
};

void make_map(long long time_start, long long time_end, std::string topic, std::vector<Chunk>& chunks, std::vector<Select>& selects,
               BagHeader& bh, std::ifstream& ifs);
