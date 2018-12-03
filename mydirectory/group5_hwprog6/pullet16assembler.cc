#include "pullet16assembler.h"

/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456
 * Class 'Assembler' for assembling code.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Tsung Wei Wu
 * Date: 20 November 2018
**/

/***************************************************************************
 * Constructor
**/
Assembler::Assembler() {
}
// trying GitHub
/***************************************************************************
 * Destructor
**/
Assembler::~Assembler() {
}

/***************************************************************************
 * Accessors and Mutators
**/

/***************************************************************************
 * General functions.
**/

/***************************************************************************
 * Function 'Assemble'.
 * This top level function assembles the code.
 *
 * Parameters:
 *   in_scanner - the scanner to read for source code
 *   out_stream - the output stream to write to
**/
void Assembler::Assemble(Scanner& in_scanner, string binary_filename,
                         ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter Assemble" << endl;
#endif

  //////////////////////////////////////////////////////////////////////////
  // Pass one
  // Produce the symbol table and detect errors in symbols.
  PassOne(in_scanner);
  PrintCodeLines();
  PrintSymbolTable();
  //////////////////////////////////////////////////////////////////////////
  // Pass two
  // Generate the machine code.
  PassTwo();
  PrintCodeLines();
  PrintSymbolTable();
  PrintMachineCode(binary_filename, out_stream);
  //////////////////////////////////////////////////////////////////////////
  // Dump the results.
  // Reading Binary File
  std::ifstream input(binary_filename, std::ifstream::binary);
  
  if (input) {
    input.seekg(0, input.end);
    int length = input.tellg();
    input.seekg(0, input.beg);

    // Converted Binary to ascii
    char buffer[2];
    for (int i = 0; i < length/2; ++i) {
      input.read(buffer, 2);
      int16_t valueread = static_cast<int16_t>((buffer[1]) | (buffer[0] << 8));

      string converted_binary = DABnamespace::DecToBitString(valueread, 16);

      // dumps it into output text file
      out_stream << converted_binary << endl;
    }
  }
  input.close();

#ifdef EBUG
  Utils::log_stream << "leave Assemble" << endl;
#endif
}

/***************************************************************************
 * Function 'GetInvalidMessage'.
 * This creates a "value is invalid" error message.
 *
 * Parameters:
 *   leadingtext - the text of what it is that is invalid
 *   symbol - the symbol that is invalid
**/
string Assembler::GetInvalidMessage(string leadingtext, string symbol) {
  string returnvalue = "";
  return returnvalue;
}

/***************************************************************************
 * Function 'GetInvalidMessage'.
 * This creates a "value is invalid" error message.
 *
 * Parameters:
 *   leadingtext - the text of what it is that is invalid
 *   hex - the hex operand that is invalid
**/
string Assembler::GetInvalidMessage(string leadingtext, Hex hex) {
  string returnvalue = "";

  return returnvalue;
}

/***************************************************************************
 * Function 'GetUndefinedMessage'.
 * This creates a "symbol is undefined" error message.
 *
 * Parameters:
 *   badtext - the undefined symbol text
**/
string Assembler::GetUndefinedMessage(string badtext) {
  string returnvalue = "";
  return returnvalue;
}

