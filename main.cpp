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

using namespace std;
namespace po = boost::program_options;

std::string
num2conjstr(std::uintmax_t len, std::uintmax_t n)
{
    std::string s(len, '0');
    std::uintmax_t i = 0;
    while (n != 0) {
        if (n % 2 == 1)
            s[len - 1 - i] = '1';
        i++;
        n /= 2;
    }
    return s;
}

std::uintmax_t
binpow(std::uintmax_t a, std::uintmax_t n)
{
    std::uintmax_t res = 1;
    while (n != 0) {
        if (n % 2 == 1)
            res *= a;
        a *= a;
        n /= 2;
    }
    return res;
}

int
main(int argc, char **argv)
try {
    // Parse args and print help if needed
    std::string mode;
    po::options_description desc("Data is being read from stdin. Allowed options:");
    desc.add_options()
        ("help,h", "print this help message")
        ("input-format,f", po::value<std::string>(&mode)->default_value("vector"),
            "specify input function definition format. Could be 'vector',"\
            "'charset' or 'implicants'")
        ("num-var,n", po::value<std::uintmax_t>(), "specify number of function variables. required by "\
            "'vector' input format")
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
    if (mode == "vector") {
        if (vm.count("num-var") == 0)
            throw std::runtime_error("the number of function variables is not specified");
        std::uintmax_t num_var = vm["num-var"].as<std::uintmax_t>();
        std::uintmax_t cnt_read = 0;
        std::uintmax_t vec_len = binpow(2, num_var);
        std::uintmax_t cnt_remain = vec_len;
        while (int ch = getchar()) {
            if (isspace(ch))
                continue;
            if (EOF == ch)
                break;
            if (0 == cnt_remain)
                 throw std::runtime_error("vector length does not correspond to number of variables");
            if ('1' == ch) {
                Implicant imp(num2conjstr(num_var, cnt_read));
                implicants.push_back(imp);
            } else if ('0' != ch) {
                throw std::runtime_error("found incorrect symbol in function vector");
            }
            --cnt_remain;
            ++cnt_read;
        }
        if (cnt_remain != 0)
            throw std::runtime_error("vector length does not correspond to number of variables");
    } else if (mode == "charset") {
        std::string str;
        while (std::cin >> str)
            implicants.push_back(Implicant(str));
    } else if (mode == "implicants") {
        std::string str;
        while (std::cin >> str)
            implicants.push_back(Implicant(str));
    } else {
        throw std::runtime_error("unknown input format");
    }

    // TODO: is it necessary?
    if (implicants.empty()) {
        return EXIT_SUCCESS;
    }

    // solve
    std::vector<Implicant> solution = makeQM(implicants, {});
    for (const auto & imp : solution) {
        std::cout << imp << std::endl;
    }
    
    return EXIT_SUCCESS;
} catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
