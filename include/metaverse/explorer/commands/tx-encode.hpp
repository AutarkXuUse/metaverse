/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 * Copyright (c) 2016-2017 metaverse core developers (see MVS-AUTHORS)
 *
 * This file is part of metaverse-explorer.
 *
 * metaverse-explorer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef BX_TX_ENCODE_HPP
#define BX_TX_ENCODE_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <metaverse/bitcoin.hpp>
#include <metaverse/explorer/command.hpp>
#include <metaverse/explorer/define.hpp>
#include <metaverse/explorer/generated.hpp>
#include <metaverse/explorer/config/address.hpp>
#include <metaverse/explorer/config/algorithm.hpp>
#include <metaverse/explorer/config/btc.hpp>
#include <metaverse/explorer/config/byte.hpp>
#include <metaverse/explorer/config/cert_key.hpp>
#include <metaverse/explorer/config/ec_private.hpp>
#include <metaverse/explorer/config/encoding.hpp>
#include <metaverse/explorer/config/endorsement.hpp>
#include <metaverse/explorer/config/hashtype.hpp>
#include <metaverse/explorer/config/hd_key.hpp>
#include <metaverse/explorer/config/header.hpp>
#include <metaverse/explorer/config/input.hpp>
#include <metaverse/explorer/config/language.hpp>
#include <metaverse/explorer/config/output.hpp>
#include <metaverse/explorer/config/raw.hpp>
#include <metaverse/explorer/config/script.hpp>
#include <metaverse/explorer/config/signature.hpp>
#include <metaverse/explorer/config/transaction.hpp>
#include <metaverse/explorer/config/wrapper.hpp>
#include <metaverse/explorer/utility.hpp>

/********* GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY **********/

