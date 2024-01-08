#include <market.hpp>

ACTION market::init() {
    require_auth(get_self());
    config.get_or_create(get_self(), config_a{});
}
ACTION market::deletemarket(uint64_t sale_id) {
  auto sale_itr = sales.require_find(sale_id,"No Sale with this id exists");

  require_auth(get_self());

  sales.erase(sale_itr);
}

ACTION market::cancelmarket(uint64_t sale_id) {
  auto sale_itr = sales.require_find(sale_id,"No Sale with this id exists");

  require_auth(sale_itr->seller);

  sales.erase(sale_itr);
}

ACTION market::uploadmarket(name seller,name col_name,name schema_name,uint64_t asset_id,asset price,name asset_name, string asset_img) {
    vector <uint64_t> empty;
    string memo = "uploadmarket";
     require_auth(seller);
    config_t_for_abi config_table(name("eosio.nft"), name("eosio.nft").value); 
    auto itr = config_table.begin();
    uint64_t offer_id_value = itr->offer_counter;
    for (auto itr = sales.begin(); itr != sales.end(); ++itr) {
      if (itr->asset_id == asset_id && itr->is_sale == 0) {
          check(false, "이미 등록된 asset_id가 있습니다.");
      }
    }
    config_a current_config = config.get();
    uint64_t market_id = current_config.market_counter++;
    sales.emplace(seller, [&](auto& row) {
      row.sale_id = market_id;
      row.seller = seller;
      row.collection_name = col_name;
      row.schema_name = schema_name;
      row.asset_id = asset_id;
      row.price = price;
      row.offer_id = offer_id_value;
      row.asset_name = asset_name;
      row.asset_img = asset_img;
    
    });
    config.set(current_config, get_self());
}
void market::buynft(name buyer,vector <uint64_t> asset_id,uint64_t sale_id,uint64_t offer_id){
  require_auth(buyer);
  string memo = "uploadmarket";

    action(
      permission_level{get_self(), "active"_n},
      "eosio.nft"_n, // eosio.token 컨트랙트의 계정 이름
      "acceptoffer"_n,    // 호출할 액션 이름
      std::make_tuple(offer_id)
    ).send();
    action(
      permission_level{get_self(), "active"_n},
      "eosio.nft"_n, // eosio.token 컨트랙트의 계정 이름
      "transfer"_n,    // 호출할 액션 이름
      std::make_tuple(get_self(), buyer, asset_id, memo)
    ).send();
     auto sale_itr = sales.require_find(sale_id,"No Sale with this id exists");
       sales.modify(sale_itr, same_payer, [&](auto& sale) {
      sale.is_sale = 1;
      sale.buyer = buyer;
    });
}
void market::internal_transfer(name from,name to,asset quantity,string memo){
    action(
      permission_level{get_self(), "active"_n},
      "eosio.token"_n, // eosio.token 컨트랙트의 계정 이름
      "transfer"_n,    // 호출할 액션 이름
      std::make_tuple(from, to, quantity, memo)
    ).send();
}








