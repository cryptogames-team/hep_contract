#include <item_oasis.hpp>

ACTION item_oasis::init() {
    require_auth(get_self());
    config.get_or_create(get_self(), config_a{});
}

ACTION item_oasis::create(name seller,name buyer,uint64_t transaction_board_id,asset price, uint64_t date, uint64_t game_id, uint64_t item_count,uint64_t game_server,uint64_t item_type) {

    require_auth(get_self());

    config_a current_config = config.get();
    uint64_t transaction_id = current_config.transaction_counter++;
    transactions.emplace(get_self(), [&](auto& row) {
      row.transaction_id = transaction_id;
      row.seller = seller;
      row.buyer = buyer;
      row.buy_confirmation = 0;
      row.sell_confirmation = 0; 
      row.transaction_completed = 0;  
      row.is_fraud = 0;
      row.transaction_board_id = transaction_board_id;
      row.price = price;
      row.date = date;
      row.game_id = game_id;
      row.item_count = item_count;
      row.game_server = game_server;
      row.item_type = item_type;
    });
    config.set(current_config, get_self());
}

ACTION item_oasis::buyconfirmed(uint64_t transaction_id){
    require_auth(get_self());
    string memo = "Complete";
    auto transaction_itr = transactions.require_find(transaction_id,"No Transaction with this id exists");
    if (transaction_itr->sell_confirmation == 0) {
          check(false, "물품이 아직 전달이 되지 않았습니다.");
      }
      transactions.modify(transaction_itr, same_payer, [&](auto& transaction) {
      transaction.buy_confirmation = 1;
      if(transaction.sell_confirmation == 1){
        transaction.transaction_completed = 1;

        action(
          permission_level{get_self(), name("active")},
          name("eosio.token"), // eosio.token 컨트랙트의 계정 이름
          "transfer"_n,    // 호출할 액션 이름
          std::make_tuple(
            get_self(), 
            transaction.seller, 
            transaction.price, 
            memo)
        ).send();
      }
    });
}

ACTION item_oasis::saleconfirmed(uint64_t transaction_id){
    require_auth(get_self());
    string memo = "Complete";
    auto transaction_itr = transactions.require_find(transaction_id,"No Transaction with this id exists");
      transactions.modify(transaction_itr, same_payer, [&](auto& transaction) {
      transaction.sell_confirmation = 1;
      
    });
}

ACTION item_oasis::setisfraud(uint64_t transaction_id){
    require_auth(get_self());
    string memo = "Refund";
    auto transaction_itr = transactions.require_find(transaction_id,"No Transaction with this id exists");
      transactions.modify(transaction_itr, same_payer, [&](auto& transaction) {
      transaction.is_fraud = 1;
      action(
          permission_level{get_self(), name("active")},
          name("eosio.token"), // eosio.token 컨트랙트의 계정 이름
          "transfer"_n,    // 호출할 액션 이름
          std::make_tuple(
            get_self(), 
            transaction.buyer, 
            transaction.price, 
            memo)
        ).send();
  });
}

ACTION item_oasis::expiration(uint64_t date){
  require_auth(get_self());
  string memo = "Complete";
  for (auto itr = transactions.begin(); itr != transactions.end(); ++itr) {
      uint64_t just_date = itr->date / 1000000;
      if (just_date == date && itr->transaction_completed == 0 && itr->buy_confirmation == 0) {
          transactions.modify(itr,same_payer,[&](auto& transaction) {
            transaction.buy_confirmation = 1;
            if(transaction.sell_confirmation == 1 && transaction.is_fraud == 0){
              transaction.transaction_completed = 1;

              action(
                permission_level{get_self(), name("active")},
                name("eosio.token"), // eosio.token 컨트랙트의 계정 이름
                "transfer"_n,    // 호출할 액션 이름
                std::make_tuple(
                  get_self(), 
                  transaction.seller, 
                  transaction.price, 
                  memo)
              ).send();
            }
          });
      }
    }
}
