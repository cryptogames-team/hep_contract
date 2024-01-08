#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

CONTRACT market : public contract {
public:
  using contract::contract;
    ACTION init();
    ACTION deletemarket(uint64_t sale_id);
    ACTION uploadmarket(name seller,name col_name,name schema_name,uint64_t asset_id,asset price,name asset_name, string asset_img);
    ACTION cancelmarket(uint64_t sale_id);
    ACTION buynft(name buyer,vector <uint64_t> asset_id,uint64_t sale_id,uint64_t offer_id);


    void internal_transfer(
        name from,
        name to,
        asset quantity,
        string memo
    );

    TABLE sales_s {
    uint64_t         sale_id;
    name             seller;
    name             collection_name;
    name             schema_name;
    uint64_t         asset_id;
    asset            price;
    uint64_t         offer_id;
    name             asset_name;
    string           asset_img;
    name             buyer;
    uint64_t             is_sale = 0;

    uint64_t primary_key() const { return sale_id; };
    uint64_t by_collection() const { return collection_name.value; }; // 컬렉션 이름을 기준으로 하는 인덱스
    uint64_t by_schema() const { return schema_name.value; }; // 스키마 이름을 기준으로 하는 인덱스
    uint64_t by_seller() const { return seller.value; }; // 스키마 이름을 기준으로 하는 인덱스
    uint64_t by_asset_name() const { return asset_name.value; }; // 스키마 이름을 기준으로 하는 인덱스
    uint64_t by_buyer() const { return buyer.value; }; // 스키마 이름을 기준으로 하는 인덱스
    uint64_t by_is_sale() const { return is_sale; }; // 스키마 이름을 기준으로 하는 인덱스
    uint64_t by_asset_id() const { return asset_id; }; // 스키마 이름을 기준으로 하는 인덱스
  };

  typedef multi_index<name("sales"), sales_s,
    indexed_by<name("bycollection"), const_mem_fun<sales_s, uint64_t, &sales_s::by_collection>>,
    indexed_by<name("byseller"), const_mem_fun<sales_s, uint64_t, &sales_s::by_seller>>,
    indexed_by<name("byassetname"), const_mem_fun<sales_s, uint64_t, &sales_s::by_asset_name>>,
    indexed_by<name("bybuyer"), const_mem_fun<sales_s, uint64_t, &sales_s::by_buyer>>,
    indexed_by<name("byissale"), const_mem_fun<sales_s, uint64_t, &sales_s::by_is_sale>>,
    indexed_by<name("byschema"), const_mem_fun<sales_s, uint64_t, &sales_s::by_schema>>,
    indexed_by<name("byassetid"), const_mem_fun<sales_s, uint64_t, &sales_s::by_asset_id>>

> sales_t;

    TABLE config_a {
        uint64_t                 market_counter     = 1;
    };
    typedef singleton <name("configs"), config_a>               config_t;
    typedef multi_index <name("configs"), config_a>             configs_t_for_abi;

    config_t       config       = config_t(get_self(), get_self().value);

struct  config_s {
  uint32_t                 primary_keys = 1;
        uint64_t                 offer_counter = 1;
        
        uint64_t primary_key() const { return primary_keys; };
    };
    typedef singleton <name("config"), config_s>  configs_offer;
typedef eosio::multi_index<name("config"), config_s> config_t_for_abi;
struct assets_s {
	uint64_t         asset_id;
    	name             collection_name;
    	name             schema_name;
    	int32_t          template_id;
    	name             ram_payer;
    	vector <asset>   backed_tokens;
    	vector <uint8_t> immutable_serialized_data;
    	vector <uint8_t> mutable_serialized_data;
	uint64_t primary_key() const { return asset_id; };
};
typedef eosio::multi_index< "assets"_n, assets_s > assets;
  
sales_t       sales       = sales_t(get_self(), get_self().value);

};