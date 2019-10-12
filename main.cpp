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
#include <sstream>
#include <thread>
#include <mutex>
#include <sstream>
#include <algorithm>
#include <functional>

#include "qm.h"
#include "string_implicant.h"
#include "utils.h"

namespace po = boost::program_options;
using std::uintmax_t;
using std::cout;
using std::cin;
using std::min;
using std::endl;
using std::string;
using std::runtime_error;
using std::vector;
using std::thread;
using std::set;
using std::pair;
using std::mutex;
using std::scoped_lock;

po::variables_map
parse_args(int argc, char **argv)
{
    po::options_description desc("OpenQM - boolean function minimizer.\nFunction definition is read from stdin and result is printed to stdout. Options");
    desc.add_options()
        ("help,h", "print this help message")
        ("solve-problem", po::value<uintmax_t>(), "solve the problem!")
        ("threads,t", po::value<uintmax_t>()->default_value(4), "number of threads to solve the problem")
        ("input-format,i", po::value<string>()->default_value("vector"),
            "specify input format of function definition. can be 'vector' "\
            "or 'implicants' (see README.md)")
        ("num-var,n", po::value<uintmax_t>(), "specify number of function variables. required by "\
            "'vector' input format")
        ("kernel,k", "find kernel implicants")
        ("deadends,d", "find deadend DNFs")
        ("shortest,s", "find shortest deadends")
        ("minimal,m", "find minimal deadends")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        cout << desc << "\n";
        exit(0);
    }
    return vm;
}

void
read_vector(std::istream &is, vector<StringImplicant> &implicants, uintmax_t num_var)
{
    uintmax_t cnt_read = 0;
    uintmax_t vec_len = binpow(2, num_var);
    uintmax_t cnt_remain = vec_len;
    while (int ch = is.get()) {
        if (isspace(ch))
            continue;
        if (EOF == ch)
            break;
        if (0 == cnt_remain)
             throw runtime_error("vector length does not correspond to number of variables");
        if ('1' == ch) {
            StringImplicant imp(binary_repr(cnt_read, num_var));
            implicants.emplace_back(imp);
        } else if ('0' != ch) {
            throw runtime_error("found incorrect symbol in function vector");
        }
        --cnt_remain;
        ++cnt_read;
    }
    if (cnt_remain != 0)
        throw runtime_error("vector length does not correspond to number of variables");
}

void
read_implicants(std::istream &is, vector<StringImplicant> &implicants)
{
    string str;
    while (is >> str)
        implicants.emplace_back(StringImplicant(str));
}

class BinaryString
{
    string bin_str;
public:
    BinaryString(uintmax_t x, size_t num_bin_digits) :
        bin_str(binary_repr(x, num_bin_digits)) {}

    size_t size() const
    {
        return bin_str.size();
    }

    bool increment()
    {
        size_t s = size();
        bool carry = true;
        size_t ind = 0;
        do {
            if (bin_str[s - ind - 1] == '0') {
                bin_str[s - ind - 1] = '1';
                carry = false;
            } else {
                bin_str[s - ind - 1] = '0';
            }
            ++ind;
        } while (carry && (ind < s));
        return carry; // true if overflow happened
    }

    const string &str() const
    {
        return bin_str;
    }
};

class ResultStore
{
    mutex mut;
    vector<BinaryString> result;
public:
    void
    show_msg(uintmax_t thread_id, const string &msg)
    {
        scoped_lock lock(mut);
        cout << "[" << thread_id << "] " << msg << endl;
    }

    void
    add_result(const BinaryString &s)
    {
        scoped_lock lock(mut);
        result.push_back(s);
        cout << "found! " << s.str() << endl;
    }
};

template<typename T>
bool
intersect(const set<T> s1, const set<T> s2)
{
    if (s1.empty() || s2.empty())
        return true;
    for (const T &el : s1)
        if (s2.count(el) == 1)
            return true;
    return false;
}

template<typename ImplicantType>
set<int>
get_shortest(const vector<ImplicantType> &reduced_dnf, const vector<set<int>> &deadends)
{
    set<int> res;
    if (deadends.empty())
        return res;

    vector<uintmax_t> lengths;
    lengths.reserve(deadends.size());
    uintmax_t min_length = deadends[0].size();
    for (size_t i = 0; i < deadends.size(); i++) {
        lengths[i] = deadends[i].size();
        min_length = min(min_length, lengths[i]);
    }
    // TODO: what's wrong with the following code for finding length and minimum length?
    //    transform(deadends.begin(), deadends.end(), lengths.begin(), std::mem_fn(&set<int>::size));
    //    uintmax_t min_length = *std::min_element(lengths.begin(), lengths.end());
    for (size_t i = 0; i < deadends.size(); i++)
        if (lengths[i] == min_length)
            res.insert(i);
    return res;
}

template<typename ImplicantType>
int
deadend_rank(const vector<ImplicantType> &reduced_dnf, const set<int> &deadend)
{
    uintmax_t rank = 0;
    for (int implicant_ind : deadend)
        rank += reduced_dnf[implicant_ind].rank();
    return rank;
}

