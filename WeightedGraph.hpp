#ifndef __WEIGHTED_GRAPH_HPP__
#define __WEIGHTED_GRAPH_HPP__

#include <vector>
#include <map>
#include <queue>
#include <utility>
#include <cassert>

template <typename WeightT>
class AdjVectorWeightedGraph
{
    std::vector <std::map <size_t, WeightT>> neibs;

public:

    struct WeightedEdge;
    struct UnweightedEdge;

    explicit AdjVectorWeightedGraph (size_t count):
        neibs (count) 
    {}

    bool insert (size_t u, size_t v, WeightT weight)
    {
        if (neibs[u].contains(v))
            return false;

        neibs[u][v] = neibs[v][u] = weight;
        return true;
    }

    bool insert (const WeightedEdge &edge)
    {
        return insert (edge.u, edge.v, edge.weight);
    }

    bool erase (size_t u, size_t v)
    {
        auto it = neibs[u].find (v);
        if (it == neibs[u].end ())
            return false;
        neibs[u].erase (it);
        neibs[v].erase (u);
        return true;
    }

    bool erase (const UnweightedEdge &edge)
    {
        return erase (edge.u, edge.v);
    }

    WeightT getWeight (size_t u, size_t v) const
    {
        auto it = neibs[u].find (v);
        if (it == neibs[u].end ())
            return (WeightT) 0;
        return it->second;
    }

    inline size_t size () const
    {
        return neibs.size ();
    }

    inline const std::map <size_t, WeightT> &getNeibs (size_t u) const
    {
        return neibs[u];
    }
};

template <typename WeightT>
struct AdjVectorWeightedGraph<WeightT>::WeightedEdge
{
    size_t u, v;
    WeightT weight;

    operator UnweightedEdge ()
    {
        return {u, v};
    }
};
template <typename WeightT>
struct AdjVectorWeightedGraph<WeightT>::UnweightedEdge
{
    size_t u, v;
};

#endif // ! __WEIGHTED_GRAPH_HPP__