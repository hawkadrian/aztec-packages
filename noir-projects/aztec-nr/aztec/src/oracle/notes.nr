use crate::{
    note::{note_interface::NoteType, note_metadata::NoteMetadata, retrieved_note::RetrievedNote},
    utils::array,
};

use dep::protocol_types::{
    address::AztecAddress,
    indexed_tagging_secret::IndexedTaggingSecret,
    traits::{FromField, Packable},
};

/// Notifies the simulator that a note has been created, so that it can be returned in future read requests in the same
/// transaction. This note should only be added to the non-volatile database if found in an actual block.
pub fn notify_created_note<let N: u32>(
    storage_slot: Field,
    note_type_id: Field,
    packed_note: [Field; N],
    note_hash: Field,
    counter: u32,
) {
    // Safety: This oracle call returns nothing: we only call it for its side effects. It is therefore always safe
    // to call.
    unsafe {
        notify_created_note_oracle_wrapper(
            storage_slot,
            note_type_id,
            packed_note,
            note_hash,
            counter,
        )
    };
}

/// Notifies the simulator that a note has been nullified, so that it is no longer returned in future read requests in
/// the same transaction. This note should only be removed to the non-volatile database if its nullifier is found in an
/// actual block.
pub fn notify_nullified_note(nullifier: Field, note_hash: Field, counter: u32) {
    // Safety: This oracle call returns nothing: we only call it for its side effects. It is therefore always safe to
    // call.
    unsafe { notify_nullified_note_oracle_wrapper(nullifier, note_hash, counter) };
}

/// Notifies the simulator that a non-note nullifier has been created, so that it can be used for note nonces.
pub fn notify_created_nullifier(nullifier: Field) {
    // Safety: This oracle call returns nothing: we only call it for its side effects. It is therefore always safe to
    // call.
    unsafe { notify_created_nullifier_oracle_wrapper(nullifier) };
}

unconstrained fn notify_created_note_oracle_wrapper<let N: u32>(
    storage_slot: Field,
    note_type_id: Field,
    packed_note: [Field; N],
    note_hash: Field,
    counter: u32,
) {
    notify_created_note_oracle(storage_slot, note_type_id, packed_note, note_hash, counter);
}

#[oracle(notifyCreatedNote)]
unconstrained fn notify_created_note_oracle<let N: u32>(
    _storage_slot: Field,
    _note_type_id: Field,
    _packed_note: [Field; N],
    _note_hash: Field,
    _counter: u32,
) {}

unconstrained fn notify_nullified_note_oracle_wrapper(
    nullifier: Field,
    note_hash: Field,
    counter: u32,
) {
    notify_nullified_note_oracle(nullifier, note_hash, counter);
}

#[oracle(notifyNullifiedNote)]
unconstrained fn notify_nullified_note_oracle(_nullifier: Field, _note_hash: Field, _counter: u32) {}

unconstrained fn notify_created_nullifier_oracle_wrapper(nullifier: Field) {
    notify_created_nullifier_oracle(nullifier);
}

#[oracle(notifyCreatedNullifier)]
unconstrained fn notify_created_nullifier_oracle(_nullifier: Field) {}

#[oracle(getNotes)]
unconstrained fn get_notes_oracle<let N: u32, let ORACLE_RETURN_FIELD_LENGTH: u32>(
    _storage_slot: Field,
    _num_selects: u8,
    _select_by_indexes: [u8; N],
    _select_by_offsets: [u8; N],
    _select_by_lengths: [u8; N],
    _select_values: [Field; N],
    _select_comparators: [u8; N],
    _sort_by_indexes: [u8; N],
    _sort_by_offsets: [u8; N],
    _sort_by_lengths: [u8; N],
    _sort_order: [u8; N],
    _limit: u32,
    _offset: u32,
    _status: u8,
    _return_size: u32,
) -> [Field; ORACLE_RETURN_FIELD_LENGTH] {}

unconstrained fn get_notes_oracle_wrapper<let N: u32, let ORACLE_RETURN_FIELD_LENGTH: u32>(
    storage_slot: Field,
    num_selects: u8,
    select_by_indexes: [u8; N],
    select_by_offsets: [u8; N],
    select_by_lengths: [u8; N],
    select_values: [Field; N],
    select_comparators: [u8; N],
    sort_by_indexes: [u8; N],
    sort_by_offsets: [u8; N],
    sort_by_lengths: [u8; N],
    sort_order: [u8; N],
    limit: u32,
    offset: u32,
    status: u8,
) -> [Field; ORACLE_RETURN_FIELD_LENGTH] {
    // This wrapper exists to extract the ORACLE_RETURN_FIELD_LENGTH generic numeric param into a value and pass it to
    // the oracle, so that it knows how big the return array must be.
    get_notes_oracle(
        storage_slot,
        num_selects,
        select_by_indexes,
        select_by_offsets,
        select_by_lengths,
        select_values,
        select_comparators,
        sort_by_indexes,
        sort_by_offsets,
        sort_by_lengths,
        sort_order,
        limit,
        offset,
        status,
        ORACLE_RETURN_FIELD_LENGTH,
    )
}