/***************************************************************************
 * Function 'PassOne'.
 * Produce the symbol table and detect multiply defined symbols.
 *
 * CAVEAT: We have deliberately forced symbols and mnemonics to have
 *         blank spaces at the end and thus to be all the same length.
 *         Symbols are three characters, possibly with one or two blank at end.
 *         Mnemonics are three characters, possibly with one blank at end.
 *
 * Parameters:
 *   in_scanner - the input stream from which to read
 *   out-stream - the output stream to which to write
**/
void Assembler::PassOne(Scanner& in_scanner) {
#ifdef EBUG
  Utils::log_stream << "enter PassOne" << endl;
#endif
  Utils::log_stream << "PASS ONE" << endl;
  // Unfinished code -Sean

  string lbl = "";
  pc_in_assembler_ = 0;
  int linecounter_ = 0;
  CodeLine codeline;
  // obtaining the next line read
  string line = in_scanner.NextLine();
  while (line != "") {
    codeline = CodeLine();  // creating object
    string mnemonic, label, addr, symoperand, hexoperand, comments, code;
    mnemonic = "nullmnemonic";
    label = "nulllabel";
    symoperand = "nullsymoperand";
    comments = "nullcomments";
    code = "nullcode";
    if (line.length() < 21) {
      line.append(21-line.length(), ' ');  // pad to length 21, for easier code
    }
    if (line.substr(0, 1) == "*") {
      // set beginning lines of comments
      codeline.SetCommentsOnly(linecounter_, line);
      // set machine code to null for the comments
      codeline.SetMachineCode(code);
    } else {
      if (line.substr(0, 3) != "   ") {  // reading label
        label = line.substr(0, 3);
        if (symboltable_.count(label) == 0) {
          // add symbol to table if not already present
          symboltable_.insert({label, Symbol(label, pc_in_assembler_)});
        } else {
          // sets a flag for duplicate labels
          symboltable_.at(label).SetMultiply();
        }
      } // end of if label
      mnemonic = line.substr(4, 3);  // getting mnemonic
      addr = line.substr(8, 1);  // getting addr
      if (line.substr(10, 3) != "   ") {
        // if field is empty, value of symoperand_ will be null
        symoperand = line.substr(10, 3);
      }  // end of if symbol
      hexoperand = line.substr(14, 5);
      if (line.substr(20, 1) == "*") {
        comments = line.substr(20);
      }  // end of if comments
      // sends all the data for it to be assigned
      codeline.SetCodeLine(linecounter_, pc_in_assembler_, label,
      mnemonic, addr, symoperand, hexoperand, comments, kDummyCodeA);
      int jumpvalue = codeline.GetHexObject().GetValue();
      if (mnemonic == "ORG") {
        if (jumpvalue > 4095 || jumpvalue < 0) {
          Utils::log_stream << "Invalid ORG at line: " << linecounter_ << endl;
        } else {
        pc_in_assembler_ = jumpvalue;
        }
      } else if (mnemonic == "DS ") {
        if ( (jumpvalue + pc_in_assembler_) > 4095 || jumpvalue < 1) {
          Utils::log_stream << "Invalid DS at line: " << linecounter_ << endl;
        } else {
        pc_in_assembler_ += jumpvalue;
        }
      } else if (mnemonic != "END") {
        pc_in_assembler_++;
      }  // end of if END

      if (mnemonic == "END") {
        found_end_statement_ = true;
      } // end of if statement END
    }  // end of else statement
    linecounter_++;
    codelines_.push_back(codeline);
    line = in_scanner.NextLine();
  }  // end of while loop

#ifdef EBUG
  Utils::log_stream << "leave PassOne" << endl;
#endif
}

/***************************************************************************
 * Function 'PassTwo'.
 * This function does pass two of the assembly process.
**/
void Assembler::PassTwo() {
#ifdef EBUG
  Utils::log_stream << "enter PassTwo" << endl;
#endif
  Utils::log_stream << "PASS TWO" << endl;
  string bitstring = "";
  Symbol symbol;
// Note: this code does not handle ORG, DS, END correctly yet
// This block goes through each of the inputted codelines and converts the
// mnemonic, direct/indirect addressing, and 12 bit address to appropriate
// machine code.
  for (auto it = codelines_.begin(); it != codelines_.end(); it++) {
    // The first block looks for the mnemonic in the map
    // of opcodes, and converts to machine code accordingly.
    if ((*it).IsAllComment()) {
      continue;
    }
    map <string, string>::iterator op_it = opcodes.find((*it).GetMnemonic());
    if (op_it != opcodes.end()) {
      bitstring = opcodes.find((*it).GetMnemonic())-> second;
      // The second block looks for the
      // asterisk to determine whether the 4th bit
      // should be denoted as direct or indirect addressing.
      if ((*it).GetAddr() == "*") {
        bitstring += "1";
      } else {
        bitstring += "0";
      }
      map <string, Symbol>::iterator sym_it = symboltable_.find(
                                                (*it).GetSymOperand());
      // The last twelve digits are defined by the symbol or the hex operand,
      // unless it's RD or STP or WRT
      if (sym_it != symboltable_.end()) {
        symbol = symboltable_.find((*it).GetSymOperand())-> second;
        bitstring += DABnamespace::DecToBitString(symbol.GetLocation(), 12);
      } else if (!(*it).GetHexObject().IsNull()) {
        bitstring += DABnamespace::DecToBitString(
            (*it).GetHexObject().GetValue(), 12);
      } else {
        //to do: The symbol is not found in the symbol table 
        Utils::log_stream << "Symbol not in table at " << (*it).GetPC() << endl;
        continue;
      }
      WriteMemory((*it).GetPC(), bitstring);
      (*it).SetMachineCode(bitstring);  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "RD ") {
      WriteMemory((*it).GetPC(), "1110000000000001");
      (*it).SetMachineCode("1110000000000001");  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "STP") {
      WriteMemory((*it).GetPC(), "1110000000000010");
      (*it).SetMachineCode("1110000000000010");  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "WRT") {
      WriteMemory((*it).GetPC(), "1110000000000011");
      (*it).SetMachineCode("1110000000000011");  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "HEX") {
      WriteMemory((*it).GetPC(), DABnamespace::DecToBitString(
          (*it).GetHexObject().GetValue(), 16));
      (*it).SetMachineCode(DABnamespace::DecToBitString(
          (*it).GetHexObject().GetValue(), 16));  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "DS ") {
      WriteMemory((*it).GetPC() + 
          (*it).GetHexObject().GetValue() - 1, kDummyCodeA);
      WriteMemory((*it).GetPC(), kDummyCodeC);
    } else if ((*it).GetMnemonic() == "ORG") {
      //nothing needs to be done
    }
  }
  #ifdef EBUG
    Utils::log_stream << "leave PassTwo" << endl;
  #endif
}

