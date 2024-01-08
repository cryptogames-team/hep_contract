#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

CONTRACT item_oasis : public contract {
public:
  using contract::contract;
    ACTION init();
    ACTION create(name seller,name buyer,uint64_t transaction_board_id,asset price,uint64_t date);
    ACTION buyconfirmed(uint64_t transaction_id);
    ACTION saleconfirmed(uint64_t transaction_id);
    ACTION setisfraud(uint64_t transaction_id);
    ACTION expiration(uint64_t date);
    TABLE transaction_s {
    uint64_t         transaction_id;
    name             seller;
    name             buyer;
    uint8_t          buy_confirmation;
    uint8_t          sell_confirmation;    
    uint8_t          trasaction_completed;
    uint8_t          is_fraud;
    uint64_t         transaction_board_id;
    asset            price;
    uint64_t         date;

    uint64_t primary_key() const { return transaction_id; };
    uint64_t by_seller() const { return seller.value; }; 
    uint64_t by_buyer() const { return buyer.value; }; 
    uint64_t by_buy_confirmation() const { return buy_confirmation; }; 
    uint64_t by_sell_confirmation() const { return sell_confirmation; }; 
    uint64_t by_trasaction_completed() const { return trasaction_completed; }; 
    uint64_t by_fraud() const { return is_fraud; };
    uint64_t by_date() const { return date; };
  };

  typedef multi_index<name("transactions"), transaction_s,
    indexed_by<name("byseller"), const_mem_fun<transaction_s, uint64_t, &transaction_s::by_seller>>,
    indexed_by<name("bybuyer"), const_mem_fun<transaction_s, uint64_t, &transaction_s::by_buyer>>,
    indexed_by<name("bybuyconf"), const_mem_fun<transaction_s, uint64_t, &transaction_s::by_buy_confirmation>>,
    indexed_by<name("bysellconf"), const_mem_fun<transaction_s, uint64_t, &transaction_s::by_sell_confirmation>>,
    indexed_by<name("bytrxcom"), const_mem_fun<transaction_s, uint64_t, &transaction_s::by_trasaction_completed>>,
    indexed_by<name("bydate"), const_mem_fun<transaction_s, uint64_t, &transaction_s::by_date>>,
    indexed_by<name("byfraud"), const_mem_fun<transaction_s, uint64_t, &transaction_s::by_fraud>>
    > transaction_t;

    TABLE config_a {
        uint64_t                 transaction_counter     = 1;
    };
    typedef singleton <name("configs"), config_a>               config_t;
    typedef multi_index <name("configs"), config_a>             configs_t_for_abi;

    config_t       config       = config_t(get_self(), get_self().value);

  
transaction_t       transactions       = transaction_t(get_self(), get_self().value);

};