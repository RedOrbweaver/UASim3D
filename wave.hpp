#pragma once
#include "hmain.hpp"



class Wave
{
    private:
    protected:
    public:
    std::vector<node> nodes;
    std::vector<node> mic_nodes;
    std::vector<node> src_nodes;
    std::unordered_map<uint64_t, int> gEdgeMidCache; // edge -> midpoint
};