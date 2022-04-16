#include "WeightedGraph.hpp"
#include <limits>
#include <cstdio>

// Prime algo
template <typename WeightT>
WeightedGraph<WeightT> getMST (const WeightedGraph<WeightT>& g)
{
    using edge_t = WeightedGraph<WeightT>::WeightedEdge;

    struct oq_compare
    {
        bool operator () (const edge_t &lhs, const edge_t &rhs)
        {
            return lhs.weight > rhs.weight;
        }
    };

    std::priority_queue <edge_t, std::vector<edge_t>, oq_compare> qu;

    WeightedGraph<WeightT> mst (g.size ());
    
    // Выбираем произвольную вершину (нулевую) и кладём в очередь все инцидентные ей рёбра
    {
        size_t u = 0;
        for (auto [v, weight]: g.getNeibs(u))
            qu.push ({u, v, weight});
    }

    // Выбираем самое лёгкое ребро из очереди и добавляем его в MST
    edge_t light = qu.top (); qu.pop ();
    mst.insert (light);

    // Выбираем вершину которой инцидентно ребро (не нулевую) и добавляем в очередь все её рёбра
    size_t vertex = std::max (light.u, light.v);
    for (auto [newU, newWeight]: g.getNeibs(vertex))
        if (newU != 0)
            qu.push ({vertex, newU, newWeight});
        

    // Пока дерево не покрыло весь граф, ...
    for (size_t mstEdges = 1, end = g.size () - 1; mstEdges < end;)
    {
        // Выбираем самое лёгкое ребро из очереди и добавляем его в MST
        auto [u, v, weight] = qu.top (); qu.pop ();
        bool uIsNew = mst.getNeibs(u).empty();
        bool vIsNew = mst.getNeibs(v).empty();
        mst.insert (u, v, weight);
        ++mstEdges;

        // Если оно идёт в новую вершину (одна из u, v новая для MST), добавляем все рёбра, смежные с ней, в MST
        if (uIsNew)
        {
            for (auto [newV, newWeight]: g.getNeibs(u))
                if (mst.getNeibs(newV).empty())
                    qu.push ({u, newV, newWeight});
            continue;
        }
        if (vIsNew)
        {
            for (auto [newU, newWeight]: g.getNeibs(v))
                if (mst.getNeibs(newU).empty())
                    qu.push ({v, newU, newWeight});
            continue;
        }
        // если ни одна из вершин u,v в MST в графе не новая, удаляем самое тяжёлое ребро из инцидентных им,
        // чтобы не было цикла
        edge_t max_edge { };
        max_edge.weight = std::numeric_limits<WeightT>::min ();

        for (auto [otherU, otherWeight]: mst.getNeibs(v))
            if (max_edge.weight < otherWeight)
                max_edge = {v, otherU, otherWeight};
        
        for (auto [otherV, otherWeight]: mst.getNeibs(u))
            if (max_edge.weight < otherWeight)
                max_edge = {u, otherV, otherWeight};

        mst.erase (max_edge);
        --mstEdges;
    }

    return mst;
}


int main ()
{ 
    WeightedGraph<int> g (8);
    g.insert (0, 2, 7);
    g.insert (0, 1, 10);
    g.insert (0, 5, 26);
    g.insert (0, 6, 20);
    g.insert (0, 7, 9);
    g.insert (4, 6, 30);
    g.insert (4, 7, 22);
    g.insert (4, 3, 11);
    g.insert (4, 5, 15);
    g.insert (1, 7, 7);
    g.insert (6, 7, 10);
    g.insert (3, 5, 4);

    auto mst = getMST (g);
    for (size_t i = 0; i < 8UL; ++i)
    {
        for (auto u: mst.getNeibs(i))
        {
            if (u.first > i) printf ("%lu %lu\n", i, u.first);
        }
    }
    
}

