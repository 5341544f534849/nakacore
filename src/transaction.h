#ifndef NAKACORE_TRANSACTION_H
#define NAKACORE_TRANSACTION_H

#include "block.h"
#include "utxo.h"
#include "wallet.h"
#include <vector>
#include <iostream>

namespace NakaCore {

struct TxIn {
    OutPoint prevout;
    std::vector<uint8_t> signature;
};

class TransactionBuilder {
public:
    // Crear una transacción estándar de transferencia de fondos
    static Transaction CreateTransaction(const Wallet& sender, const std::string& recipient_address, uint64_t amount, const OutPoint& input_outpoint, uint64_t input_value) {
        Transaction tx;
        tx.version = 1;
        tx.lock_time = 0;

        // Configurar la entrada gastando el UTXO previo
        // (En Naka Core simplificamos la vinculación de entradas)

        // Salida para el destinatario
        TxOut recipient_out;
        recipient_out.value = amount;
        recipient_out.pk_script.assign(recipient_address.begin(), recipient_address.end());
        tx.vout.push_back(recipient_out);

        // Si hay cambio, devolverlo al remitente (simplificado)
        if (input_value > amount) {
            TxOut change_out;
            change_out.value = input_value - amount;
            std::string sender_addr = sender.GetAddressHex();
            change_out.pk_script.assign(sender_addr.begin(), sender_addr.end());
            tx.vout.push_back(change_out);
        }

        std::cout << "[+] Transaction created successfully. Value transferred: " << amount << " nakas.\n";
        return tx;
    }
};

} // namespace NakaCore

#endif // NAKACORE_TRANSACTION_H
