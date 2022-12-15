#include "getopt.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "html2md.h"

using std::string;
using std::ifstream;
using std::fstream;
using std::cerr;
using std::cin;
using std::ios;
using std::cout;
using std::endl;
using std::stringstream;

namespace file {
bool exists(const std::string& name) {
  ifstream f(name.c_str());
  return f.good();
}
}

constexpr const char * const description =
        " [Options]\n\n"
        "Simple and fast HTML to Markdown converter with table support.\n\n"
        "Options:\n"
        "  -h\tDisplays this help information.\n"
        "  -v\tDisplay version information and exit.\n"
        "  -o\tSets the output file.\n"
        "  -i\tSets the input file or text.\n"
        "  -p\tPrint Markdown(overrides -o).\n"
        "  -r\tOverwrite the output file (if it already exists) without asking.\n";


int main(int argc, char* argv[]) {
  string input;
  string inFile;
  string outFile = "Converted.md";

  bool print = false;
  bool replace = false;

  if (argc == 1) {
      cout << argv[0] << description;
      return 0;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "hvo:i:pr")) != -1) {
      switch (opt) {
      case 'h':
          cout << argv[0] << description;
          return 0;
      case 'v':
          cout << "Version " << VERSION << endl;
          return 0;
      case 'p':
          print = true;
          break;
      case 'r':
          replace = true;
          break;
      case 'o':
          outFile = optarg;
          break;
      case 'i':
          inFile = optarg;
          break;
      default:
          cout << "Invalid argument: " << (char)opt << endl;
          break;
      }
  }

  if (file::exists(inFile)) {
    ifstream in(inFile);
    stringstream buffer;
    buffer << in.rdbuf();
    input = std::move(buffer.str());

    if (in.bad()) {
      cerr << "Error: " << strerror(errno) << ": " << inFile << endl;
      return 0;
    }

    in.close();
  }
  else input = std::move(inFile);

  auto md = html2md::Convert(input);

  if (print) {
    cout << md << endl;
    return 0;
  }

  if (file::exists(outFile) && !replace) {
    string override;

    while (true) {
      cout << outFile << " already exists, override? [y/n] ";
      getline(cin, override);
      if (override.empty()) continue;

      stringstream ss(override);
      string input;
      ss >> input;
      if (input.empty() || input.length() > 1) {
          cout << "Invalid input" << endl;
          continue;
      }

      char c = (char)tolower(input[0]);
      if (c == 'n') return 0;
      else if (c == 'y') break;
      else {
          cout << "Invalid input" << endl;
          continue;
      }
    }

  }

  fstream out;
  out.open(outFile, ios::out);
  if (!out.is_open()) {
    cout << md << endl;
    return 0;
  }

  out << md;
  out.close();

  if (out.bad()) {
    cerr << "Error: " << strerror(errno) << ": " << outFile << endl;
    return 1;
  }

  cout << "Markdown written to " << outFile << endl;

  return 0;
}