pub unconstrained fn get_notes<Note, let N: u32, let M: u32, let MAX_NOTES: u32, let ORACLE_RETURN_FIELD_LENGTH: u32>(
    storage_slot: Field,
    num_selects: u8,
    select_by_indexes: [u8; M],
    select_by_offsets: [u8; M],
    select_by_lengths: [u8; M],
    select_values: [Field; M],
    select_comparators: [u8; M],
    sort_by_indexes: [u8; M],
    sort_by_offsets: [u8; M],
    sort_by_lengths: [u8; M],
    sort_order: [u8; M],
    limit: u32,
    offset: u32,
    status: u8,
    _placeholder_fields: [Field; ORACLE_RETURN_FIELD_LENGTH], // TODO: Compute this value automatically from MAX_NOTES
) -> [Option<RetrievedNote<Note>>; MAX_NOTES]
where
    Note: NoteType + Packable<N>,
{
    let fields: [_; ORACLE_RETURN_FIELD_LENGTH] = get_notes_oracle_wrapper(
        storage_slot,
        num_selects,
        select_by_indexes,
        select_by_offsets,
        select_by_lengths,
        select_values,
        select_comparators,
        sort_by_indexes,
        sort_by_offsets,
        sort_by_lengths,
        sort_order,
        limit,
        offset,
        status,
    );
    let num_notes = fields[0] as u32;
    let contract_address = AztecAddress::from_field(fields[1]);

    let mut opt_notes = [Option::none(); MAX_NOTES];
    for i in 0..opt_notes.len() {
        if i < num_notes {
            // lengths named as per typescript.
            let return_header_length: u32 = 2; // num_notes & contract_address.
            let extra_preimage_length: u32 = 2; // nonce & note_hash_counter.
            let read_offset: u32 = return_header_length + i * (N + extra_preimage_length);

            let maybe_nonce = fields[read_offset];
            let maybe_note_hash_counter = fields[read_offset + 1] as u32;
            let packed_note = array::subarray(fields, read_offset + 2);

            let note = Note::unpack(packed_note);
            let retrieved_note = RetrievedNote {
                note,
                contract_address,
                metadata: NoteMetadata::from_raw_data(maybe_note_hash_counter != 0, maybe_nonce),
            };

            opt_notes[i] = Option::some(retrieved_note);
        };
    }
    opt_notes
}

/// Returns true if the nullifier exists. Note that a `true` value can be constrained by proving existence of the
/// nullifier, but a `false` value should not be relied upon since other transactions may emit this nullifier before the
/// current transaction is included in a block. While this might seem of little use at first, certain design patterns
/// benefit from this abstraction (see e.g. `PrivateMutable`).
pub unconstrained fn check_nullifier_exists(inner_nullifier: Field) -> bool {
    check_nullifier_exists_oracle(inner_nullifier)
}

#[oracle(checkNullifierExists)]
unconstrained fn check_nullifier_exists_oracle(_inner_nullifier: Field) -> bool {}

/// Returns the derived app tagging secret ready to be included in a log for a given sender and recipient pair,
/// siloed for the current contract address.
pub unconstrained fn get_app_tag_as_sender(sender: AztecAddress, recipient: AztecAddress) -> Field {
    get_indexed_tagging_secret_as_sender_oracle(sender, recipient).compute_tag(recipient)
}

#[oracle(getIndexedTaggingSecretAsSender)]
unconstrained fn get_indexed_tagging_secret_as_sender_oracle(
    _sender: AztecAddress,
    _recipient: AztecAddress,
) -> IndexedTaggingSecret {}

/// Notifies the simulator that a tag has been used in a note, and to therefore increment the associated index so that
/// future notes get a different tag and can be discovered by the recipient.
/// This change should only be persisted in a non-volatile database if the tagged log is found in an actual block -
/// otherwise e.g. a reverting transaction can cause the sender to accidentally skip indices and later produce notes
/// that are not found by the recipient.
pub fn increment_app_tagging_secret_index_as_sender(sender: AztecAddress, recipient: AztecAddress) {
    // Safety: This oracle call returns nothing: we only call it for its side effects. It is therefore always safe
    // to call.
    unsafe {
        increment_app_tagging_secret_index_as_sender_wrapper(sender, recipient);
    }
}

unconstrained fn increment_app_tagging_secret_index_as_sender_wrapper(
    sender: AztecAddress,
    recipient: AztecAddress,
) {
    increment_app_tagging_secret_index_as_sender_oracle(sender, recipient);
}

#[oracle(incrementAppTaggingSecretIndexAsSender)]
unconstrained fn increment_app_tagging_secret_index_as_sender_oracle(
    _sender: AztecAddress,
    _recipient: AztecAddress,
) {}
