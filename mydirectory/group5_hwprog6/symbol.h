/****************************************************************
 * Header file for the 'Symbol' class to contain one symbol.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Group 5:
   Tsung Wei Wu, Sean Wiig, Samyu Comandur, Mark Mcmurtury, Mayank Patel.
 * Date: 4 December 2018
**/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

// #include "../../Utilities/scanner.h"
// #include "../../Utilities/scanline.h"
#include "../../Utilities/utils.h"

class Symbol {
 public:
  Symbol();
  Symbol(string symboltext, int programcounter);
  virtual ~Symbol();

  int GetLocation() const;
  bool HasAnError() const;
  bool IsInvalid() const;
  bool IsMultiply() const;
  void SetMultiply();
  string ToString() const;

 private:
  int location_;
  bool is_multiply_;
  bool is_invalid_;
  string text_;

  bool CheckInvalid() const;
};
#endif
