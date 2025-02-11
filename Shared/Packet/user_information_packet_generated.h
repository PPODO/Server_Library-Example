// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_USERINFORMATIONPACKET_FLATPACKET_USERPACKET_H_
#define FLATBUFFERS_GENERATED_USERINFORMATIONPACKET_FLATPACKET_USERPACKET_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

#include "data_define_generated.h"

namespace FlatPacket {
namespace UserPacket {

struct UserInformationPacket;
struct UserInformationPacketBuilder;

struct UserInformationPacket FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef UserInformationPacketBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_REQUEST_RESULT = 4,
    VT_UUID = 6,
    VT_NUMBER_OF_UNCONFIRMED_MAILS = 8,
    VT_INVENTORY_LIST = 10
  };
  int16_t request_result() const {
    return GetField<int16_t>(VT_REQUEST_RESULT, 0);
  }
  int32_t uuid() const {
    return GetField<int32_t>(VT_UUID, 0);
  }
  int32_t number_of_unconfirmed_mails() const {
    return GetField<int32_t>(VT_NUMBER_OF_UNCONFIRMED_MAILS, 0);
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::item>> *inventory_list() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::item>> *>(VT_INVENTORY_LIST);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int16_t>(verifier, VT_REQUEST_RESULT, 2) &&
           VerifyField<int32_t>(verifier, VT_UUID, 4) &&
           VerifyField<int32_t>(verifier, VT_NUMBER_OF_UNCONFIRMED_MAILS, 4) &&
           VerifyOffset(verifier, VT_INVENTORY_LIST) &&
           verifier.VerifyVector(inventory_list()) &&
           verifier.VerifyVectorOfTables(inventory_list()) &&
           verifier.EndTable();
  }
};

struct UserInformationPacketBuilder {
  typedef UserInformationPacket Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_request_result(int16_t request_result) {
    fbb_.AddElement<int16_t>(UserInformationPacket::VT_REQUEST_RESULT, request_result, 0);
  }
  void add_uuid(int32_t uuid) {
    fbb_.AddElement<int32_t>(UserInformationPacket::VT_UUID, uuid, 0);
  }
  void add_number_of_unconfirmed_mails(int32_t number_of_unconfirmed_mails) {
    fbb_.AddElement<int32_t>(UserInformationPacket::VT_NUMBER_OF_UNCONFIRMED_MAILS, number_of_unconfirmed_mails, 0);
  }
  void add_inventory_list(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::item>>> inventory_list) {
    fbb_.AddOffset(UserInformationPacket::VT_INVENTORY_LIST, inventory_list);
  }
  explicit UserInformationPacketBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<UserInformationPacket> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<UserInformationPacket>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<UserInformationPacket> CreateUserInformationPacket(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int16_t request_result = 0,
    int32_t uuid = 0,
    int32_t number_of_unconfirmed_mails = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::item>>> inventory_list = 0) {
  UserInformationPacketBuilder builder_(_fbb);
  builder_.add_inventory_list(inventory_list);
  builder_.add_number_of_unconfirmed_mails(number_of_unconfirmed_mails);
  builder_.add_uuid(uuid);
  builder_.add_request_result(request_result);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<UserInformationPacket> CreateUserInformationPacketDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int16_t request_result = 0,
    int32_t uuid = 0,
    int32_t number_of_unconfirmed_mails = 0,
    const std::vector<::flatbuffers::Offset<FlatPacket::item>> *inventory_list = nullptr) {
  auto inventory_list__ = inventory_list ? _fbb.CreateVector<::flatbuffers::Offset<FlatPacket::item>>(*inventory_list) : 0;
  return FlatPacket::UserPacket::CreateUserInformationPacket(
      _fbb,
      request_result,
      uuid,
      number_of_unconfirmed_mails,
      inventory_list__);
}

inline const FlatPacket::UserPacket::UserInformationPacket *GetUserInformationPacket(const void *buf) {
  return ::flatbuffers::GetRoot<FlatPacket::UserPacket::UserInformationPacket>(buf);
}

inline const FlatPacket::UserPacket::UserInformationPacket *GetSizePrefixedUserInformationPacket(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<FlatPacket::UserPacket::UserInformationPacket>(buf);
}

inline bool VerifyUserInformationPacketBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FlatPacket::UserPacket::UserInformationPacket>(nullptr);
}

inline bool VerifySizePrefixedUserInformationPacketBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FlatPacket::UserPacket::UserInformationPacket>(nullptr);
}

inline void FinishUserInformationPacketBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatPacket::UserPacket::UserInformationPacket> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedUserInformationPacketBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatPacket::UserPacket::UserInformationPacket> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace UserPacket
}  // namespace FlatPacket

#endif  // FLATBUFFERS_GENERATED_USERINFORMATIONPACKET_FLATPACKET_USERPACKET_H_
