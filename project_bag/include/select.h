#include "records.h"

class Select{
public:
    std::map <long long, bool> valid;

    void make_map(long long time_start, long long time_end, std::string topic, std::vector<Chunk>& chunks, std::ifstream& ifs);
    void make_map(long long time_start, long long time_end, std::string topic, Chunk& c, std::ifstream& ifs);
    void sift_index_data(long long time_start, long long time_end, Connection& c, std::ifstream& ifs);
};
