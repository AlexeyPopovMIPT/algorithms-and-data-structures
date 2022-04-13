/*
 * АВЛ-дерево для решения задачи о динамическом поиске медианы:
 *
 * 
 * Медиана — элемент массива, который находился бы в этом массиве после сортировки на среднем месте. 
 * При нумерации элементов массива размером N с единицы, номер медианного элемента вычисляется по выражению (N+1)/2.
 * Известно, что имеются алгоритмы поиск медианы в массиве с линейной сложностью. 
 * Вам предстоит решать эту задачу много раз, поэтому стоило бы уменьшить сложность этой операции.
 *
 * Формат входных данных
 *
 * Первая строка файла содержит число запросов 5 ⩽ N ⩽ 1000000
 *
 * Запросы могут быть двух типов:
 *
 *  0 X — добавить число X в массив. В массиве может быть произвольное число одинаковых элементов.
 *  1 — вывести медиану массива.
 *
 * Числа X по абсолютной величине не превосходят 1e9. 
 * 
 * Ограничение по времени: 0.7 c (отработал за 0.573 с)
 * Ограничение размера стека: 1 М
 * Ограничение памяти: 64 M
 * 
 */

#include <iostream>
#include <cassert>

template <typename T>
struct AVLTree
{
    struct AVLNode
    {
        T data_;
        AVLNode *left_, *right_;
        struct Count
        {
            int left_, right_;
        }; Count count_;
        int height_;

        AVLNode (const T& value):
            data_ (value),
            left_ (nullptr), right_ (nullptr),
            count_ ({0, 0}),
            height_ (1)
        {} 

        AVLNode (const T& value, const Count& count, int height):
            data_ (value),
            count_ (count),
            height_ (height)
        {}

        int getBalanceFactor() const
        {
            int r = right_ == nullptr ? 0 : right_->height_;
            int l = left_ == nullptr ? 0 : left_->height_;
            return r - l;
        }

        void fix() 
        {
            int r = right_ == nullptr ? 0 : right_->height_;
            int l = left_ == nullptr ? 0 : left_->height_;
            height_ = std::max (r, l) + 1; // (r > l ? r : l) + 1;
            
            // r = right_ == nullptr ? 0 : 1 + right_->count_.left_ + right_->count_.right_;
            // r = left_ == nullptr ? 0 : 1 + left_->count_.left_ + left_->count_.right_;

            // if (l != count_.left_) printf ("Line %d: l = %d, count_.left_ = %d\n", __LINE__, l, count_.left_);
            // if (r != count_.right_) printf ("Line %d: r = %d, count_.right_ = %d\n", __LINE__, r, count_.right_);
        }

        static void rotateRight (AVLNode **root)
        {
            AVLNode *tmp = *root;
            *root = (*root)->left_;
            AVLNode *tmp2 = (*root)->right_;
            (*root)->right_ = tmp;
            (*root)->right_->left_ = tmp2;

            (*root)->count_.right_ += 1 + (*root)->right_->count_.right_;
            (*root)->right_->count_.left_ -= 1 + (*root)->count_.left_;

            //printf ("Before fixing:\n");
            //(*root)->dump (stdout, 0);

            (*root)->right_->fix ();
            (*root)->fix ();

        }

        static void rotateLeft (AVLNode **root)
        {
            AVLNode *tmp = *root;
            *root = (*root)->right_;
            AVLNode *tmp2 = (*root)->left_;
            (*root)->left_ = tmp;
            (*root)->left_->right_ = tmp2;

            (*root)->count_.left_ += 1 + (*root)->left_->count_.left_;
            (*root)->left_->count_.right_ -= 1 + (*root)->count_.right_;

            //printf ("Before fixing:\n");
            //(*root)->dump (stdout, 0);

            (*root)->left_->fix ();
            (*root)->fix ();
        }

