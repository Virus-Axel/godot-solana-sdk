#ifndef GODOT_SOLANA_SDK_HONEYCOMB_HPP
#define GODOT_SOLANA_SDK_HONEYCOMB_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include "transaction.hpp"
#include "honeycomb/honeycomb_generated.hpp"
#include "godot_cpp/classes/json.hpp"

namespace godot{

enum ResourceStorageEnum{
    AccountState = 0,
    LedgerState = 1,
};

enum LockTypeEnum{
    Freeze = 0,
    Custoday = 1,
};

enum BadgesCondition{
    Public = 0,
};

enum AssetAssemblerPermissionInput{
    ManageAssembler,
    UpdateBlock,
    UpdateBlockDefinition,
    UpdateNFT,
    InitialArtGeneration, 
};

enum AssetManagerPermissionInput{
    ManageAssets,
};

enum BuzzGuildPermissionInput{
    ManageGuildKit,
};

enum HiveControlPermissionInput{
    ManageCriterias,
    ManageServices,
    ManageIndexing,
    ManageProfiles,
};

enum CurrencyManagerPermissionInput{
    ManageCurrencies,
    MintCurrencies,
    ManageCurrencyStatus,
};

enum NectarMissionsPermissionInput{
    ManageMissionPool,
    WithdrawMissionPoolRewards,
};

enum NectarStakingPermissionInput{
    ManageStakingPool,
    WithdrawStakingPoolRewards,
};

class HoneyComb: public Node{
GDCLASS(HoneyComb, Node)
private:
    const String HONEYCOMB_URL = "https://edge.test.honeycombprotocol.com/";

    bool pending = false;
    Node* child = nullptr;
    HTTPRequest* api_request;
    Transaction* result_tx;

    String request;
    String method_name;
    Array args;

    Array signers;
    String query_fields = "";

    String build();
    void send_query();

    void bind_method_from_ref(const String ref);
    void add_arg(const String& name, const String& type_name, const Variant& value, bool optional = false);
    
    void query_response_callback(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body);
    void transaction_response_callback(const Dictionary& response);

    static void bind_non_changing_methods();

    template <typename T>
    void fetch_type_callback(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body){
        Dictionary response = JSON::parse_string(body.get_string_from_ascii());
        std::cout << body.get_string_from_ascii().ascii() << std::endl;

        if(response["data"].get_type() != Variant::DICTIONARY){
            ERR_FAIL_EDMSG("Error in request, check console logs");
        }

        Array method_response = ((Dictionary)response["data"])[method_name];
        std::cout << "AAA" << std::endl;
        std::cout << "AAA" << std::endl;
        Array ret;

        for(unsigned int i = 0; i < method_response.size(); i++){
            Dictionary element = method_response[i];

            T* item = memnew(T);
            Object::cast_to<T>(item)->from_dict(element);
            ret.append(item);
        }
        std::cout << "AAA" << std::endl;

        emit_signal("type_fetched", ret);
        std::cout << "AAA" << std::endl;
    }

    template <typename T>
    void fetch_type(){
        Callable callback = callable_mp(this, &HoneyComb::fetch_type_callback<T>);
        api_request->connect("request_completed", callback, CONNECT_ONE_SHOT);

        PackedStringArray headers;
        headers.append("content-type: application/json");
        add_child(api_request);
        child = api_request;
        pending = true;
        std::cout << "honey request: " << build().ascii() << std::endl;
        api_request->request(HONEYCOMB_URL, headers, HTTPClient::METHOD_POST, build());
    }

protected:
    HoneyComb();
    static void _bind_methods();
    ~HoneyComb();
public:

    HONEYCOMB_METHOD_DEFS
    /*Variant createCreateNewResourceTransaction(const Variant& project, const Variant& authority, Variant params, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateNewResourceTreeTransaction(const Variant& project, const Variant& resource, const Variant& authority, Variant treeConfig, const Variant& delegateAuthority = "", const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createMintResourceTransaction(const Variant& resource, const Variant& owner, const Variant& authority, int64_t amount, const Variant& delegateAuthority = "", const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createBurnResourceTransaction(const Variant& resource, int64_t amount, const Variant& authority, const Variant& owner = "", const Variant& payer = "", const Variant& delegateAuthority = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createNewUserTransaction(const Variant& wallet, Variant info = Variant(nullptr), const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateUnwrapHoldingTransaction(String resource, int64_t amount, const Variant& authority, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateWrapHoldingTransaction(String resource, int64_t amount, const Variant& authority, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createTransferResourceTransaction(String resource, const Variant& owner, String recipient, int64_t amount, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createInitializeRecipeTransaction(String project, int64_t xp, Array ingredients, Variant meal, const Variant& authority, String delegateAuthority = "", const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createUpdateUserTransaction(const Variant& payer, Variant info = Variant(nullptr), Variant wallets = Variant(nullptr), bool populateCivic = false, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateProfilesTreeTransaction(Variant treeConfig, const Variant& project, const Variant& payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createNewProfileTransaction(const Variant& project, const Variant& payer, String identity = "", Variant info = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createUpdateProfileTransaction(const Variant& profile, const Variant& payer, Variant info = Variant(nullptr), Variant customData = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createNewUserWithProfileTransaction(const Variant& project, const Variant& wallet, Variant userInfo = Variant(nullptr), String profileIdentity = "", Variant profileInfo = Variant(nullptr), const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createClaimBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createUpdateBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateProjectTransaction(const Variant& authority, String name, const Variant& driver = "", Array associatedPrograms = Array(), Variant profileDataConfig = Variant(nullptr), bool subsidizeFees = false, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createChangeProjectDriverTransaction(const Variant& project, const Variant& driver, const Variant& authority, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateDelegateAuthorityTransaction(const Variant& project, const Variant& delegate, Variant serviceDelegations, const Variant& authority, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createModifyDelegationTransaction(const Variant& project, const Variant& delegate, Variant modifyDelegation, const Variant& authority, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateAssemblerConfigTransaction(Variant treeConfig, String ticker, PackedStringArray order, const Variant& project, const Variant& authority, const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createInitializeBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);*/
};

}

#endif