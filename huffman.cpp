/*
 * Демонстрация алгоритма кодирования Хаффмана
 */


#include <iostream>
#include <string>
#include <queue>
#include <vector>

using std::vector, std::string, std::priority_queue;

struct Node
{
    int sym = -1;
    size_t freq = 0;
    Node *left = nullptr, *right = nullptr;
};

struct pq_less 
{
    bool operator() (const Node *lhs, const Node *rhs)
    {
        return lhs->freq > rhs->freq;
    }
};

void walk (const Node *node, const string &s)
{
    if (node->left  != nullptr) walk (node->left , s + "0");
    if (node->right != nullptr) walk (node->right, s + "1");
    if (node->sym >= 0)
        printf ("%c: \'%s\'\n", node->sym, s.c_str ());
}

vector <string> huf (const string &s)
{
    vector <string> ret;
    vector <int> freq (256);

    for (auto c: s)
    {
        freq [c & 0xFF] ++;
    }

    vector <Node*> nodes;
    priority_queue <Node*, vector <Node*>, pq_less> pq;

    for (int sym = 0; sym < 256; sym++)
    {
        if (freq[sym] > 0)
        {
            Node *n = new Node ();
            n->sym = sym;
            n->freq = freq [sym];
            pq.push (n);
        }
    }

    while (pq.size () > 1)
    {
        auto el1 = pq.top (); pq.pop ();
        auto el2 = pq.top (); pq.pop ();
        auto par = new Node {-1, el1->freq + el2->freq, el1, el2};
        pq.push (par);
        // на практике не хранятся сами коды - хранятся длины
    }

    auto root = pq.top (); pq.pop ();
    
    walk (root, "");

    return ret;
}


int main ()
{
    auto res = huf ("abrashvabracadabra");

    for (size_t i = 0; i < res.size(); i++)
    {
        auto const & c = res[i];
        if (!c.empty ())
            printf ("%c: \'%s\'\n", static_cast <char> (i), c.c_str ());
    }
}
