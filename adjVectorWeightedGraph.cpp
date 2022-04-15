#include <vector>
#include <queue>
#include <map>
#include <stack>
#include <utility>
#include <cstdio>

template <typename WeightT>
class AdjVectorWeightedGraph
{
    struct Node;
    std::vector <std::map <size_t, WeightT>> neibs;

public:

    explicit AdjVectorWeightedGraph (size_t count):
        neibs (count) 
    {}

    bool insert (size_t u, size_t v, WeightT weight)
    {
        if (neibs[u].contains(v))
            return false;

        neibs[u][v] = weight;
        return true;
    }

    bool erase (size_t u, size_t v)
    {
        auto it = neibs[u].find (v);
        if (it == neibs[u].end ())
            return false;
        neibs[u].erase (it);
        return true;
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

    inline const auto &getNeibs (size_t u) const
    {
        return neibs[u];
    }
};


int main ()
{
    AdjVectorWeightedGraph<int> g (8);
    g.insert (0, 1, 1);
    g.insert (1, 2, 8);
    g.erase (0, 1);
    printf ("%d %d\n", g.getWeight (0, 1), g.getWeight (1, 2));
}

