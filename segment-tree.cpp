/* 
 * Использование дерева отрезков для того, что я напишу потом. полпятого ночи.
 * Достаточно легко быстро выполнить операцию XOR над всеми элементами массива или над его срезом. 
 * Можно ли это делать быстро, если элементы массива постоянно меняются?
 * Формат входных данных

 * В первой строке файла содержатся два числа: количество элементов в массиве V: 5⩽N⩽500000 и количество запросов 1⩽M⩽500000. 
 * Каждый элемент массива имеет тип данных int.

 * Каждый запрос — отдельная строка, состоящая из кода запроса, который может быть равен 1 или 2 и аргументов запроса.

 * Запрос с кодом один содержит два аргумента: начало 0⩽L<N и конец отрезка 0⩽R<N подмассива, L⩽R.

 * В ответ на этот запрос программа должна вывести значение операции XOR над элементами массива от L до R вкючительно, 
 * то есть, отрезка, закрытого на левом и правом концах.

 * Запрос с кодом два тоже содержит два аргумента, первый из которых есть номер элемента массива V, а второй — его новое значение. 
 */

#include <iostream>


template <typename T, typename Oper, typename Neutral>
class SegmentTree
{
    T *tree_;
    unsigned size_;
    unsigned round_size_;
    Oper oper_;
    Neutral neutral_;

    unsigned roundUp (unsigned value)
    {
        unsigned ret;
        while (value > 0)
        {
            ret = value;
            value &= value - 1;
        }
        return ret << 1;
    }

    T recurseGet (int l, int r, unsigned level)
    {
        T result = neutral_ ();
        if (l % 2 == 1) result = oper_ (result, tree_[level + l++]);
        if (r % 2 == 0) result = oper_ (result, tree_[level + r--]);
        if (l < r) result = oper_ (result, recurseGet (l/2, r/2, level/2));
        return result;
    }

public:

    SegmentTree (unsigned array_size):
        size_ (array_size)
    {
        round_size_ = roundUp (array_size);
        tree_ = new T [2 * round_size_];
    }

    ~SegmentTree () { delete[] tree_; }

    void fillFrom (std::istream &is)
    {
        int i;
        for (i = round_size_; i < round_size_ + size_; i++)
            is >> tree_[i];

        for (; i < 2 * round_size_; i++)
            tree_[i] = neutral_ ();

        for (i = round_size_ - 1; i > 0; i--)
            tree_[i] = oper_ (tree_[2 * i], tree_[2 * i + 1]);
    }

    T getResultAtRange (int l, int r)
    {
        return recurseGet (l, r, round_size_);
    }

    void change (int idx, T new_value)
    {
        int index = idx + round_size_;
        tree_ [index] = new_value;

        for (index /= 2; index > 0; index /= 2)
        {
            tree_ [index] = oper_ (tree_[2 * index], tree_[2 * index + 1]);
        }
    }

    void dump (std::ostream &os)
    {
        for (int i = 0; i < 2 * round_size_; i++)
        {
            os << tree_[i] << " ";
        }
        os << "\n";
    }

    T getResultOf (int lhs, int rhs)
    {
        return oper_ (tree_[lhs], tree_[rhs]);
    }
};

template <typename T>
struct xor_tr
{
    T operator() (T lhs, T rhs)
    {
        return lhs ^ rhs;
    }
};

template <typename T>
struct neutral
{
    T operator () ()
    {
        return (T) 0;
    }
};


int main ()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie (0);
    int v, m;
    std::cin >> v >> m;

    SegmentTree <int, xor_tr<int>, neutral<int>> tr (v);
    tr.fillFrom (std::cin);

    for (int i = 0; i < m; i++)
    {
        int k, arg1, arg2;
        std::cin >> k >> arg1 >> arg2;
        if (k == 1)
            std::cout << tr.getResultAtRange (arg1, arg2) << "\n";
        else
            tr.change (arg1, arg2);
    }

    return 0;
}