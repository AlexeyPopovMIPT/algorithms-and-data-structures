/*
 * Граф, представленный вектором смежности, с реализованными алгоритмами DFS, BFS
 */
#include <vector>
#include <queue>
#include <set>
#include <stack>
#include <functional>
#include <cstdio>

class AdjVectorGraph
{
    std::vector <std::set <int>> neibs;

    enum State { UNVISITED, VISITED };

public:

    explicit AdjVectorGraph (size_t count):
        neibs (count) 
    {}

    bool insertEdge (size_t u, size_t v)
    {
        if (neibs[u].contains(v))
            return false;

        neibs[u].insert (v);
        return true;
    }

    bool eraseEdge (size_t u, size_t v)
    {
        if (!neibs[u].contains(v))
            return false;

        neibs[u].erase (v);
        return true;
    }

    template <typename Func>
    void bfs (int s, Func &func) const
    {
        std::vector <State> state (neibs.size(), UNVISITED);
        std::queue <int> queue;

        state[s] = VISITED;
        queue.push (s);

        while (!queue.empty ())
        {
            int u = queue.front (); queue.pop ();

            for (auto v: neibs [u])
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
    void dfs (size_t s, FuncEnter &funcEnter, FuncExit &funcExit) const
    {
        const size_t MARK = (size_t)-1;
        std::stack <size_t> stk;
        std::vector <State> state (neibs.size(), UNVISITED);

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
            
            for (auto v: neibs[u])
            {
                if (state[v] == UNVISITED)
                {
                    stk.push (MARK);
                    stk.push (v);
                }
            }
        }
    }

    inline size_t size () const
    {
        return neibs.size ();
    }


};

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
    g.insertEdge (0, 1);
    g.insertEdge (0, 2);
    g.insertEdge (1, 3);
    g.insertEdge (1, 4);
    g.insertEdge (2, 5);
    g.insertEdge (2, 6);

    printf ("Vertexes in order they are visited from v = 0:\n");
    Say say;
    g.bfs <Say> (0, say);

    printf ("Minimal distances from v = 0:\n");
    GetMinDistance counter (8, 0);
    g.bfs <GetMinDistance> (0, counter);

    for (int i = 0; i < 8; ++i)
    {
        printf ("%d: %d\n", i, counter.distances[i]);
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
            g.dfs (vertex, onEntry, onExit);
    }

    return onExit.answer;
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
    g.insertEdge (0, 1);
    g.insertEdge (0, 2);
    g.insertEdge (1, 3);
    g.insertEdge (1, 4);
    g.insertEdge (2, 5);
    g.insertEdge (2, 6);

    size_t time  = 0;
    EnterTime enterTime { &time };
    ExitTime exitTime { &time };
    
    printf ("Vertexes in order they are visited from v = 0:\n");
    g.dfs (0, enterTime, exitTime);
}

void topoSortDemo ()
{
    AdjVectorGraph g (5);
    g.insertEdge (0, 1);
    g.insertEdge (1, 2);
    g.insertEdge (2, 3);
    g.insertEdge (3, 4);
    g.insertEdge (0, 2);
    g.insertEdge (0, 3);
    g.insertEdge (2, 4);

    std::vector <size_t> sorted = topoSort (g);

    for (auto ord: sorted)
        printf ("%lu\n", ord);

}

int main ()
{
    topoSortDemo ();
}

