// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 27.12.2020                                              ** //
// **   Module: String - header                                          ** //
// ************************************************************************ //

#ifndef VDU_String_H
#define VDU_String_H

/**
 * Code for `ltrim`, `rtrim` and `trim` was taken over from:
 * https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring 
 * 
 * Author: Evan Teran
 */

#include <algorithm> 
#include <string>

using namespace std;

/**
 * trim from start (in place)
 */
static inline void ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
}

/**
 * trim from end (in place)
 */
static inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

/**
 * trim from both ends (in place)
 */
inline void trim(string &s) {
    ltrim(s);
    rtrim(s);
}

#endif