template<typename ImplicantType>
set<int>
get_minimal(const vector<ImplicantType> &reduced_dnf, const vector<set<int>> &deadends)
{
    set<int> res;
    if (deadends.empty())
        return res;

    vector<uintmax_t> ranks;
    ranks.reserve(deadends.size());

    // TODO : compute rank in separate function
    uintmax_t min_rank = deadend_rank(reduced_dnf, deadends[0]);

    for (size_t i = 0; i < deadends.size(); i++) {
        ranks[i] = deadend_rank(reduced_dnf, deadends[i]);
        min_rank = min(min_rank, ranks[i]);
    }

    for (size_t i = 0; i < deadends.size(); i++)
        if (ranks[i] == min_rank)
            res.insert(i);
    return res;
}

void
bruteforce(uintmax_t thread_id, BinaryString start, uintmax_t num_var, uintmax_t num_threads, ResultStore &store)
{
    store.show_msg(thread_id, "start");
    BinaryString &cur = start;
    bool overflow = false;
    do {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        store.show_msg(thread_id, cur.str());
        vector<StringImplicant> implicants;
        std::stringstream ss(cur.str());
        read_vector(ss, implicants, num_var);

        vector<StringImplicant> reduced_dnf = make_qm(implicants);
        vector<set<int>> deadends = get_deadends(implicants, reduced_dnf);
        set<int> minimals = get_minimal(reduced_dnf, deadends);
        set<int> shortests = get_shortest(reduced_dnf, deadends);

        // TODO: maybe std::set_intersection?
        if (!intersect(minimals, shortests)) {
            store.add_result(cur);
        }

        for (uintmax_t i = 0; i < num_threads; i++) {
            overflow = start.increment();
            if (overflow)
                break;
        }
    } while (!overflow);
}

void
solve_problem(uintmax_t num_var, uintmax_t num_threads)
{
    ResultStore store;
    vector<thread> threads;
    BinaryString start(0, binpow(2, num_var));
    for (uintmax_t i = 0; i < num_threads; i++) {
        threads.push_back(thread(bruteforce, i, start, num_var, num_threads, std::ref(store)));
        start.increment();
    }
    for (auto &thread : threads)
        thread.join();
}

int
main(int argc, char **argv)
try {

    po::variables_map vm = parse_args(argc, argv);

    // solve the problem
    if (vm.count("solve-problem") != 0) {
        uintmax_t num_var = vm["solve-problem"].as<uintmax_t>();
        uintmax_t num_threads = vm["threads"].as<uintmax_t>();
        solve_problem(num_var, num_threads);
        return EXIT_SUCCESS;
    }

    // create a list of implicants according to input format
    vector<StringImplicant> implicants;
    string input_format = vm["input-format"].as<string>();
    if (input_format == "vector") {
        if (vm.count("num-var") == 0)
            throw runtime_error("the number of function variables is not specified");
        uintmax_t num_var = vm["num-var"].as<uintmax_t>();
        read_vector(cin, implicants, num_var);
    } else if (input_format == "implicants") {
        read_implicants(cin, implicants);
    } else {
        throw runtime_error("unknown input format");
    }

    // find reduced DNF
    vector<StringImplicant> reduced_dnf = make_qm(implicants);
    cout << "--- reduced DNF ---" << endl;
    cout << reduced_dnf.size() << endl;
    for (const auto &implicant : reduced_dnf)
        cout << implicant << endl;
    cout << endl;

    // find kernel if needed
    if (vm.count("kernel") != 0) {
        vector<int> kernel_table = get_kernels(implicants, reduced_dnf);
        cout << "--- kernel ---" << endl;
        for (int is_kernel_implicant : kernel_table)
            cout << is_kernel_implicant << endl;
        cout << endl;
    }

    // find deadends if needed
    bool deadends_found = false;
    vector<set<int>> deadends;
    if (vm.count("deadends") != 0) {
        deadends = get_deadends(implicants, reduced_dnf);
        deadends_found = true;
        cout << "--- deadend DNFs ---" << endl;
        cout << deadends.size() << endl;
        for (const set<int> &deadend : deadends) {
            for (int implicant_id : deadend)
                cout << implicant_id << " ";
            cout << endl;
        }
        cout << endl;
    }

    // find shortest if needed
    if (vm.count("shortest") != 0) {
        if (!deadends_found) {
            deadends = get_deadends(implicants, reduced_dnf);
            deadends_found = true;
        }
        set<int> shortest = get_shortest(reduced_dnf, deadends);
        size_t min_length = deadends[*shortest.begin()].size();
        cout << "--- shortest deadends (min length = " << min_length << ") ---" << endl;
        cout << shortest.size() << endl;
        for (auto i : shortest)
            cout << i << " ";
        cout << endl << endl;
    }

    // find minimal if needed
    if (vm.count("minimal") != 0) {
        if (!deadends_found) {
            deadends = get_deadends(implicants, reduced_dnf);
            deadends_found = true;
        }
        set<int> minimal = get_minimal(reduced_dnf, deadends);
        size_t min_rank = deadend_rank(reduced_dnf, deadends[*minimal.begin()]);
        cout << "--- minimal deadends (min rank = " << min_rank << ") ---" << endl;
        cout << minimal.size() << endl;
        for (auto i : minimal)
            cout << i << " ";
        cout << endl << endl;
    }


    return EXIT_SUCCESS;
} catch (std::exception &e) {
    cout << "error: " << e.what() << endl;
    return EXIT_FAILURE;
}