        void balance (AVLNode **thisref)
        {
            fix ();
            int balanceFactor = getBalanceFactor ();
            if (balanceFactor <= -2) // assert == -2
            {
                if (right_ && right_->getBalanceFactor () < 0)
                    rotateRight (&right_); 
                return rotateRight (thisref); // если баланс правого отрицателен, так вращать нельзя
            }
            else if (balanceFactor >= 2) // assert == 2
                return rotateLeft (thisref);
        }
        
        void insert (const T& value, AVLNode **thisref)
        {
            if (value < data_)
            {
                // insert to left
                if (left_ == nullptr)
                    left_ = new AVLNode (value);
                else
                    left_->insert (value, &left_);

                count_.left_ ++;
            }
            else
            {
                if (right_ == nullptr)
                    right_ = new AVLNode (value);
                else
                    right_->insert (value, &right_);

                count_.right_ ++;
            }

            balance (thisref);
        }

        const T& findMedian (int less, int greaterEqual) const
        {
            // fprintf (stderr, "Search at %d, less=%d+%d, greater=%d+%d\n", data_, less, count_.left_, greaterEqual, count_.right_);
            int diff = (greaterEqual + count_.right_) - (less + count_.left_);
            if (diff == 0 || diff == 1)
                return data_;

            if (diff > 0)
            {
                // медиану нужно искать справа
                assert (right_);
                less += count_.left_ + 1;
                return right_->findMedian (less, greaterEqual);
            }
            else
            {
                assert (left_);
                greaterEqual += count_.right_ + 1;
                return left_->findMedian (less, greaterEqual);
            }
        }

        void dump (FILE* out, int depth) const
        {
            if (right_) right_->dump (out, depth + 1);
            for (int i = 0; i < depth; i++) 
                fprintf (out, "     ");
            fprintf (out, "[%d l=%d r=%d h=%d]\n", data_, count_.left_, count_.right_, height_);
            if (left_) left_->dump (out, depth + 1);

        }

    };
    AVLNode *root_;

public:

    AVLTree (const T& value): 
        root_ (new AVLNode (value)) 
    {}

    ~AVLTree () 
    {}


    inline void insert (const T& value)
    {
        root_->insert (value, &root_);
    }

    inline const T& findMedian () const
    {
        return root_->findMedian (0, 0);
    }

    void dump (FILE *out) const
    {
        root_->dump (out, 0);
    }

    enum Requests
    {
        ADD = 0,
        FIND_MEDIAN = 1
    };


};

/*int test ()
{
    AVLTree <int> tree (4);

    tree.root_ = new AVLNode<int> {4, {3, 3}, 3};

    tree.root_->left_  = new AVLNode<int> {2, {1, 1}, 2};
    tree.root_->right_ = new AVLNode<int> {6, {1, 1}, 2};

    tree.root_->left_->left_   = new AVLNode<int> {1, {0, 0}, 1};
    tree.root_->left_->right_  = new AVLNode<int> {3, {0, 0}, 1};
    tree.root_->right_->left_  = new AVLNode<int> {5, {0, 0}, 1};
    tree.root_->right_->right_ = new AVLNode<int> {7, {0, 0}, 1};

    AVLNode<int>::rotateLeft  (&(tree.root_));
    tree.dump (stdout);

    AVLNode<int>::rotateRight (&(tree.root_));
    tree.dump (stdout);

    return 0;
}*/


int main ()
{
    int n;
    scanf ("%d", &n);
    
    int type, arg;
    scanf ("%d %d", &type, &arg);

    AVLTree <int> tree (arg);

    for (int i = 1; i < n; i++)
    {
        scanf ("%d", &type);
        if (type == AVLTree<int>::Requests::FIND_MEDIAN)
            printf ("%d\n", tree.findMedian ());
        else if (type == AVLTree<int>::Requests::ADD)
        {
            scanf ("%d", &arg);
            tree.insert (arg);
        }

    }

}
