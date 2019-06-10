#include "tx.h"

ssize_t waves_tx_from_bytes(tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes;
    uint8_t type = *src;
    switch (type)
    {
    case TRANSACTION_TYPE_ISSUE:
        nbytes = waves_issue_tx_from_bytes(&tx->data.issue, src);
        break;
    case TRANSACTION_TYPE_TRANSFER:
        nbytes = waves_transfer_tx_from_bytes(&tx->data.transfer, src);
        break;
    case TRANSACTION_TYPE_REISSUE:
        nbytes = waves_reissue_tx_from_bytes(&tx->data.reissue, src);
        break;
    case TRANSACTION_TYPE_BURN:
        nbytes = waves_burn_tx_from_bytes(&tx->data.burn, src);
        break;
    // case TRANSACTION_TYPE_EXCHANGE
    case TRANSACTION_TYPE_LEASE:
        nbytes = waves_lease_tx_from_bytes(&tx->data.lease, src);
        break;
    case TRANSACTION_TYPE_CANCEL_LEASE:
        nbytes = waves_lease_cancel_tx_from_bytes(&tx->data.lease_cancel, src);
        break;
    case TRANSACTION_TYPE_ALIAS:
        nbytes = waves_alias_tx_from_bytes(&tx->data.alias, src);
        break;
    case TRANSACTION_TYPE_MASS_TRANSFER:
        nbytes = waves_mass_transfer_tx_from_bytes(&tx->data.mass_transfer, src);
        break;
    case TRANSACTION_TYPE_DATA:
        nbytes = waves_data_tx_from_bytes(&tx->data.data, src);
        break;
    case TRANSACTION_TYPE_SET_SCRIPT:
        nbytes = waves_set_script_tx_from_bytes(&tx->data.set_script, src);
        break;
    case TRANSACTION_TYPE_SPONSORSHIP:
        nbytes = waves_sponsorship_tx_from_bytes(&tx->data.sponsorship, src);
        break;
    case TRANSACTION_TYPE_SET_ASSET_SCRIPT:
        nbytes = waves_set_asset_script_tx_from_bytes(&tx->data.set_asset_script, src);
        break;
    case TRANSACTION_TYPE_INVOKE_SCRIPT:
        nbytes = waves_invoke_script_tx_from_bytes(&tx->data.invoke_script, src);
        break;
    default:
        return -1;
    }
    tx->type = type;
    return nbytes;
}

size_t waves_tx_to_bytes(unsigned char *dst, const tx_bytes_t* tx)
{
    switch (tx->type) {
    case TRANSACTION_TYPE_ISSUE:
        return waves_issue_tx_to_bytes(dst, &tx->data.issue);
    case TRANSACTION_TYPE_TRANSFER:
        return waves_transfer_tx_to_bytes(dst, &tx->data.transfer);
    case TRANSACTION_TYPE_REISSUE:
        return waves_reissue_tx_to_bytes(dst, &tx->data.reissue);
    case TRANSACTION_TYPE_BURN:
        return waves_burn_tx_to_bytes(dst, &tx->data.burn);
    // case TRANSACTION_TYPE_EXCHANGE
    case TRANSACTION_TYPE_LEASE:
        return waves_lease_tx_to_bytes(dst, &tx->data.lease);
    case TRANSACTION_TYPE_CANCEL_LEASE:
        return waves_lease_cancel_tx_to_bytes(dst, &tx->data.lease_cancel);
    case TRANSACTION_TYPE_ALIAS:
        return waves_alias_tx_to_bytes(dst, &tx->data.alias);
    case TRANSACTION_TYPE_MASS_TRANSFER:
        return waves_mass_transfer_tx_to_bytes(dst, &tx->data.mass_transfer);
    case TRANSACTION_TYPE_DATA:
        return waves_data_tx_to_bytes(dst, &tx->data.data);
    case TRANSACTION_TYPE_SET_SCRIPT:
        return waves_set_script_tx_to_bytes(dst, &tx->data.set_script);
    case TRANSACTION_TYPE_SPONSORSHIP:
        return waves_sponsorship_tx_to_bytes(dst, &tx->data.sponsorship);
    case TRANSACTION_TYPE_SET_ASSET_SCRIPT:
        return waves_set_asset_script_tx_to_bytes(dst, &tx->data.set_asset_script);
    case TRANSACTION_TYPE_INVOKE_SCRIPT:
        return waves_invoke_script_tx_to_bytes(dst, &tx->data.invoke_script);
    default:
        return 0;
    }
}

void waves_destroy_tx(tx_bytes_t* tx)
{
    switch (tx->type)
    {
        case TRANSACTION_TYPE_ISSUE:
            waves_destroy_issue_tx(&tx->data.issue);
            break;
        case TRANSACTION_TYPE_TRANSFER:
            waves_destroy_transfer_tx(&tx->data.transfer);
            break;
        case TRANSACTION_TYPE_MASS_TRANSFER:
            waves_destroy_mass_transfer_tx(&tx->data.mass_transfer);
            break;
        case TRANSACTION_TYPE_DATA:
            waves_destroy_data_tx(&tx->data.data);
            break;
        case TRANSACTION_TYPE_SET_SCRIPT:
            waves_destroy_set_script_tx(&tx->data.set_script);
            break;
        case TRANSACTION_TYPE_SET_ASSET_SCRIPT:
            waves_destroy_set_asset_script_tx(&tx->data.set_asset_script);
            break;
        case TRANSACTION_TYPE_INVOKE_SCRIPT:
            waves_destroy_invoke_script_tx(&tx->data.invoke_script);
            break;
        case TRANSACTION_TYPE_REISSUE:
        case TRANSACTION_TYPE_BURN:
        case TRANSACTION_TYPE_LEASE:
        case TRANSACTION_TYPE_CANCEL_LEASE:
        case TRANSACTION_TYPE_ALIAS:
        case TRANSACTION_TYPE_SPONSORSHIP:
            // do nothing
        default:;
    }
    tx->type = 0;
}

