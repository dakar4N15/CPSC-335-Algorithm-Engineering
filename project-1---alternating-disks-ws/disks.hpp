///////////////////////////////////////////////////////////////////////////////
// disks.hpp
// 
// Definitions for two algorithms that each solve the alternating disks
// problem.
//
// As provided, this header has four functions marked with TODO comments.
// You need to write in your own implementation of these functions.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// TODO
#include <functional>
#include <iostream>

enum disk_color { DISK_DARK, DISK_LIGHT};

class disk_state {
private:
  std::vector<disk_color> _colors;

public:
  disk_state(size_t light_count)
    : _colors(light_count * 2, DISK_DARK) {

    assert(light_count > 0);

    for (size_t i = 0; i < _colors.size(); i += 2) {
      _colors[i] = DISK_LIGHT;
    }
  }

  bool operator== (const disk_state& rhs) const {
    return std::equal(_colors.begin(), _colors.end(), rhs._colors.begin());
  }

  size_t total_count() const {
    return _colors.size();
  }

  size_t light_count() const {
    return total_count() / 2;
  }

  size_t dark_count() const {
    return light_count();
  }

  bool is_index(size_t i) const {
    return (i < total_count());
  }

  disk_color get(size_t index) const {
    assert(is_index(index));
    return _colors[index];
  }

  void swap(size_t left_index) {
    assert(is_index(left_index));
    auto right_index = left_index + 1;
    assert(is_index(right_index));
    std::swap(_colors[left_index], _colors[right_index]);
  }

  std::string to_string() const {
    std::stringstream ss;
    bool first = true;
    for (auto color : _colors) {
      if (!first) {
        ss << " ";
      }

      if (color == DISK_LIGHT) {
        ss << "L";
      } else {
        ss << "D";
      }

      first = false;
    }
    return ss.str();
  }

  // Return true when this disk_state is in alternating format. That means
  // that the first disk at index 0 is light, the second disk at index 1
  // is dark, and so on for the entire row of disks.
  bool is_initialized() const {
    for (size_t i = 0; i < _colors.size(); ++i)       //for loop to iterate each disk
    {
      if (i % 2 == 0 && _colors[i] != DISK_LIGHT)     //check if disk at index 0 and even indexes are light, return false otherwise
      {
        return false;
      }
      else if (i % 2 == 1 && _colors[i] != DISK_DARK) //check if disk at index 1 and odd indexes are dark, return false otherwise
      {
        return false;
      }
    }
    return true;
  }

  // Return true when this disk_state is fully sorted, with all dark disks
  // on the left (low indices) and all light disks on the right (high
  // indices).
  bool is_sorted() const {
    int n = total_count();    //stores the total number of disks
    int begin_light = n/2;    //stores the index where light disks started

    for (int i = 0; i < begin_light; ++i)   //iterate every disk starting from index 0 to the one before light disks started
    {                                 
      if (_colors[i] != DISK_DARK)          //check if disk color is dark, return false otherwise
      {
        return false;
      }
    }

    for (int i = begin_light; i < n; ++i)   //iterate every disk starting from index where light disks started to the end
    {
      if(_colors[i] != DISK_LIGHT)          //check if disk color is light, return false otherwise
      {
        return false;
      }
    }
    return true;
  }
};

// Data structure for the output of the alternating disks problem. That
// includes both the final disk_state, as well as a count of the number
// of swaps performed.
class sorted_disks {
private:
  disk_state _after;
  unsigned _swap_count;
  
public:
  
  sorted_disks(const disk_state& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }
  
  sorted_disks(disk_state&& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }
  
  const disk_state& after() const {
    return _after;
  }
  
  unsigned swap_count() const {
    return _swap_count;
  }
};

// Algorithm that sorts disks using the alternate algorithm.
sorted_disks sort_alternate(const disk_state& before) {
  disk_state after = before;          //make a copy of the disk
  int n = (after.total_count() / 2);  //total number of runs to be executed
  int a = 0;                          //stores number of runs
  int count_swaps = 0;                //stores count of swaps performed

  while (a < n)                       //while total number of runs to be executed haven't been reached, continue
  {
    for (size_t i = 0; i < after.total_count(); i += 2)    //run 1, alternate algorithm iterates over each pair, starting from the leftmost disk to the rightmost disk
    {
      if (after.get(i) == DISK_LIGHT && after.get(i + 1) == DISK_DARK) //check if the left disk of the pair is light and right disk is dark
      {
        after.swap(i);                                                 //if true, swap them so that left disk is now dark and right disk is now light
        count_swaps++;
      }
    }
    a++;
    for (size_t i = 1; i < after.total_count() - 1; i += 2)  //run 2, alternate algorithm iterates over each pair, starting from the second leftmost disk to the second rightmost disk
    {
      if (after.get(i) == DISK_LIGHT && after.get(i + 1) == DISK_DARK)  //check if the left disk of the pair is light and right disk is dark
      {
        after.swap(i);                                                  //if true, swap them so that left disk is now dark and right disk is now light
        count_swaps++;
      }
    }
    a++;
  }

  return sorted_disks(after, count_swaps);  //return the sorted disk with dark disks on the left side and light disks on the right side, including total number of swaps performed
}


// Algorithm that sorts disks using the lawnmower algorithm.
sorted_disks sort_lawnmower(const disk_state& before) {
  disk_state after = before;            //make a copy of the disk
  int n = (after.total_count() / 2);    //total number of runs to be executed
  int a = 0;                            //stores number of runs
  int count_swaps = 0;                  //stores count of swaps performed

while (a < n)     //while total number of runs to be executed haven't been reached, continue
  {
    for (size_t i = 0; i < after.total_count() - 1; i++)    //run 1 forward, starts with the leftmost disk to the right
    {
      if (after.get(i) == DISK_LIGHT && after.get(i + 1) == DISK_DARK)  //check if disk at index i is light and the disk after index i is dark
      { 
        after.swap(i);                                                  //if true, swap them so that left disk is now dark and right disk is now light
        count_swaps++;
      }
    }
    a++;
    for (size_t i = after.total_count() - 2; i > 0; i--)  //run 2 reverse, starts with the disk before the rightmost disk to the left until the disk before the leftmost end
    {
      if (after.get(i - 1) == DISK_LIGHT && after.get(i) == DISK_DARK)  //check if disk at index i is dark and the disk before index i is light
      {
        after.swap(i - 1);                                             //if true, swap them so that left disk is now dark and right disk is now light                             
        count_swaps++;
      }
    }
    a++;
  }
  return sorted_disks(after, count_swaps);  //return the sorted disk with dark disks on the left side and light disks on the right side, including total number of swaps performed
}
  
