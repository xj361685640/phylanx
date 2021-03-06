//  Copyright (c) 2017 Alireza Kheirkhahan
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/config.hpp>
#include <phylanx/execution_tree/primitives/file_read_csv.hpp>
#include <phylanx/ir/node_data.hpp>

#include <hpx/include/components.hpp>
#include <hpx/include/lcos.hpp>

#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_real.hpp>

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
typedef hpx::components::component<
    phylanx::execution_tree::primitives::file_read_csv>
    file_read_csv_type;
HPX_REGISTER_DERIVED_COMPONENT_FACTORY(file_read_csv_type,
    phylanx_file_read_csv_component, "phylanx_primitive_component",
    hpx::components::factory_enabled)
HPX_DEFINE_GET_COMPONENT_TYPE(file_read_csv_type::wrapped_type)

///////////////////////////////////////////////////////////////////////////////
namespace phylanx { namespace execution_tree { namespace primitives
{
    ///////////////////////////////////////////////////////////////////////////
    std::vector<match_pattern_type> const file_read_csv::match_data =
    {
        hpx::util::make_tuple(
            "file_read_csv", "file_read_csv(_1)", &create<file_read_csv>)
    };

    ///////////////////////////////////////////////////////////////////////////
    file_read_csv::file_read_csv(
        std::vector<primitive_argument_type> && operands)
    {
        if (operands.size() != 1)
        {
            HPX_THROW_EXCEPTION(hpx::bad_parameter,
                "phylanx::execution_tree::primitives::file_read_csv::"
                    "file_read_csv",
                "the file_read_csv primitive requires exactly one literal "
                    "argument");
        }

        if (!valid(operands[0]))
        {
            HPX_THROW_EXCEPTION(hpx::bad_parameter,
                "phylanx::execution_tree::primitives::file_read_csv::"
                    "file_read_csv",
                "the file_read_csv primitive requires that the given operand "
                    "is valid");
        }

        std::string* name = util::get_if<std::string>(&operands[0]);
        if (name == nullptr)
        {
            HPX_THROW_EXCEPTION(hpx::bad_parameter,
                "phylanx::execution_tree::primitives::file_read_csv::"
                    "file_read_csv",
                "the first literal argument must be a string representing a "
                    "valid file name");
        }

        filename_ = std::move(*name);
    }

    // read data from given file and return content
    hpx::future<primitive_result_type> file_read_csv::eval(
        std::vector<primitive_argument_type> const& args) const
    {
        std::ifstream infile(filename_.c_str(), std::ios::in);

        if (!infile.is_open())
        {
            HPX_THROW_EXCEPTION(hpx::bad_parameter,
                "phylanx::execution_tree::primitives::file_read_csv::eval",
                "couldn't open file: " + filename_);
        }

        std::string line;
        std::vector<double> matrix_array;
        std::size_t n_rows = 0, n_cols = 0;
        std::size_t before_readln = 0, after_readln = 0;

        while (std::getline(infile, line))
        {
            before_readln = matrix_array.size();

            if (boost::spirit::qi::parse(line.begin(), line.end(),
                    boost::spirit::qi::double_ % ',', matrix_array))
            {
                after_readln = matrix_array.size();
                if (n_rows == 0)
                {
                    n_cols = matrix_array.size();
                }
                else if (n_cols != (after_readln - before_readln))
                {
                    HPX_THROW_EXCEPTION(hpx::invalid_data,
                        "phylanx::execution_tree::primitives::file_read_csv::"
                            "eval",
                        "wrong data format, different number of element in "
                            "this row " +
                            filename_ + ':' + std::to_string(n_rows));
                }
                n_rows++;
            }
            else
            {
                HPX_THROW_EXCEPTION(hpx::invalid_data,
                    "phylanx::execution_tree::primitives::file_read_csv::eval",
                    "wrong data format " + filename_ + ':' +
                        std::to_string(n_rows));
            }
        }

        if (n_rows == 1)
        {
            if (n_cols == 1)
            {
                // scalar value
                return hpx::make_ready_future(primitive_result_type{
                    ir::node_data<double>{matrix_array[0]}});
            }

            // vector
            blaze::DynamicVector<double> vector(
                n_cols, matrix_array.data());

            return hpx::make_ready_future(primitive_result_type{
                ir::node_data<double>{std::move(vector)}});
        }

        // matrix
        blaze::DynamicMatrix<double> matrix(
            n_rows, n_cols, matrix_array.data());

        return hpx::make_ready_future(
            primitive_result_type{ir::node_data<double>{std::move(matrix)}});
    }
}}}
