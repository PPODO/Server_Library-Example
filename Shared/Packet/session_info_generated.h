// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_SESSIONINFO_FLATPACKET_SESSIONPACKET_H_
#define FLATBUFFERS_GENERATED_SESSIONINFO_FLATPACKET_SESSIONPACKET_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

#include "session_data_define_generated.h"

namespace FlatPacket {
namespace SessionPacket {

struct session_information;
struct session_informationBuilder;

struct session_information FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef session_informationBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SESSION_ID = 4,
    VT_SESSION_NAME = 6,
    VT_SESSION_CURRENT_USERS_COUNT = 8,
    VT_SESSION_USERS_MAX_LIMIT = 10,
    VT_USERS_INFORMATION = 12
  };
  int32_t session_id() const {
    return GetField<int32_t>(VT_SESSION_ID, 0);
  }
  bool KeyCompareLessThan(const session_information * const o) const {
    return session_id() < o->session_id();
  }
  int KeyCompareWithValue(int32_t _session_id) const {
    return static_cast<int>(session_id() > _session_id) - static_cast<int>(session_id() < _session_id);
  }
  const ::flatbuffers::String *session_name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_SESSION_NAME);
  }
  int16_t session_current_users_count() const {
    return GetField<int16_t>(VT_SESSION_CURRENT_USERS_COUNT, 0);
  }
  int16_t session_users_max_limit() const {
    return GetField<int16_t>(VT_SESSION_USERS_MAX_LIMIT, 0);
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::SessionPacket::session_user_info>> *users_information() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::SessionPacket::session_user_info>> *>(VT_USERS_INFORMATION);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_SESSION_ID, 4) &&
           VerifyOffset(verifier, VT_SESSION_NAME) &&
           verifier.VerifyString(session_name()) &&
           VerifyField<int16_t>(verifier, VT_SESSION_CURRENT_USERS_COUNT, 2) &&
           VerifyField<int16_t>(verifier, VT_SESSION_USERS_MAX_LIMIT, 2) &&
           VerifyOffset(verifier, VT_USERS_INFORMATION) &&
           verifier.VerifyVector(users_information()) &&
           verifier.VerifyVectorOfTables(users_information()) &&
           verifier.EndTable();
  }
};

struct session_informationBuilder {
  typedef session_information Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_session_id(int32_t session_id) {
    fbb_.AddElement<int32_t>(session_information::VT_SESSION_ID, session_id, 0);
  }
  void add_session_name(::flatbuffers::Offset<::flatbuffers::String> session_name) {
    fbb_.AddOffset(session_information::VT_SESSION_NAME, session_name);
  }
  void add_session_current_users_count(int16_t session_current_users_count) {
    fbb_.AddElement<int16_t>(session_information::VT_SESSION_CURRENT_USERS_COUNT, session_current_users_count, 0);
  }
  void add_session_users_max_limit(int16_t session_users_max_limit) {
    fbb_.AddElement<int16_t>(session_information::VT_SESSION_USERS_MAX_LIMIT, session_users_max_limit, 0);
  }
  void add_users_information(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::SessionPacket::session_user_info>>> users_information) {
    fbb_.AddOffset(session_information::VT_USERS_INFORMATION, users_information);
  }
  explicit session_informationBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<session_information> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<session_information>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<session_information> Createsession_information(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t session_id = 0,
    ::flatbuffers::Offset<::flatbuffers::String> session_name = 0,
    int16_t session_current_users_count = 0,
    int16_t session_users_max_limit = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FlatPacket::SessionPacket::session_user_info>>> users_information = 0) {
  session_informationBuilder builder_(_fbb);
  builder_.add_users_information(users_information);
  builder_.add_session_name(session_name);
  builder_.add_session_id(session_id);
  builder_.add_session_users_max_limit(session_users_max_limit);
  builder_.add_session_current_users_count(session_current_users_count);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<session_information> Createsession_informationDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t session_id = 0,
    const char *session_name = nullptr,
    int16_t session_current_users_count = 0,
    int16_t session_users_max_limit = 0,
    const std::vector<::flatbuffers::Offset<FlatPacket::SessionPacket::session_user_info>> *users_information = nullptr) {
  auto session_name__ = session_name ? _fbb.CreateString(session_name) : 0;
  auto users_information__ = users_information ? _fbb.CreateVector<::flatbuffers::Offset<FlatPacket::SessionPacket::session_user_info>>(*users_information) : 0;
  return FlatPacket::SessionPacket::Createsession_information(
      _fbb,
      session_id,
      session_name__,
      session_current_users_count,
      session_users_max_limit,
      users_information__);
}

inline const FlatPacket::SessionPacket::session_information *Getsession_information(const void *buf) {
  return ::flatbuffers::GetRoot<FlatPacket::SessionPacket::session_information>(buf);
}

inline const FlatPacket::SessionPacket::session_information *GetSizePrefixedsession_information(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<FlatPacket::SessionPacket::session_information>(buf);
}

inline bool Verifysession_informationBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FlatPacket::SessionPacket::session_information>(nullptr);
}

inline bool VerifySizePrefixedsession_informationBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FlatPacket::SessionPacket::session_information>(nullptr);
}

inline void Finishsession_informationBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatPacket::SessionPacket::session_information> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedsession_informationBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatPacket::SessionPacket::session_information> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace SessionPacket
}  // namespace FlatPacket

#endif  // FLATBUFFERS_GENERATED_SESSIONINFO_FLATPACKET_SESSIONPACKET_H_
