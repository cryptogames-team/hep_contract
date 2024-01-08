#include "eosio.nft.hpp"
using namespace eosio;

ACTION nft::create( name issuer, std::string sym ) {

	require_auth( _self );

	// Check if issuer account exists


        // Valid symbol
        asset supply(0, symbol( symbol_code( sym.c_str() ), 0) );

        auto symbol = supply.symbol;


        // Check if currency with symbol already exists
	auto symbol_name = symbol.code().raw();
        currency_index currency_table( _self, symbol_name );
        auto existing_currency = currency_table.find( symbol_name );


        // Create new currency
        currency_table.emplace( _self, [&]( auto& currency ) {
           currency.supply = supply;
           currency.issuer = issuer;
        });
}

ACTION nft::issue( name to,
                     asset quantity,
                     vector<string> uris,
		     string tkn_name,
                     string memo) {



        // e,g, Get EOS from 3 EOS
        auto symbol = quantity.symbol;


        // Ensure currency has been created
        auto symbol_name = symbol.code().raw();
        currency_index currency_table( _self, symbol_name );
        auto existing_currency = currency_table.find( symbol_name );

        const auto& st = *existing_currency;

        // Ensure have issuer authorization and valid quantity
        require_auth( st.issuer );


        // Increase supply
	add_supply( quantity );

        // Check that number of tokens matches uri size


        // mint nfts
        for(auto const& uri: uris) {
            mint( to, st.issuer, asset{1, symbol}, uri, tkn_name);
        }

        // Add balance to account
        add_balance( to, quantity, st.issuer );
}


ACTION nft::transferid( name	from,
                        name 	to,
                        id_type	id,
                        string	memo ) {
        // Ensure authorized to send from account

        require_auth( from );

        // Ensure 'to' account exists


	// Check memo size and print


        // Ensure token ID exists
        auto send_token = tokens.find( id );


	const auto& st = *send_token;

	// Notify both recipients
        require_recipient( from );
        require_recipient( to );

        // Transfer NFT from sender to receiver
        tokens.modify( send_token, from, [&]( auto& token ) {
	        token.owner = to;
        });

        // Change balance of both accounts
        sub_balance( from, st.value );
        add_balance( to, st.value, from );
}

ACTION nft::transfer( name 	from,
                      name 	to,
                      asset	quantity,
                      string	memo ) {
        // Ensure authorized to send from account

        require_auth( from );



	auto symbl = tokens.get_index<"bysymbol"_n>();

	auto it = symbl.lower_bound(quantity.symbol.code().raw());

	bool found = false;
	id_type id = 0;
	for(; it!=symbl.end(); ++it){

		if( it->value.symbol == quantity.symbol && it->owner == from) {
			id = it->id;
			found = true;
			break;
		}
	}



	// Notify both recipients
        require_recipient( from );
	require_recipient( to );

	SEND_INLINE_ACTION( *this, transferid, {from, "active"_n}, {from, to, id, memo} );
}

void nft::mint( name 	owner,
                name 	ram_payer,
                asset 	value,
                string 	uri,
		string 	tkn_name) {
        // Add token with creator paying for RAM
        tokens.emplace( ram_payer, [&]( auto& token ) {
            token.id = tokens.available_primary_key();
            token.uri = uri;
            token.owner = owner;
            token.value = value;
	    token.tokenName = tkn_name;
        });
}

ACTION nft::setrampayer(name payer, id_type id) {

	require_auth(payer);

	// Ensure token ID exists
	auto payer_token = tokens.find( id );


	const auto& st = *payer_token;

	// Notify payer
	require_recipient( payer );

	/*tokens.erase(payer_token);
 	tokens.emplace(payer, [&](auto& token){
  		token.id = st.id;
  		token.uri = st.uri;
  		token.owner = st.owner;
  		token.value = st.value;
  		token.name = st.name;
 	});*/

	// Set owner as a RAM payer
	tokens.modify(payer_token, payer, [&](auto& token){
		token.id = st.id;
		token.uri = st.uri;
		token.owner = st.owner;
		token.value = st.value;
		token.tokenName = st.tokenName;
	});

	sub_balance( payer, st.value );
	add_balance( payer, st.value, payer );
}


ACTION nft::burn( name owner, id_type token_id ) {

        require_auth( owner );

        // Find token to burn
        auto burn_token = tokens.find( token_id );


	asset burnt_supply = burn_token->value;

	// Remove token from tokens table
        tokens.erase( burn_token );

        // Lower balance from owner
        sub_balance( owner, burnt_supply );

        // Lower supply from currency
        sub_supply( burnt_supply );
}


void nft::sub_balance( name owner, asset value ) {

	account_index from_acnts( _self, owner.value );
        const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );



        if( from.balance.amount == value.amount ) {
            from_acnts.erase( from );
        } else {
            from_acnts.modify( from, owner, [&]( auto& a ) {
                a.balance -= value;
            });
        }
}

void nft::add_balance( name owner, asset value, name ram_payer ) {

	account_index to_accounts( _self, owner.value );
        auto to = to_accounts.find( value.symbol.code().raw() );
        if( to == to_accounts.end() ) {
            to_accounts.emplace( ram_payer, [&]( auto& a ){
                a.balance = value;
            });
        } else {
            to_accounts.modify( to, _self, [&]( auto& a ) {
                a.balance += value;
            });
        }
}

void nft::sub_supply( asset quantity ) {

	auto symbol_name = quantity.symbol.code().raw();
        currency_index currency_table( _self, symbol_name );
        auto current_currency = currency_table.find( symbol_name );

        currency_table.modify( current_currency, _self, [&]( auto& currency ) {
            currency.supply -= quantity;
        });
}

void nft::add_supply( asset quantity ) {

        auto symbol_name = quantity.symbol.code().raw();
        currency_index currency_table( _self, symbol_name );
        auto current_currency = currency_table.find( symbol_name );

        currency_table.modify( current_currency, name(0), [&]( auto& currency ) {
            currency.supply += quantity;
        });
}

EOSIO_DISPATCH( nft, (create)(issue)(transfer)(transferid)(setrampayer)(burn) )