/***************************************************************************
 * Function 'PrintCodeLines'.
 * This function prints the code lines.
**/
void Assembler::PrintCodeLines() {
#ifdef EBUG
  Utils::log_stream << "enter PrintCodeLines" << endl;
#endif
  string s = "";

  for (auto iter = codelines_.begin(); iter != codelines_.end(); ++iter) {
    Utils::log_stream << (*iter).ToString() << '\n';
  }

  if (!found_end_statement_) {
    s += "\n***** ERROR -- NO 'END' STATEMENT\n";
    has_an_error_ = true;
  }

#ifdef EBUG
  Utils::log_stream << "leave PrintCodeLines" << endl;
#endif
  Utils::log_stream << s << endl;
}

/***************************************************************************
 * Function 'PrintMachineCode'.
 * This function prints the machine code.
**/
void Assembler::PrintMachineCode(string binary_filename,
                                 ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter PrintMachineCode" << " "
                    << binary_filename << endl;
#endif
string s = "";
// WILL CHANGE THIS TO BE READING THE BINARY MACHINE CODE LATER BUT FOR NOW
// JUST DUMPING THE ASCII THAT WE HAVE
// to do find a way to do that kDummyCodeA thing in the middle !!!
Utils::log_stream << "MACHINE CODE\n"
                  << "enter PrintMachineCode " << binary_filename
                  << endl;

std::ofstream output(binary_filename, std::ofstream::binary);
int count = 0;
for (auto iter = memory_.begin(); iter != memory_.end(); ++iter) {
  s += Utils::Format(count, 4) + " ";
  s += DABnamespace::DecToBitString(count, 12) + " ";
  s += (*iter).GetBitPattern().substr(0, 4) + " ";
  s += (*iter).GetBitPattern().substr(4, 4) + " ";
  s += (*iter).GetBitPattern().substr(8, 4) + " ";
  s += (*iter).GetBitPattern().substr(12, 4) + '\n';
  count++;
  if (output) {
    int16_t ascii_16 = DABnamespace::BitStringToDec((*iter).GetBitPattern());
    char data[4];
      data[0] = static_cast<char>(ascii_16 >> 8);
      data[1] = static_cast<char>((ascii_16));
      // writes the binary to file
      output.write(data, 2);
  } // end of if (output) 
} // end of for loop
output.close();
Utils::log_stream << s << endl;
#ifdef EBUG
  Utils::log_stream << "leave PrintMachineCode" << endl;
#endif
}

/******************************************************************************
 * Function 'PrintSymbolTable'.
 * This function prints the symbol table.
**/
void Assembler::PrintSymbolTable() {
#ifdef EBUG
  Utils::log_stream << "enter PrintSymbolTable" << endl;
#endif

string s = "";
Utils::log_stream << "SYMBOL TABLE\n    SYM LOC FLAGS\n";
for (std::pair<string, Symbol> it : symboltable_) {
  Utils::log_stream << "SYM " << it.second.ToString() << endl;
}

#ifdef EBUG
  Utils::log_stream << "leave PrintSymbolTable" << endl;
#endif
  Utils::log_stream << s << endl;
}

/******************************************************************************
 * Function 'SetNewPC'.
 * This function gets a new value for the program counter.
 *
 * No return value--this sets the class's PC variable.
 *
 * Parameters:
 *   codeline - the line of code from which to update
**/
void Assembler::SetNewPC(CodeLine codeline) {
#ifdef EBUG
  Utils::log_stream << "enter SetNewPC" << endl;
#endif

#ifdef EBUG
  Utils::log_stream << "leave SetNewPC" << endl;
#endif
}

/******************************************************************************
 * Function 'UpdateSymbolTable'.
 * This function updates the symbol table for a putative symbol.
 * Note that there is a hack here, in that the default value is 0
 * and that would mean we can't store a symbol at location zero.
 * So we add one, and then back that out after the full first pass is done.
**/
void Assembler::UpdateSymbolTable(int pc, string symboltext) {
#ifdef EBUG
  Utils::log_stream << "enter UpdateSymbolTable" << endl;
#endif

#ifdef EBUG
  Utils::log_stream << "leave UpdateSymbolTable" << endl;
#endif
}

void Assembler::WriteMemory(int pc, string code) {
  for (int i = memory_.size(); i < pc; i++) {
    memory_.push_back(OneMemoryWord(kDummyCodeA));
  }
  if ((int)memory_.size() == pc) {
    memory_.push_back(OneMemoryWord(code));
  } else {
    memory_.at(pc) = OneMemoryWord(code);
  }
}
