#pragma once
#include <functional>
#include <vector>
#include <string>
#include <regex>
#include "Numerics.h"

namespace Starsurge {
    static char* WHITESPACE = " \t\n\r\f\v"; // Why does static instead of const fix this?

    std::vector<std::string> Explode(std::string const & str, char delim, bool include_empty = true);
    std::vector<std::string> Explode(std::string const & str, std::string delim, bool include_empty = true);
    std::vector<std::string> ExplodeWhitespace(std::string const & str, bool include_empty = true);
    std::string LTrim(std::string const str);
    std::string RTrim(std::string const str);
    std::string Trim(std::string const str);

    std::string Replace(std::string const str, std::string regex, const std::string & to);
    void PrintCode(std::string code);

    // Select various elements from an tuple
    template <typename Tuple, size_t... Idx>
    auto Select(Tuple&& tuple, std::index_sequence<Idx...>) {
        return std::make_tuple(std::get<Idx>(std::forward<Tuple>(tuple))...);
    }

    struct Match {
        unsigned int position;
        std::vector<std::string> matches;
        std::string prefix;
        std::string suffix;
    };

    std::vector<Match> FindSubstrings(std::string str, std::string regex);
    Match FindFirstSubstrings(std::string str, std::string regex);
    Match FindLastSubstrings(std::string str, std::string regex);

    template<typename T>
    void Swap(T & a, T & b) {
        T temp = a;
        a = b;
        b = temp;
    }

    template<typename T>
    bool ElemOf(const T array[], const unsigned int arraySize, const T query) {
        int index = Index<T>(array, arraySize, query);
        return (index >= 0);
    }

    template<typename T>
    int Index(const T array[], const unsigned int arraySize, const T query) {
        for (unsigned int i = 0; i < arraySize; ++i) {
            if (array[i] == query) {
                return i;
            }
        }
        return -1;
    }

    template<typename T>
    void Sort(T array[], const unsigned int arraySize) {
        Sort<T>(array, arraySize, [](T a, T b) { return a < b; });
    }

    template<typename T>
    void Sort(T array[], const unsigned int arraySize, std::function<bool(T,T)> test) {
        // Insertion sort.
        for (unsigned int i = 1; i < arraySize; ++i) {
            int j = i;
            while (j > 0 && !test(array[j-1], array[j])) {
                Swap(array[j-1], array[j]);
                j -= 1;
            }
        }
    }

    // Returns true if a is sublist of b.
    template<typename T>
    bool IsSublist(const T a[], const unsigned int a_size, const T b[], const unsigned int b_size) {
        // Quick sanity check. The array a cannot be a subset of b if it's longer.
        if (a_size > b_size)
            return false;
        if (a_size == 0)
            return true;

        unsigned int j = 0;
        for (unsigned int i = 0; i < b_size; ++i) {
            if (b[i] == a[j]) {
                j += 1;
                if (j == a_size) {
                    return true;
                }
            }
            else {
                j = 0;
            }
        }
        return false;
    }

    template<typename T>
    unsigned int AllSublists(const T array[], const unsigned int arraySize, unsigned int * &listSizes, T ** &lists) {
        const unsigned int nlists = (arraySize*(arraySize+1))/2;
        unsigned int * newListSizes = new unsigned int[nlists];
        T ** newLists = new T*[nlists];
        unsigned int index = 0;

        // Add the empty list.
        newSetSizes[index] = 0;
        newSets[index] = NULL;
        index += 1;

        // Increment the size of the subarray.
        for (unsigned int i = 1; i <= arraySize; ++i) {
            // Increment where the subarray starts, ensuring that there is enough space.
            for (unsigned int j = 0; j + i <= arraySize; ++j) {
                if (index >= nlists) continue; // Safety.

                newListSizes[index] = i;
                newLists[index] = new T[i];
                for (unsigned int k = 0; k < i; ++k) {
                    newLists[index][k] = array[j+k];
                }
                index += 1;
            }
        }
        if (listSizes) {
            delete [] listSizes;
        }
        if (lists) {
            delete [] lists;
        }
        listSizes = newListSizes;
        lists = newLists;
        return index;
    }

    // Returns true if a is subset of b.
    template<typename T>
    bool IsSubset(const T a[], const unsigned int a_size, const T b[], const unsigned int b_size) {
        // Quick sanity check. The array a cannot be a subset of b if it's longer.
        if (a_size > b_size)
            return false;

        std::vector<unsigned int> skip;
        for (unsigned int i = 0; i < a_size; ++i) {
            if (ElemOf<unsigned int>(skip, i))
                continue;

            // Determine how many times a[i] occurs in a.
            unsigned int count = 1;
            for (unsigned int j = i+1; j < a_size; ++j) {
                if (a[i] == a[j]) {
                    count += 1;
                    skip.push_back(j); // Don't check items multiple times.
                }
            }

            // Check if a[i] occurs at least count times in b.
            for (unsigned int j = 0; j < b_size; ++j) {
                if (a[i] == b[j]) {
                    count -= 1;
                    if (count == 0)
                        break;
                }
            }
            if (count > 0)
                return false;
        }
        return true;
    }