namespace libbitcoin {
namespace explorer {
namespace commands {

/**
 * Various localizable strings.
 */
#define BX_TX_ENCODE_INVALID_OUTPUT \
    "An output is not valid."
#define BX_TX_ENCODE_LOCKTIME_CONFLICT \
    "The specified lock time is ineffective because all sequences are set to the maximum value."

/**
 * Class to implement the tx-encode command.
 */
class BCX_API tx_encode 
  : public command
{
public:

    /**
     * The symbolic (not localizable) command name, lower case.
     */
    static const char* symbol()
    {
        return "tx-encode";
    }


    /**
     * The member symbolic (not localizable) command name, lower case.
     */
    virtual const char* name()
    {
        return tx_encode::symbol();
    }

    /**
     * The localizable command category name, upper case.
     */
    virtual const char* category()
    {
        return "TRANSACTION";
    }

    /**
     * The localizable command description.
     */
    virtual const char* description()
    {
        return "Encode an unsigned transaction as Base16.";
    }

    /**
     * Load program argument definitions.
     * A value of -1 indicates that the number of instances is unlimited.
     * @return  The loaded program argument definitions.
     */
    virtual arguments_metadata& load_arguments()
    {
        return get_argument_metadata();
    }

	/**
     * Load parameter fallbacks from file or input as appropriate.
     * @param[in]  input  The input stream for loading the parameters.
     * @param[in]         The loaded variables.
     */
    virtual void load_fallbacks(std::istream& input, 
        po::variables_map& variables)
    {
    }

    /**
     * Load program option definitions.
     * BUGBUG: see boost bug/fix: svn.boost.org/trac/boost/ticket/8009
     * @return  The loaded program option definitions.
     */
    virtual options_metadata& load_options()
    {
        using namespace po;
        options_description& options = get_option_metadata();
        options.add_options()
        (
            BX_HELP_VARIABLE ",h",
            value<bool>()->zero_tokens(),
            "Get a description and instructions for this command."
        )
        (
            BX_CONFIG_VARIABLE ",c",
            value<boost::filesystem::path>(),
            "The path to the configuration settings file."
        )
        (
            "script_version,s",
            value<explorer::config::byte>(&option_.script_version)->default_value(5),
            "The pay-to-script-hash payment address version, defaults to 5. This is used to differentiate output addresses."
        )
        (
            "lock_time,l",
            value<uint32_t>(&option_.lock_time),
            "The transaction lock time."
        )
        (
            "version,v",
            value<uint32_t>(&option_.version)->default_value(1),
            "The transaction version, defaults to 1."
        )
        (
            "input,i",
            value<std::vector<explorer::config::input>>(&option_.inputs),
            "The set of transaction input points encoded as TXHASH:INDEX:SEQUENCE. TXHASH is a Base16 transaction hash. INDEX is the 32 bit input index in the context of the transaction. SEQUENCE is the optional 32 bit input sequence and defaults to the maximum value."
        )
        (
            "output,o",
            value<std::vector<explorer::config::output>>(&option_.outputs),
            "The set of transaction output data encoded as TARGET:SATOSHI:SEED. TARGET is an address (including stealth or pay-to-script-hash) or a Base16 script. SATOSHI is the 32 bit spend amount in satoshi. SEED is required for stealth outputs and not used otherwise. The same seed should NOT be used for multiple outputs."
        )
		(
			"period,p",
			value<uint32_t>(&option_.period)->default_value(7),
			"The deposit period support [7, 30, 90, 182, 365] days"
		)
		(
			"deposit,d",
			value<explorer::config::output>(&option_.deposit),
			"The transaction output deposit data encoded as TARGET:SATOSHI. TARGET is an address (including stealth or pay-to-script-hash) or a Base16 script. SATOSHI is the 32 bit spend amount in satoshi."
		);

        return options;
    }

    /**
     * Set variable defaults from configuration variable values.
     * @param[in]  variables  The loaded variables.
     */
    virtual void set_defaults_from_config(po::variables_map& variables)
    {
        const auto& option_script_version = variables["script_version"];
        const auto& option_script_version_config = variables["wallet.pay_to_script_hash_version"];
        if (option_script_version.defaulted() && !option_script_version_config.defaulted())
        {
            option_.script_version = option_script_version_config.as<explorer::config::byte>();
        }

        const auto& option_version = variables["version"];
        const auto& option_version_config = variables["wallet.transaction_version"];
        if (option_version.defaulted() && !option_version_config.defaulted())
        {
            option_.version = option_version_config.as<uint32_t>();
        }
    }

    /**
     * Invoke the command.
     * @param[out]  output  The input stream for the command execution.
     * @param[out]  error   The input stream for the command execution.
     * @return              The appropriate console return code { -1, 0, 1 }.
     */
    virtual console_result invoke(std::ostream& output,
        std::ostream& cerr);

    /* Properties */

    /**
     * Get the value of the script_version option.
     */
    virtual explorer::config::byte& get_script_version_option()
    {
        return option_.script_version;
    }

    /**
     * Set the value of the script_version option.
     */
    virtual void set_script_version_option(
        const explorer::config::byte& value)
    {
        option_.script_version = value;
    }

    /**
     * Get the value of the lock_time option.
     */
    virtual uint32_t& get_lock_time_option()
    {
        return option_.lock_time;
    }

    /**
     * Set the value of the lock_time option.
     */
    virtual void set_lock_time_option(
        const uint32_t& value)
    {
        option_.lock_time = value;
    }

    /**
     * Get the value of the version option.
     */
    virtual uint32_t& get_version_option()
    {
        return option_.version;
    }

    /**
     * Set the value of the version option.
     */
    virtual void set_version_option(
        const uint32_t& value)
    {
        option_.version = value;
    }

    /**
     * Get the value of the period option.
     */
    virtual uint32_t& get_period_option()
    {
        return option_.period;
    }

    /**
     * Set the value of the period option.
     */
    virtual void set_period_option(
        const uint32_t& value)
    {
        option_.period = value;
    }
	
	uint32_t get_reward_lock_block_height();
		
    /**
     * Get the value of the input options.
     */
    virtual std::vector<explorer::config::input>& get_inputs_option()
    {
        return option_.inputs;
    }

    /**
     * Set the value of the input options.
     */
    virtual void set_inputs_option(
        const std::vector<explorer::config::input>& value)
    {
        option_.inputs = value;
    }

    /**
     * Get the value of the output options.
     */
    virtual std::vector<explorer::config::output>& get_outputs_option()
    {
        return option_.outputs;
    }

    /**
     * Set the value of the output options.
     */
    virtual void set_outputs_option(
        const std::vector<explorer::config::output>& value)
    {
        option_.outputs = value;
    }
	
    /**
     * Get the value of the deposit options.
     */
    virtual explorer::config::output& get_deposit_option()
    {
        return option_.deposit;
    }

    /**
     * Set the value of the deposit options.
     */
    virtual void set_deposit_option(const explorer::config::output& value)
    {
        option_.deposit = value;
    }

private:

    /**
     * Command line argument bound variables.
     * Uses cross-compiler safe constructor-based zeroize.
     * Zeroize for unit test consistency with program_options initialization.
     */
    struct argument
    {
        argument()
        {
        }

    } argument_;

    /**
     * Command line option bound variables.
     * Uses cross-compiler safe constructor-based zeroize.
     * Zeroize for unit test consistency with program_options initialization.
     */
    struct option
    {
        option()
          : script_version(),
            lock_time(),
            version(),
            period(),
            inputs(),
            outputs(),
            deposit()
        {
        }

        explorer::config::byte script_version;
        uint32_t lock_time;
        uint32_t version;
        uint32_t period;
        std::vector<explorer::config::input> inputs;
        std::vector<explorer::config::output> outputs;
        explorer::config::output deposit;
    } option_;
};

} // namespace commands
} // namespace explorer
} // namespace libbitcoin

#endif