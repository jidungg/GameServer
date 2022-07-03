#pragma once
#include "Types.h"
#include "Allocator.h"
#include <vector>
#include <list>
#include <array>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
using namespace std;

template<typename Type, uint32 Size>
using Array = array<Type, Size>;

template<typename T>
using Vector = vector<T, STLAllocator<T>>;

template<typename T>
using List = list<T, STLAllocator<T>>;

template<typename Key, typename Type, typename Pred= less<Key>>
using Map = map<Key, Type, Pred, STLAllocator<pair<const Key, Type>> >;

template<typename Key, typename Pred = less<Key>>
using Set = set<Key, Pred, STLAllocator<Key>>;

template<typename T>
using Deque = deque<T, STLAllocator<T>>;

template<typename T, typename Container = Deque<T>>
using Queue = queue<T,Container>;

template<typename T, typename Container = Deque<T>>
using Stack = stack<T, Container>;

template<typename T, typename Container = Vector<T>, typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<T, Container,Pred>;

template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashMap = unordered_map<Key, Type, Hasher, KeyEq, STLAllocator<pair<const Key, Type>>>;

template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashSet = unordered_set<Key, Hasher, KeyEq, STLAllocator<Key>>;

using String = basic_string<char, char_traits<char>, STLAllocator<char>>;
using WString = basic_string<wchar_t, char_traits<wchar_t>, STLAllocator<wchar_t>>;