    template<typename T>
    unsigned int PowerSet(const T array[], const unsigned int arraySize, unsigned int * &setSizes, T ** &sets) {
        const unsigned int nsets = Pow(2, arraySize);
        unsigned int * newSetSizes = new unsigned int[nsets];
        T ** newSets = new T*[nsets];
        unsigned int index = 0;

        // Add the empty set.
        newSetSizes[index] = 0;
        newSets[index] = NULL;
        index += 1;

        // Increment the size of the subarray.
        for (unsigned int i = 1; i <= arraySize; ++i) {
            for (unsigned int j = 0; j + i <= arraySize; ++j) {
                if (i > 1) {
                    unsigned int * temp1 = NULL;
                    T ** temp2 = NULL;
                    unsigned int temp3 = PowerSet(&array[j+1], arraySize-j-1, temp1, temp2);
                    for (unsigned int k = 0; k < temp3; ++k) {
                        if (temp1[k] != i-1)
                            continue;
                        newSetSizes[index] = i;
                        newSets[index] = new T[i];
                        newSets[index][0] = array[j];
                        for (unsigned int l = 0; l < temp1[k]; ++l) {
                            newSets[index][l+1] = temp2[k][l];
                        }
                        index += 1;
                    }
                    delete [] temp1;
                    delete [] temp2;
                }
                else {
                    newSetSizes[index] = i;
                    newSets[index] = new T[i];
                    newSets[index][0] = array[j];
                    index += 1;
                }
            }
        }

        // Save the arrays
        if (setSizes) {
            delete [] setSizes;
        }
        if (sets) {
            delete [] sets;
        }
        setSizes = newSetSizes;
        sets = newSets;
        return index;
    }

    template<typename T>
    bool ElemOf(const std::vector<T> & v, T query) {
        int index = Index<T>(v, query);
        return (index >= 0);
    }

    template<typename T>
    int Index(const std::vector<T> & v, T query) {
        if (v.size() == 0) {
            return -1;
        }

        return Index<T>(&v[0], v.size(), query);
    }

    template<typename T>
    void Sort(std::vector<T> & v) {
        Sort<T>(v, [](T a, T b) { return a < b; });
    }

    template<typename T>
    void Sort(std::vector<T> & v, std::function<bool(T,T)> test) {
        if (v.size() > 0) {
            Sort(&v[0], v.size(), test);
        }
    }

    // Returns true if a is sublist of b.
    template<typename T>
    bool IsSublist(const std::vector<T> & a, const std::vector<T> & b) {
        if (a.size() == 0 || b.size() == 0) {
            return (a.size() <= b.size());
        }
        return IsSublist<T>(&a[0], a.size(), &b[0], b.size());
    }

    template<typename T>
    std::vector<std::vector<T>> AllSublists(const std::vector<T> & v) {
        std::vector<std::vector<T>> ret;
        if (v.size() == 0) {
            std::vector<T> emptyList;
            ret.push_back(emptyList);
            return ret;
        }

        unsigned int * listSizes = NULL;
        int ** lists = NULL;
        unsigned int nlists = AllSublists<int>(&v[0], v.size(), listSizes, lists);

        for (unsigned int i = 0; i < nlists; ++i) {
            std::vector<T> list;
            for (unsigned int j = 0; j < listSizes[i]; ++j) {
                list.push_back(lists[i][j]);
            }
            ret.push_back(list);
        }
        return ret;
    }

    // Returns true if a is subset of b.
    template<typename T>
    bool IsSubset(const std::vector<T> & a, const std::vector<T> & b) {
        if (a.size() == 0 || b.size() == 0) {
            return (a.size() <= b.size());
        }
        return IsSubset<T>(&a[0], a.size(), &b[0], b.size());
    }

    template<typename T>
    std::vector<std::vector<T>> PowerSet(const std::vector<T> & v) {
        std::vector<std::vector<T>> ret;
        if (v.size() == 0) {
            std::vector<T> emptyList;
            ret.push_back(emptyList);
            return ret;
        }

        unsigned int * listSizes = NULL;
        int ** lists = NULL;
        unsigned int nlists = PowerSet<int>(&v[0], v.size(), listSizes, lists);

        for (unsigned int i = 0; i < nlists; ++i) {
            std::vector<T> list;
            for (unsigned int j = 0; j < listSizes[i]; ++j) {
                list.push_back(lists[i][j]);
            }
            ret.push_back(list);
        }
        return ret;
    }
}
