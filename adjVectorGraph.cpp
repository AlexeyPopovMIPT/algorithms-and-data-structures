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

            funcEnter (u);
            
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


void dfsDemo ()
{
    struct EnterTime
    {
        size_t *time;

        void operator () (int vertex)
        {
            printf ("%d entry at t=%lu\n", vertex, (*time)++);
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

int main ()
{
}

