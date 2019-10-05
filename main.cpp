/**
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 *
 * Name        :  OpenQM
 * Author      :  Andrea Bontempi
 * Description :  Logic Network Synthesizer
 *
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 *
 * This file is part of OpenQM.
 *
 *  OpenQM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenQM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this project.  If not, see <http://www.gnu.org/licenses/>.
 *
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 */

#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/program_options.hpp>
#include <cstdio>
#include <sstream>

#include "implicant.h"
#include "qm.h"
#include "utils.h"

namespace po = boost::program_options;
using std::uintmax_t;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

string
num2conjstr(uintmax_t len, uintmax_t n)
{
    string s(len, '0');
    uintmax_t i = 0;
    while (n != 0) {
        if (n % 2 == 1)
            s[len - 1 - i] = '1';
        i++;
        n /= 2;
    }
    return s;
}

int
main(int argc, char **argv)
try {
    // Parse args and print help if needed
    string input_format;
    po::options_description desc("OpenQM - boolean function minimizer.\nFunction definition is read from stdin and result is printed to stdout. Options");
    desc.add_options()
        ("help,h", "print this help message")
        ("input-format,i", po::value<string>(&input_format)->default_value("vector"),
            "specify input format of function definition. can be 'vector', "\
            "'charset' or 'implicants' (see README.md)")
        ("num-var,n", po::value<uintmax_t>(), "specify number of function variables. required by "\
            "'vector' input format")
        ("no-print-result", "don't print the resulted formula")
        ("no-print-number", "don't print the number of implicants in the resulted formula")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        cout << desc << "\n";
        return 0;
    }

    // create a list of implicants according to input format
    std::vector<Implicant> implicants;

    if (input_format == "vector") {
        if (vm.count("num-var") == 0)
            throw runtime_error("the number of function variables is not specified");
        uintmax_t num_var = vm["num-var"].as<uintmax_t>();
        uintmax_t cnt_read = 0;
        uintmax_t vec_len = binpow(2, num_var);
        uintmax_t cnt_remain = vec_len;
        while (int ch = getchar()) {
            if (isspace(ch))
                continue;
            if (EOF == ch)
                break;
            if (0 == cnt_remain)
                 throw runtime_error("vector length does not correspond to number of variables");
            if ('1' == ch) {
                Implicant imp(num2conjstr(num_var, cnt_read));
                implicants.push_back(imp);
            } else if ('0' != ch) {
                throw runtime_error("found incorrect symbol in function vector");
            }
            --cnt_remain;
            ++cnt_read;
        }
        if (cnt_remain != 0)
            throw runtime_error("vector length does not correspond to number of variables");
    }

    else if (input_format == "charset") {
        string str;
        while (std::cin >> str)
            implicants.push_back(Implicant(str));
    }

    else if (input_format == "implicants") {
        string str;
        while (std::cin >> str)
            implicants.push_back(Implicant(str));
    }

    else {
        throw runtime_error("unknown input format");
    }

    // TODO: is it necessary?
    if (implicants.empty()) {
        return EXIT_SUCCESS;
    }

    // solve
    std::vector<Implicant> solution = make_qm(implicants);

    // print
    if (vm.count("no-print-number") == 0)
        cout << solution.size() << endl;
    if (vm.count("no-print-result") == 0)
        for (const auto & imp : solution) {
            cout << imp << endl;
        }
    
    return EXIT_SUCCESS;
} catch (std::exception &e) {
    cout << "error: " << e.what() << endl;
    return EXIT_FAILURE;
}
