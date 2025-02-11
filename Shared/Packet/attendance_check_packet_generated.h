// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_ATTENDANCECHECKPACKET_FLATPACKET_ATTENDANCEPACKET_H_
#define FLATBUFFERS_GENERATED_ATTENDANCECHECKPACKET_FLATPACKET_ATTENDANCEPACKET_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

#include "attendance_data_define_generated.h"
#include "data_define_generated.h"

namespace FlatPacket {
namespace AttendancePacket {

struct AttendanceCheckPacket;
struct AttendanceCheckPacketBuilder;

enum AttendanceCheckMessageType : int8_t {
  AttendanceCheckMessageType_None = (1 << 0),
  AttendanceCheckMessageType_TodayAttendance = (1 << 1),
  AttendanceCheckMessageType_NextDayAttendance = (1 << 2),
  AttendanceCheckMessageType_LeftAllDayAttendance = (1 << 3),
  AttendanceCheckMessageType_MIN = AttendanceCheckMessageType_None,
  AttendanceCheckMessageType_MAX = AttendanceCheckMessageType_LeftAllDayAttendance
};

inline const AttendanceCheckMessageType (&EnumValuesAttendanceCheckMessageType())[4] {
  static const AttendanceCheckMessageType values[] = {
    AttendanceCheckMessageType_None,
    AttendanceCheckMessageType_TodayAttendance,
    AttendanceCheckMessageType_NextDayAttendance,
    AttendanceCheckMessageType_LeftAllDayAttendance
  };
  return values;
}

inline const char * const *EnumNamesAttendanceCheckMessageType() {
  static const char * const names[5] = {
    "None",
    "TodayAttendance",
    "NextDayAttendance",
    "LeftAllDayAttendance",
    nullptr
  };
  return names;
}

inline const char *EnumNameAttendanceCheckMessageType(AttendanceCheckMessageType e) {
  if (::flatbuffers::IsOutRange(e, AttendanceCheckMessageType_None, AttendanceCheckMessageType_LeftAllDayAttendance)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesAttendanceCheckMessageType()[index];
}

struct AttendanceCheckPacket FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef AttendanceCheckPacketBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_REQUEST_RESULT = 4,
    VT_ATTENDANCE_TYPE = 6,
    VT_EVENT_KEY = 8,
    VT_UUID = 10,
    VT_UPDATED_ATTENDANCE_COUNT = 12,
    VT_COST_ITEM_ID = 14,
    VT_COST_ITEM_COUNT = 16
  };
  int16_t request_result() const {
    return GetField<int16_t>(VT_REQUEST_RESULT, 0);
  }
  int8_t attendance_type() const {
    return GetField<int8_t>(VT_ATTENDANCE_TYPE, 0);
  }
  int32_t event_key() const {
    return GetField<int32_t>(VT_EVENT_KEY, 0);
  }
  int32_t uuid() const {
    return GetField<int32_t>(VT_UUID, 0);
  }
  int32_t updated_attendance_count() const {
    return GetField<int32_t>(VT_UPDATED_ATTENDANCE_COUNT, 0);
  }
  int32_t cost_item_id() const {
    return GetField<int32_t>(VT_COST_ITEM_ID, 0);
  }
  int32_t cost_item_count() const {
    return GetField<int32_t>(VT_COST_ITEM_COUNT, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int16_t>(verifier, VT_REQUEST_RESULT, 2) &&
           VerifyField<int8_t>(verifier, VT_ATTENDANCE_TYPE, 1) &&
           VerifyField<int32_t>(verifier, VT_EVENT_KEY, 4) &&
           VerifyField<int32_t>(verifier, VT_UUID, 4) &&
           VerifyField<int32_t>(verifier, VT_UPDATED_ATTENDANCE_COUNT, 4) &&
           VerifyField<int32_t>(verifier, VT_COST_ITEM_ID, 4) &&
           VerifyField<int32_t>(verifier, VT_COST_ITEM_COUNT, 4) &&
           verifier.EndTable();
  }
};

struct AttendanceCheckPacketBuilder {
  typedef AttendanceCheckPacket Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_request_result(int16_t request_result) {
    fbb_.AddElement<int16_t>(AttendanceCheckPacket::VT_REQUEST_RESULT, request_result, 0);
  }
  void add_attendance_type(int8_t attendance_type) {
    fbb_.AddElement<int8_t>(AttendanceCheckPacket::VT_ATTENDANCE_TYPE, attendance_type, 0);
  }
  void add_event_key(int32_t event_key) {
    fbb_.AddElement<int32_t>(AttendanceCheckPacket::VT_EVENT_KEY, event_key, 0);
  }
  void add_uuid(int32_t uuid) {
    fbb_.AddElement<int32_t>(AttendanceCheckPacket::VT_UUID, uuid, 0);
  }
  void add_updated_attendance_count(int32_t updated_attendance_count) {
    fbb_.AddElement<int32_t>(AttendanceCheckPacket::VT_UPDATED_ATTENDANCE_COUNT, updated_attendance_count, 0);
  }
  void add_cost_item_id(int32_t cost_item_id) {
    fbb_.AddElement<int32_t>(AttendanceCheckPacket::VT_COST_ITEM_ID, cost_item_id, 0);
  }
  void add_cost_item_count(int32_t cost_item_count) {
    fbb_.AddElement<int32_t>(AttendanceCheckPacket::VT_COST_ITEM_COUNT, cost_item_count, 0);
  }
  explicit AttendanceCheckPacketBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<AttendanceCheckPacket> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<AttendanceCheckPacket>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<AttendanceCheckPacket> CreateAttendanceCheckPacket(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int16_t request_result = 0,
    int8_t attendance_type = 0,
    int32_t event_key = 0,
    int32_t uuid = 0,
    int32_t updated_attendance_count = 0,
    int32_t cost_item_id = 0,
    int32_t cost_item_count = 0) {
  AttendanceCheckPacketBuilder builder_(_fbb);
  builder_.add_cost_item_count(cost_item_count);
  builder_.add_cost_item_id(cost_item_id);
  builder_.add_updated_attendance_count(updated_attendance_count);
  builder_.add_uuid(uuid);
  builder_.add_event_key(event_key);
  builder_.add_request_result(request_result);
  builder_.add_attendance_type(attendance_type);
  return builder_.Finish();
}

inline const FlatPacket::AttendancePacket::AttendanceCheckPacket *GetAttendanceCheckPacket(const void *buf) {
  return ::flatbuffers::GetRoot<FlatPacket::AttendancePacket::AttendanceCheckPacket>(buf);
}

inline const FlatPacket::AttendancePacket::AttendanceCheckPacket *GetSizePrefixedAttendanceCheckPacket(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<FlatPacket::AttendancePacket::AttendanceCheckPacket>(buf);
}

inline bool VerifyAttendanceCheckPacketBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FlatPacket::AttendancePacket::AttendanceCheckPacket>(nullptr);
}

inline bool VerifySizePrefixedAttendanceCheckPacketBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FlatPacket::AttendancePacket::AttendanceCheckPacket>(nullptr);
}

inline void FinishAttendanceCheckPacketBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatPacket::AttendancePacket::AttendanceCheckPacket> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedAttendanceCheckPacketBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatPacket::AttendancePacket::AttendanceCheckPacket> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace AttendancePacket
}  // namespace FlatPacket

#endif  // FLATBUFFERS_GENERATED_ATTENDANCECHECKPACKET_FLATPACKET_ATTENDANCEPACKET_H_
