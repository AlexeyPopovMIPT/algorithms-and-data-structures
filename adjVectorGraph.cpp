/*
 * Граф, представленный вектором смежности, с реализованными алгоритмами DFS, BFS, topo sort
 */
#include <vector>
#include <queue>
#include <set>
#include <stack>
#include <cstdio>

class AdjVectorGraph
{
    std::vector <std::set <int>> neibs;

public:

    explicit AdjVectorGraph (size_t count):
        neibs (count) 
    {}

    bool insert (size_t u, size_t v)
    {
        if (neibs[u].contains(v))
            return false;

        neibs[u].insert (v);
        return true;
    }

    bool erase (size_t u, size_t v)
    {
        if (!neibs[u].contains(v))
            return false;

        neibs[u].erase (v);
        return true;
    }

    bool contains (size_t u, size_t v) const
    {
        return neibs[u].contains(v);
    }

    inline size_t size () const
    {
        return neibs.size ();
    }

    inline const std::set <int> &getNeibs (size_t u) const
    {
        return neibs[u];
    }

};


template <typename Func>
void bfs (const AdjVectorGraph &g, int s, Func &func)
{
    enum State { UNVISITED, VISITED };

    std::vector <State> state (g.size(), UNVISITED);
    std::queue <int> queue;

    state[s] = VISITED;
    queue.push (s);

    while (!queue.empty ())
    {
        int u = queue.front (); queue.pop ();

        for (auto v: g.getNeibs(u))
        {
            if (state[v] == VISITED)
                continue;

            func (v, u);

            state [v] = VISITED;
            queue.push (v);
        }

        state [u] = VISITED;
    }

}

// funcEnter returns 1 if no further search required and vertex is to be closed immediately,
// returns 0 if further search is required
template <typename FuncEnter, typename FuncExit>
void dfs (const AdjVectorGraph &g, size_t s, FuncEnter &funcEnter, FuncExit &funcExit)
{
    enum State { UNVISITED, VISITED };
    const size_t MARK = (size_t)-1;

    std::stack <size_t> stk;
    std::vector <State> state (g.size(), UNVISITED);

    stk.push (MARK);
    stk.push (s);

    while (!stk.empty())
    {
        size_t u = stk.top (); stk.pop ();
        state[u] = VISITED;

        if (stk.empty () || stk.top () != MARK)
        {
            funcExit (u);
            continue;
        }

        stk.pop ();
        stk.push (u);

        if (funcEnter (u))
            continue;
            
        for (auto v: g.getNeibs(u))
        {
            if (state[v] == UNVISITED)
            {
                stk.push (MARK);
                stk.push (v);
            }
        }
    }
}

// topo sort of DAG g (if not acyclic, gigo)
std::vector <size_t> topoSort (const AdjVectorGraph &g)
{
    struct OnEntry
    {
        enum Color { BLACK, WHITE };
        std::vector <Color> color;

        OnEntry (size_t n):
            color (n, WHITE)
        {}

        bool operator () (size_t u)
        {
            printf ("%lu entered\n", u);
            if (color [u] == BLACK) return true;
            color [u] = WHITE;
            return false;
        }

    };

    struct OnExit
    {
        enum Color { BLACK, WHITE };
        std::vector <size_t> answer;
        size_t time;

        OnExit (size_t n):
            answer (n, (size_t)-1),
            time (0)
        {}

        bool operator () (size_t u)
        {
            if (answer [u] != (size_t)-1) return true;
            answer [u] = time++;
            return false;
        }
    };

    size_t n = g.size ();
    OnEntry onEntry (n);
    OnExit onExit (n);

    for (size_t vertex = 0; vertex < n; ++vertex)
    {
        if (onExit.answer [vertex] == (size_t)-1)
            dfs (g, vertex, onEntry, onExit);
    }

    return onExit.answer;
}

void bfsDemo ()
{
    struct Say
    {
        void operator () (int vertex, int parent)
        {
            printf ("vertex %d, parent %d\n", vertex, parent);
        }
    };

    struct GetMinDistance
    {
        std::vector <int> distances;

        GetMinDistance (size_t n, size_t vertexFrom):
            distances (n, -1)
        {
            distances[vertexFrom] = 0;
        }

        void operator () (int vertex, int parent)
        {
            distances [vertex] = distances [parent] + 1;
        }
    };

    AdjVectorGraph g (8);
    g.insert (0, 1);
    g.insert (0, 2);
    g.insert (1, 3);
    g.insert (1, 4);
    g.insert (2, 5);
    g.insert (2, 6);

    printf ("Vertexes in order they are visited from v = 0:\n");
    Say say;
    bfs <Say> (g, 0, say);

    printf ("Minimal distances from v = 0:\n");
    GetMinDistance counter (8, 0);
    bfs <GetMinDistance> (g, 0, counter);

    for (int i = 0; i < 8; ++i)
    {
        printf ("%d: %d\n", i, counter.distances[i]);
    }

}

void dfsDemo ()
{
    struct EnterTime
    {
        size_t *time;

        bool operator () (int vertex)
        {
            printf ("%d entry at t=%lu\n", vertex, (*time)++);
            return false;
        }
    };

    struct ExitTime
    {
        size_t *time;

        void operator () (int vertex)
        {
            printf ("%d exit  at t=%lu\n", vertex, (*time)++);
        }
    };

    AdjVectorGraph g (8);
    g.insert (0, 1);
    g.insert (0, 2);
    g.insert (1, 3);
    g.insert (1, 4);
    g.insert (2, 5);
    g.insert (2, 6);

    size_t time  = 0;
    EnterTime enterTime { &time };
    ExitTime exitTime { &time };
    
    printf ("Vertexes in order they are visited from v = 0:\n");
    dfs (g, 0, enterTime, exitTime);
}

void topoSortDemo ()
{
    AdjVectorGraph g (5);
    g.insert (0, 1);
    g.insert (1, 2);
    g.insert (2, 3);
    g.insert (3, 4);
    g.insert (0, 2);
    g.insert (0, 3);
    g.insert (2, 4);

    std::vector <size_t> sorted = topoSort (g);

    for (auto ord: sorted)
        printf ("%lu\n", ord);

}

int main ()
{
    topoSortDemo ();
}

