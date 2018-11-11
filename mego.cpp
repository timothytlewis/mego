#include <eosiolib/eosio.hpp>

using namespace eosio;

CONTRACT mego : public eosio::contract {
private:
    TABLE ridestruct {
        uint64_t      prim_key;  // primary key
        name          user;      // account name for the user
        std::string   scooter;   // id of scooter to be ridden

        // primary key
        auto primary_key() const { return prim_key; }
        // secondary key
        // only supports uint64_t, uint128_t, uint256_t, double or long double
        uint64_t get_by_user() const { return user.value; }
    };

    // create a multi-index table and support secondary key
    typedef eosio::multi_index< name("ridestruct"), ridestruct,
    indexed_by< name("getbyuser"), const_mem_fun<ridestruct, uint64_t, &ridestruct::get_by_user> >
    > ride_table;

    ride_table _rides;

public:
    using contract::contract;

    // constructor
    mego( name receiver, name code, datastream<const char*> ds ):
    contract( receiver, code, ds ),
    _rides( receiver, receiver.value ) {}

    ACTION ride( name user, std::string& scooter ) {
        // to sign the action with the given account
        require_auth( user );

        // insert new ride
        _rides.emplace( _self, [&]( auto& new_ride ) {
            new_ride.prim_key    = _rides.available_primary_key();
            new_ride.user        = user;
            new_ride.scooter     = scooter;
        });
    }

};

// specify the contract name, and export a public action: ride
EOSIO_DISPATCH( mego, (ride) )
