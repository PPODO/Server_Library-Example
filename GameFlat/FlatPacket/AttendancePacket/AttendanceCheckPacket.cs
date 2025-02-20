// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace FlatPacket.AttendancePacket
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct AttendanceCheckPacket : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static AttendanceCheckPacket GetRootAsAttendanceCheckPacket(ByteBuffer _bb) { return GetRootAsAttendanceCheckPacket(_bb, new AttendanceCheckPacket()); }
  public static AttendanceCheckPacket GetRootAsAttendanceCheckPacket(ByteBuffer _bb, AttendanceCheckPacket obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool VerifyAttendanceCheckPacket(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, AttendanceCheckPacketVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public AttendanceCheckPacket __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public short RequestResult { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetShort(o + __p.bb_pos) : (short)0; } }
  public sbyte AttendanceType { get { int o = __p.__offset(6); return o != 0 ? __p.bb.GetSbyte(o + __p.bb_pos) : (sbyte)0; } }
  public int EventKey { get { int o = __p.__offset(8); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public int Uuid { get { int o = __p.__offset(10); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public int UpdatedAttendanceCount { get { int o = __p.__offset(12); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public FlatPacket.item? AttendanceRewards(int j) { int o = __p.__offset(14); return o != 0 ? (FlatPacket.item?)(new FlatPacket.item()).__assign(__p.__indirect(__p.__vector(o) + j * 4), __p.bb) : null; }
  public int AttendanceRewardsLength { get { int o = __p.__offset(14); return o != 0 ? __p.__vector_len(o) : 0; } }

  public static Offset<FlatPacket.AttendancePacket.AttendanceCheckPacket> CreateAttendanceCheckPacket(FlatBufferBuilder builder,
      short request_result = 0,
      sbyte attendance_type = 0,
      int event_key = 0,
      int uuid = 0,
      int updated_attendance_count = 0,
      VectorOffset attendance_rewardsOffset = default(VectorOffset)) {
    builder.StartTable(6);
    AttendanceCheckPacket.AddAttendanceRewards(builder, attendance_rewardsOffset);
    AttendanceCheckPacket.AddUpdatedAttendanceCount(builder, updated_attendance_count);
    AttendanceCheckPacket.AddUuid(builder, uuid);
    AttendanceCheckPacket.AddEventKey(builder, event_key);
    AttendanceCheckPacket.AddRequestResult(builder, request_result);
    AttendanceCheckPacket.AddAttendanceType(builder, attendance_type);
    return AttendanceCheckPacket.EndAttendanceCheckPacket(builder);
  }

  public static void StartAttendanceCheckPacket(FlatBufferBuilder builder) { builder.StartTable(6); }
  public static void AddRequestResult(FlatBufferBuilder builder, short requestResult) { builder.AddShort(0, requestResult, 0); }
  public static void AddAttendanceType(FlatBufferBuilder builder, sbyte attendanceType) { builder.AddSbyte(1, attendanceType, 0); }
  public static void AddEventKey(FlatBufferBuilder builder, int eventKey) { builder.AddInt(2, eventKey, 0); }
  public static void AddUuid(FlatBufferBuilder builder, int uuid) { builder.AddInt(3, uuid, 0); }
  public static void AddUpdatedAttendanceCount(FlatBufferBuilder builder, int updatedAttendanceCount) { builder.AddInt(4, updatedAttendanceCount, 0); }
  public static void AddAttendanceRewards(FlatBufferBuilder builder, VectorOffset attendanceRewardsOffset) { builder.AddOffset(5, attendanceRewardsOffset.Value, 0); }
  public static VectorOffset CreateAttendanceRewardsVector(FlatBufferBuilder builder, Offset<FlatPacket.item>[] data) { builder.StartVector(4, data.Length, 4); for (int i = data.Length - 1; i >= 0; i--) builder.AddOffset(data[i].Value); return builder.EndVector(); }
  public static VectorOffset CreateAttendanceRewardsVectorBlock(FlatBufferBuilder builder, Offset<FlatPacket.item>[] data) { builder.StartVector(4, data.Length, 4); builder.Add(data); return builder.EndVector(); }
  public static VectorOffset CreateAttendanceRewardsVectorBlock(FlatBufferBuilder builder, ArraySegment<Offset<FlatPacket.item>> data) { builder.StartVector(4, data.Count, 4); builder.Add(data); return builder.EndVector(); }
  public static VectorOffset CreateAttendanceRewardsVectorBlock(FlatBufferBuilder builder, IntPtr dataPtr, int sizeInBytes) { builder.StartVector(1, sizeInBytes, 1); builder.Add<Offset<FlatPacket.item>>(dataPtr, sizeInBytes); return builder.EndVector(); }
  public static void StartAttendanceRewardsVector(FlatBufferBuilder builder, int numElems) { builder.StartVector(4, numElems, 4); }
  public static Offset<FlatPacket.AttendancePacket.AttendanceCheckPacket> EndAttendanceCheckPacket(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<FlatPacket.AttendancePacket.AttendanceCheckPacket>(o);
  }
  public static void FinishAttendanceCheckPacketBuffer(FlatBufferBuilder builder, Offset<FlatPacket.AttendancePacket.AttendanceCheckPacket> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedAttendanceCheckPacketBuffer(FlatBufferBuilder builder, Offset<FlatPacket.AttendancePacket.AttendanceCheckPacket> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class AttendanceCheckPacketVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*RequestResult*/, 2 /*short*/, 2, false)
      && verifier.VerifyField(tablePos, 6 /*AttendanceType*/, 1 /*sbyte*/, 1, false)
      && verifier.VerifyField(tablePos, 8 /*EventKey*/, 4 /*int*/, 4, false)
      && verifier.VerifyField(tablePos, 10 /*Uuid*/, 4 /*int*/, 4, false)
      && verifier.VerifyField(tablePos, 12 /*UpdatedAttendanceCount*/, 4 /*int*/, 4, false)
      && verifier.VerifyVectorOfTables(tablePos, 14 /*AttendanceRewards*/, FlatPacket.itemVerify.Verify, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
